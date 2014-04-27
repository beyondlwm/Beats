#include "stdafx.h"
#include "SplineGLWindow.h"
#include "Spline\Curve.h"

BEGIN_EVENT_TABLE(CSplineGLWindow, CFCEditorGLWindow)
    EVT_MOUSE_EVENTS(CSplineGLWindow::OnMouse)
END_EVENT_TABLE()

CSplineGLWindow::CSplineGLWindow(wxWindow *parent, wxGLContext* pContext, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: CFCEditorGLWindow(parent, pContext, id, pos, size, style, name)
{

}

CSplineGLWindow::~CSplineGLWindow()
{
}

void CSplineGLWindow::OnMouse( wxMouseEvent& event )
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
