#include "stdafx.h"
#include "GL/glew.h"
#include "glfw3.h"
#include "GLCanvas.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include <mmsystem.h>
#include "PublicDef.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Spline/Curve.h"

#include "Render/RenderManager.h"
#include "Render/RenderObjectManager.h"
#include "Render/AnimationManager.h"
#include "Render/RenderPublic.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Render/SpriteFrame.h"
#include "Render/SpriteAnimation.h"
#include "Render/Sprite.h"
#include "Render/AnimationController.h"
#include "Render/Camera.h"

#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"

BEGIN_EVENT_TABLE(GLAnimationCanvas, wxGLCanvas)
    EVT_SIZE(GLAnimationCanvas::OnSize)
    EVT_PAINT(GLAnimationCanvas::OnPaint)
    EVT_KEY_DOWN(GLAnimationCanvas::OnKeyDown)
    EVT_KEY_UP(GLAnimationCanvas::OnKeyUp)
    EVT_MOUSE_EVENTS(GLAnimationCanvas::OnMouse)
    EVT_MOUSE_CAPTURE_LOST(GLAnimationCanvas::MouseCaptureLost)
    EVT_IDLE(GLAnimationCanvas::OnIdle)
    END_EVENT_TABLE()

    GLAnimationCanvas::GLAnimationCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos,
    const wxSize& size, long style,
    const wxString& name)
    : wxGLCanvas(parent, id, NULL, pos, size,
    style | wxFULL_REPAINT_ON_RESIZE, name)
    , m_iType(0)
    , m_selectedIndex(0)
    , m_bLeftDown(false)
    , m_bRightDown(false)
{
    for (int i = 0; i < 26; i++)
    {
        m_KeyStates.push_back(*new bool(false));
    }

    m_glRC = new wxGLContext(this);
}

GLAnimationCanvas::~GLAnimationCanvas()
{
    delete m_glRC;
}

void GLAnimationCanvas::ResetProjectionMode()
{
    if ( !IsShownOnScreen() )
        return;

    SetCurrent(*m_glRC);

    int w, h;
    GetClientSize(&w, &h);

    CRenderManager::GetInstance()->SetWindowSize(w, h);
}

void GLAnimationCanvas::InitGL()
{
    CRenderManager::GetInstance()->Initialize();
    m_Model = CRenderObjectManager::GetInstance()->CreateModel();
}

void GLAnimationCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // must always be here
    wxPaintDC dc(this);

    SetCurrent(*m_glRC);

    // Initialize OpenGL
    if (!m_gldata.initialized)
    {
        InitGL();

        ResetProjectionMode();
        m_gldata.initialized = true;
    }


    static DWORD last = timeGetTime();
    DWORD curr = timeGetTime();
    DWORD delta = curr - last;
    last = curr;
    CAnimationManager::GetInstance()->Update((float)delta/1000);
    CRenderManager::GetInstance()->Render();
    SwapBuffers();
}

void GLAnimationCanvas::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // Reset the OpenGL view aspect.
    // This is OK only because there is only one canvas that uses the context.
    // See the cube sample for that case that multiple canvases are made current with one context.
    ResetProjectionMode();
}

void GLAnimationCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void GLAnimationCanvas::OnMouse(wxMouseEvent& event)
{

    if (m_iType == TYPE_CURVE)
    {
        wxPoint wxpt = event.GetPosition();
        Spline::Point pt(wxpt.x, wxpt.y);
        if (event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            size_t index;
            if(m_spline->findKont(pt, index))
            {
                m_selectedIndex = index;
            }
            else
            {
                m_selectedIndex = -1;
            }
        }
        else if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            size_t index;
            if(m_spline->findKont(pt, index))
            {
                m_spline->removeKnot(index);
                m_selectedIndex = -1;
            }
            else
            {
                m_selectedIndex = m_spline->addKnots(pt);
            }
        }
        else if (event.Dragging())
        {
            if(m_selectedIndex >= 0)
            {
                m_spline->setKnot(m_selectedIndex, pt);    
            }
        }
    }
    else if (m_iType == TYPE_ANIMATION)
    {
        if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
        {
            ShowCursor(false);
            m_DownPosition = event.GetPosition();
            SetFocus();
            if (!HasCapture())
            {
                CaptureMouse();
            }
            m_bRightDown = true;
        }
        else if(event.ButtonUp(wxMOUSE_BTN_RIGHT))
        {
            ShowCursor(true);
            if (HasCapture())
            {
                ReleaseMouse(); 
            }
            ResetKeyStates();
            m_bRightDown = false;
        }
        else if(event.ButtonDown(wxMOUSE_BTN_LEFT))
        {
            ShowCursor(false);
            m_DownPosition = event.GetPosition();
            SetFocus();
            if (!HasCapture())
            {
                CaptureMouse();
            }
            m_bLeftDown = true;

        }
        else if(event.ButtonUp(wxMOUSE_BTN_LEFT))
        {
            ShowCursor(true);
            if (!HasCapture())
            {
                CaptureMouse();
            }
            ResetKeyStates();
            m_bLeftDown = false;
        }
        else if(event.Dragging())
        {
            wxPoint pnt = ClientToScreen(m_DownPosition);
            SetCursorPos(pnt.x, pnt.y);
            CRenderManager* pRenderMgr = CRenderManager::GetInstance();
            if (m_bRightDown)
            {
                pRenderMgr->GetCamera()->RotateX((event.GetPosition().x - m_DownPosition.x) * MOUSESPEED);
                pRenderMgr->GetCamera()->RotateY((event.GetPosition().y - m_DownPosition.y) * MOUSESPEED);
            }
            if (m_bLeftDown)
            {
                pRenderMgr->GetCamera()->Yaw((event.GetPosition().x - m_DownPosition.x) * MOUSESPEED);
                pRenderMgr->GetCamera()->Pitch((event.GetPosition().y - m_DownPosition.y) * MOUSESPEED);
            }
        }
        else if(event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
        {
            float fSpeed = SHIFTWHEELSPEED;
            if (event.GetWheelRotation() > 0)
            {
                CRenderManager::GetInstance()->UpdateCamera(fSpeed, MOVESTRAIGHT);
            }
            else if (event.GetWheelRotation() < 0)
            {
                fSpeed *= -1;
                CRenderManager::GetInstance()->UpdateCamera(fSpeed, MOVESTRAIGHT);
            }

        };

    }
}


