#ifndef FCENGINEEDITOR_UIGLWINDOW_H__INCLUDE
#define FCENGINEEDITOR_UIGLWINDOW_H__INCLUDE

#include "FCEditorGLWindow.h"
#include "GUI\Window\Button.h"

class FCGUI::Window;
class CUIGLWindow : public CFCEditorGLWindow
{
    typedef CFCEditorGLWindow super;
public:
    CUIGLWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("FCEditorGLWindow"));
    virtual ~CUIGLWindow();

    void SetTempGUI(FCGUI::Window* pTempGUI);
    void SetActiveGUI(FCGUI::Window* pActiveGUI);
protected:
    void RenderTempGUI();
    void OnChar(wxKeyEvent& event);
    void OnIdle( wxIdleEvent& event );
    void OnMouse(wxMouseEvent& event);

private:
    FCGUI::Window*          m_pActiveGUI;
    FCGUI::Window*          m_pTempGUI;
    wxPoint                 m_ActiveGUIPosOffset;
    DECLARE_EVENT_TABLE()
};
#endif