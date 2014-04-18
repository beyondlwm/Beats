#include "stdafx.h"
#include "FCEditorGLWindow.h"
#include "Render/RenderManager.h"
#include "Render/Renderer.h"
#include "Render/Camera.h"

BEGIN_EVENT_TABLE(CFCEditorGLWindow, wxGLCanvas)
    EVT_SIZE(OnSize)
    EVT_IDLE(OnIdle)
    EVT_ERASE_BACKGROUND(OnEraseBackground)
    EVT_MOUSE_EVENTS(OnMouse)
END_EVENT_TABLE()

CFCEditorGLWindow::CFCEditorGLWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxGLCanvas(parent, id, NULL, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name)
    , m_bLeftDown(false)
    , m_bRightDown(false)
    , m_glRC(pContext)
{
    m_pCamera = new CCamera;
}

CFCEditorGLWindow::~CFCEditorGLWindow()
{
    BEATS_SAFE_DELETE(m_pCamera);
}

wxGLContext* CFCEditorGLWindow::GetGLContext() const
{
    return m_glRC;
}

void CFCEditorGLWindow::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    if ( IsShownOnScreen() )
    {
        SetCurrent(*m_glRC);
    }
    CRenderManager::GetInstance()->SetCamera(m_pCamera);
    UpdateCamera();
    CRenderManager::GetInstance()->Render();
    SwapBuffers();
}

void CFCEditorGLWindow::OnSize(wxSizeEvent& event)
{
    if ( IsShownOnScreen() )
    {
        SetCurrent(*m_glRC);
    }
    kmVec2 kmVecSize;
    wxSize size = event.GetSize();
    kmVecSize.x = size.GetWidth();
    kmVecSize.y = size.GetHeight();
    CRenderManager::GetInstance()->SetWindowSize(kmVecSize.x, kmVecSize.y);
    m_pCamera->SetWidth(size.GetWidth());
    m_pCamera->SetHeight(size.GetHeight());
}

void CFCEditorGLWindow::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void CFCEditorGLWindow::OnMouse(wxMouseEvent& event)
{
    if(event.ButtonDown(wxMOUSE_BTN_RIGHT))
    {
        if (!HasCapture())
        {
            CaptureMouse();
        }
        HidwCursor();
        SetFocus();
        m_startPosition = event.GetPosition();
        m_bRightDown = true;
    }
    else if(event.ButtonUp(wxMOUSE_BTN_RIGHT))
    {
        if (!m_bLeftDown && HasCapture())
        {
            ReleaseMouse();
        }
        ShowCursor();
        m_bRightDown = false;
    }
    else if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        if (!HasCapture())
        {
            CaptureMouse();
        }
        HidwCursor();
        SetFocus();
        m_startPosition = event.GetPosition();
        m_bLeftDown = true;
    }
    else if(event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        if (!m_bRightDown && HasCapture())
        {
            ReleaseMouse();
        }
        ShowCursor();
        m_bLeftDown = false;
    }
    else if(event.Dragging())
    {
        if (m_bRightDown || m_bLeftDown)
        {
            wxPoint curPos = event.GetPosition();
            wxPoint pnt = ClientToScreen(m_startPosition);
            SetCursorPos(pnt.x, pnt.y);
            if (m_bRightDown)
            {
                int nDeltaX = curPos.x - m_startPosition.x;
                int nDeltaY = curPos.y - m_startPosition.y;
                wxSize clientSize = GetClientSize();
                m_pCamera->Yaw((float)nDeltaX / clientSize.x);
                m_pCamera->Pitch((float)nDeltaY / clientSize.y);
            }
        }
        
    }
    else if(event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
    {
        kmVec3 vec3Speed;
        kmVec3Fill(&vec3Speed, SHIFTWHEELSPEED, SHIFTWHEELSPEED, SHIFTWHEELSPEED);
        if (event.GetWheelRotation() > 0)
        {
            m_pCamera->Update(vec3Speed, (1 << CCamera::eCMT_STRAIGHT));
        }
        else if (event.GetWheelRotation() < 0)
        {
            kmVec3Scale(&vec3Speed, &vec3Speed, -1.0f);
            m_pCamera->Update(vec3Speed, (1 << CCamera::eCMT_STRAIGHT));
        }
    }
    
}

void CFCEditorGLWindow::UpdateCamera()
{
    kmVec3 vec3Speed;
    kmVec3Fill(&vec3Speed, 1.0f, 1.0f, 1.0f);
    if ( (::GetKeyState(VK_SHIFT) & 0x8000) > 0 )
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 5.0F * 0.016F);
    }
    else
    {
        kmVec3Scale(&vec3Speed, &vec3Speed, 1.0F * 0.016F);
    }
    int type = CCamera::eCMT_NOMOVE;

    bool bPressA = (::GetKeyState('A') & 0x8000) > 0;
    bool bPressD = (::GetKeyState('D') & 0x8000) > 0;
    if ( bPressA || bPressD )
    {
        type |= (1 << CCamera::eCMT_TRANVERSE);
        if (bPressA)
        {
            vec3Speed.x *= -1;
        }
    }
    bool bPressW = (::GetKeyState('W') & 0x8000) > 0;
    bool bPressS = (::GetKeyState('S') & 0x8000) > 0;

    if ( bPressW || bPressS )
    {
        type |= (1 << CCamera::eCMT_STRAIGHT);
        if (bPressW)
        {
            vec3Speed.z *= -1;
        }
    }
    bool bPressUp = (::GetKeyState('Q') & 0x8000) > 0;
    bool bPressDown = (::GetKeyState('Z') & 0x8000) > 0;

    if ( bPressUp || bPressDown )
    {
        type |= (1 << CCamera::eCMT_UPDOWN);
        if (bPressDown)
        {
            vec3Speed.y *= -1;
        }
    }

    if (type != CCamera::eCMT_NOMOVE)
    {
        m_pCamera->Update(vec3Speed, type);
    }
}

void CFCEditorGLWindow::SetGLContext( wxGLContext* pContext )
{
    m_glRC = pContext;
}

void CFCEditorGLWindow::ShowCursor()
{
    int iCount = ::ShowCursor(true);
    while (iCount < 0)
    {
        iCount = ::ShowCursor(true);
    }
}

void CFCEditorGLWindow::HidwCursor()
{
    int iCount = ::ShowCursor(false);
    while (iCount >= 0)
    {
        iCount = ::ShowCursor(false);
    }
}