void GLAnimationCanvas::MouseCaptureLost( wxMouseCaptureLostEvent& /*event*/ )
{
    ResetKeyStates();
    m_bRightDown = false;
    m_bLeftDown = false;
}


void GLAnimationCanvas::OnIdle(wxIdleEvent& WXUNUSED(event))
{
    static float fFrame = 0.0f;
    fFrame = fFrame + 0.016f;
    if ( fFrame > 0.1f )
    {
        fFrame = 0.0f;
        UpDateCamera();
        EditAnimationDialog* pDialog = (EditAnimationDialog*)GetParent();
        if (m_Model->GetAnimationController()->IsPlaying())
        {
            int iCur = m_Model->GetAnimationController()->GetCurrFrame();
            (pDialog->GetTimeBar())->SetCurrentCursor(iCur);
        }
    }
    Refresh(false);
}

void GLAnimationCanvas::SetType( int iType )
{
    m_iType = iType;
}

void GLAnimationCanvas::OnKeyDown( wxKeyEvent& event )
{
    if (m_bRightDown)
    {
        int iKet = event.GetKeyCode();
        SetKeyState(iKet , true);
    }  
}


void GLAnimationCanvas::OnKeyUp( wxKeyEvent& event )
{
    if (m_bRightDown)
    {
        int iKet = event.GetKeyCode();
        SetKeyState(iKet , false);
    }
}


bool GLAnimationCanvas::GetKeyState( int iKey )
{
    bool bKey = false;
    switch (iKey)
    {
    case KEY_A:
        bKey = m_KeyStates[KEY_A];
        break;
    case KEY_S:
        bKey = m_KeyStates[KEY_S];
        break;
    case KEY_D:
        bKey = m_KeyStates[KEY_D];
        break;
    case KEY_W:
        bKey = m_KeyStates[KEY_W];
        break;
    case KEY_Q:
        bKey = m_KeyStates[KEY_Q];
        break;
    case KEY_Z:
        bKey = m_KeyStates[KEY_Z];
        break;
    case KEY_SHIFT:
        bKey = m_KeyStates[KEY_SHIFT];
        break;
    case KEY_CTRL:
        bKey = m_KeyStates[KEY_CTRL];
        break;
    default:
        break;
    }
    return bKey;
}

void GLAnimationCanvas::UpDateCamera()
{
    kmVec3 translateDis;
    kmVec3Fill(&translateDis, 0, 0, 0);

    float fSpeed = 0;
    int type = 0;
    if ( GetKeyState(KEY_SHIFT) )
    {
        fSpeed = SHIFTWHEELSPEED;
    }
    else
    {
        fSpeed = WHEELSPEED;
    }

    if ( GetKeyState(KEY_A) )
    {
        type = MOVETRANVERSE;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed* -1, type);
    }
    if ( GetKeyState(KEY_D) )
    {
        type = MOVETRANVERSE;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed , type);
    } 
    if ( GetKeyState(KEY_W) )
    {
        type = MOVESTRAIGHT;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed* -1, type);
    }
    if ( GetKeyState(KEY_S) )
    {
        type = MOVESTRAIGHT;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed , type);
    }
    if (GetKeyState(KEY_Q))
    {
        type = MOVEUPDOWN;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed, type);
    }
    if (GetKeyState(KEY_Z))
    {
        type = MOVEUPDOWN;
        CRenderManager::GetInstance()->UpdateCamera(fSpeed * -1, type);
    }
}

void GLAnimationCanvas::SetKeyState( int iKeyCode, bool bKey )
{
    int iKey = -1;
    switch (iKeyCode)
    {
    case 'A':
        iKey = KEY_A;
        break;
    case 'S':
        iKey = KEY_S;
        break;
    case 'D':
        iKey = KEY_D;
        break;
    case 'W':
        iKey = KEY_W;
        break;
    case 'Q':
        iKey = KEY_Q;
        break;
    case 'Z':
        iKey = KEY_Z;
        break;
    case WXK_SHIFT:
        iKey = KEY_SHIFT;
        break;
    case WXK_CONTROL:
        iKey = KEY_CTRL;
        break;
    default:
        break;
    }
    if (iKey != -1)
    {
        m_KeyStates[iKey] = bKey;
    }

}

void GLAnimationCanvas::ResetKeyStates()
{
    for (auto i : m_KeyStates)
    {
        i = false;
    }
}

SharePtr<CModel>& GLAnimationCanvas::GetModel()
{
    return m_Model;
}
