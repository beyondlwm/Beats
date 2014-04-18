#ifndef FCENGINEEDITOR_FCEDITORGLWINDOW_H__INCLUDE
#define FCENGINEEDITOR_FCEDITORGLWINDOW_H__INCLUDE

#include "wx/glcanvas.h"

class CCamera;
class CFCEditorGLWindow : public wxGLCanvas
{
public:
    CFCEditorGLWindow(wxWindow *parent,
                    wxGLContext* pShareContext = NULL,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0,
                    const wxString& name = wxT("FCEditorGLWindow"));

    virtual ~CFCEditorGLWindow();
    wxGLContext* GetGLContext() const;
    void SetGLContext(wxGLContext* pContext);
protected:
    void OnIdle(wxIdleEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse(wxMouseEvent& event);
    void ShowCursor();
    void HidwCursor();
protected:
    bool m_bLeftDown;
    bool m_bRightDown;

private:
    void InitGL();
    void UpdateCamera();

private:
    wxGLContext* m_glRC;
    CCamera* m_pCamera;
    wxPoint m_startPosition;

    wxDECLARE_NO_COPY_CLASS(CFCEditorGLWindow);
    DECLARE_EVENT_TABLE()
};
#endif