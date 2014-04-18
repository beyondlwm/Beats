#include "stdafx.h"
#include "UIGLWindow.h"
#include "Render/Model.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Render/AnimationController.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "GUI/Window/WindowDefs.h"
#include "GUI/Animation/AnimationManager_ui.h"
#include "GUI/System.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/Camera.h"
#include "Render/RenderManager.h"
#include "GUI/WindowManager.h"

BEGIN_EVENT_TABLE(CUIGLWindow, CFCEditorGLWindow)
    EVT_IDLE(CUIGLWindow::OnIdle)
    EVT_CHAR(CUIGLWindow::OnChar)
    EVT_MOUSE_EVENTS(CUIGLWindow::OnMouse)
END_EVENT_TABLE()

CUIGLWindow::CUIGLWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
            : CFCEditorGLWindow(parent, pContext, id, pos, size, style, name)
            , m_pActiveGUI(NULL)
            , m_pTempGUI(NULL)
{

}

CUIGLWindow::~CUIGLWindow()
{
}

void CUIGLWindow::OnIdle( wxIdleEvent& event )
{
    static DWORD last = timeGetTime();
    DWORD curr = timeGetTime();
    DWORD delta = curr - last;
    float dt = (float)delta / 1000;
    if(dt > 1.f/60)
    {
        FCGUI::AnimationManager::GetInstance()->Update((float)delta/1000);
        FCGUI::System::GetInstance()->Update((float)delta/1000);
        RenderTempGUI();
        last = curr;
    }
    super::OnIdle(event);
}

void CUIGLWindow::OnChar( wxKeyEvent& /*event*/ )
{

}

void CUIGLWindow::OnMouse( wxMouseEvent& event )
{
    using namespace FCGUI;
    wxPoint MousePos = event.GetPosition();
    
    if(event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        m_pActiveGUI = WindowManager::GetInstance()->GetWindow(MousePos.x, MousePos.y);
        if (m_pActiveGUI)
        {
            kmVec2 kmVec2Pos = m_pActiveGUI->Pos();
            m_ActiveGUIPosOffset = wxPoint(kmVec2Pos.x - MousePos.x, kmVec2Pos.y - MousePos.y);
        }       
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        m_pActiveGUI = nullptr;
    }
    else if(event.Dragging())
    {      
        if (m_pActiveGUI)
        {
            m_pActiveGUI->SetPos(MousePos.x + m_ActiveGUIPosOffset.x, MousePos.y + m_ActiveGUIPosOffset.y);
        }
    }
}

void CUIGLWindow::RenderTempGUI()
{
    wxPoint clientPos = ScreenToClient(wxGetMousePosition());
    if (m_pTempGUI)
    {
        m_pTempGUI->SetPos(clientPos.x, clientPos.y);
    } 
}

void CUIGLWindow::SetTempGUI( FCGUI::Window* pTempGUI )
{
    m_pTempGUI = pTempGUI;
}

void CUIGLWindow::SetActiveGUI( FCGUI::Window* pActiveGUI )
{
    m_pActiveGUI = pActiveGUI;
}
