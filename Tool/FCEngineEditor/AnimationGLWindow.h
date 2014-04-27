#ifndef FCENGINEEDITOR_ANIMATIONGLWINDOW_H__INCLUDE
#define FCENGINEEDITOR_ANIMATIONGLWINDOW_H__INCLUDE

#include "FCEditorGLWindow.h"

class CModel;
class CAnimationGLWindow : public CFCEditorGLWindow
{
    typedef CFCEditorGLWindow super;
public:
    CAnimationGLWindow(wxWindow *parent,
        wxGLContext* pShareContext = NULL,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("FCEditorGLWindow"));
    virtual ~CAnimationGLWindow();

    void SetModel(CModel* pModel);
    SharePtr<CModel>& GetModel();

protected:
    void OnIdle(wxIdleEvent& event);

private:
    SharePtr<CModel> m_Model;


    DECLARE_EVENT_TABLE()
};
#endif