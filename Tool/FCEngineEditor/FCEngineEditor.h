#ifndef FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE
#define FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE

#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "EngineProperGridManager.h"
#include "GLCanvas.h"


class TimeBarFrame;
class CEditorMainFrame : public wxFrame
{
    enum
    {
        ID_CustomizeToolbar = wxID_HIGHEST+1,
        ID_ToolBar1_Radio1,
        ID_ToolBar1_Radio2,
        ID_ToolBar1_Radio3,
        ID_ToolBar1_Radio4,
        ID_ToolBar2_Button1,
        ID_ToolBar2_Button2

    };
public:
    CEditorMainFrame(const wxString& title);
    virtual ~CEditorMainFrame();
    void InitMenu();
    void InitCtrls();
    void CreateAuiToolBar();
    void CreateAuiNoteBook();
    void CreateTreeCtrl();
    void CreateGLCanvas();
    void CreateTimeBar();
    void CreatePropertyGrid();
    void AddPageToBook();

    void OnAuiButton(wxCommandEvent& event);
    void OnEditAnimationMenuItem(wxCommandEvent& event);

    void GetEditAnimationDialog();
private:

    wxAuiNotebook*      m_pLeft;
    wxAuiNotebook*      m_pRight;
    wxAuiNotebook*      m_pBottom;
    wxAuiNotebook*      m_pCenter;
    wxTreeCtrl*         m_pTreeCtrl1;
    wxAuiManager        m_Manager;
    GLAnimationCanvas*  m_pGLCanvas;
    TimeBarFrame*       m_pTimeBar;
    EnginePropertyGirdManager* m_pPropGridManager;
    wxPropertyGrid*     m_propGrid;
    wxDialog*           m_pWEditAnimation;
DECLARE_EVENT_TABLE();
};

#endif