#ifndef FCENGINEEDITOR_SPLINEGLWINDOW_H__INCLUDE
#define FCENGINEEDITOR_SPLINEGLWINDOW_H__INCLUDE

#include "FCEditorGLWindow.h"

struct Spline;
class CSplineGLWindow : public CFCEditorGLWindow
{
    typedef CFCEditorGLWindow super;
public:
    CSplineGLWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("FCEditorGLWindow"));
    virtual ~CSplineGLWindow();

protected:
    void OnMouse(wxMouseEvent& event);

private:
    SharePtr<Spline> m_spline;
    int              m_selectedIndex;

    DECLARE_EVENT_TABLE()
};
#endif