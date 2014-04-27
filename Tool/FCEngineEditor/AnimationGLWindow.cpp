#include "stdafx.h"
#include "AnimationGLWindow.h"
#include "Render/Model.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Render/AnimationController.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"

BEGIN_EVENT_TABLE(CAnimationGLWindow, CFCEditorGLWindow)
    EVT_IDLE(CAnimationGLWindow::OnIdle)
END_EVENT_TABLE()

CAnimationGLWindow::CAnimationGLWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : CFCEditorGLWindow(parent, pContext, id, pos, size, style, name)
{

}

CAnimationGLWindow::~CAnimationGLWindow()
{
}

void CAnimationGLWindow::OnIdle( wxIdleEvent& event )
{
    
    EditAnimationDialog* pDialog = (EditAnimationDialog*)GetParent();
    if (m_Model && m_Model->GetAnimationController()->IsPlaying())
    {
        int iCur = m_Model->GetAnimationController()->GetCurrFrame();
        pDialog->GetTimeBar()->SetCurrentCursor(iCur);
    }
    super::OnIdle(event);
}

void CAnimationGLWindow::SetModel( CModel* pModel )
{
    m_Model = pModel;
}

SharePtr<CModel>& CAnimationGLWindow::GetModel()
{
    return m_Model;
}
