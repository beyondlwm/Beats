#ifndef FCENGINEEDITOR_EditUIDialog_H__INCLUDE
#define FCENGINEEDITOR_EditUIDialog_H__INCLUDE

#include "EditDialogBase.h"
#include <wx/dialog.h>
#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include "GUI/Window/Button.h"

class TimeBarDataViewCtrl;
class CUIGLWindow;
class EnginePropertyGirdManager;
class wxPropertyGrid;
class TimeBarFrame;
class CTexture;
class EditUIDialog : public EditDialogBase
{
    enum
    {
        ID_CustomizeToolbar = wxID_HIGHEST+1,
        ID_ToolBar1_Radio1,
        ID_ToolBar1_Radio2,
        ID_ToolBar1_Radio3,
        ID_ToolBar1_Radio4,
        ID_ToolBar2_Button1,
        ID_ToolBar2_Button2,
        ID_DataView_ObjectView

    };
    typedef EditDialogBase super;
public:
    EditUIDialog(wxWindow *parent, wxWindowID id, const wxString &title
        , const wxPoint &pos = wxDefaultPosition
        , const wxSize &size = wxDefaultSize
        , long style =  wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION
        , const wxString &name = wxDialogNameStr);

    virtual ~EditUIDialog();
    virtual void LanguageSwitch();

    void InitCtrls();
    void CreateAuiToolBar();
    void CreateMenu();
    void CreateAuiNoteBook();
    void CreatePropertyGrid();
    void CreateCtrlsTree();
    void RemoveSelectedGUI();
    void CreateGUITexture(const TString& strFilePath);

protected:
    void OnToolBarDrag(wxAuiToolBarEvent& event);
    void OnCtrlsTreeBeginDrag(wxTreeEvent& event);
    void OnCtrlsTreeEndDrag(wxTreeEvent& event);
    void OnObjectViewSeletionChange(wxDataViewEvent& event);
    void OnRightClick(wxDataViewEvent& event);
    void OnPopMenu(wxCommandEvent& event);
    void OnDataViewRenderer(TimeBarEvent& event);
    TString GetAvailableName(const TString& strWindowName);

private:
    wxAuiManager            m_Manager;
    wxPanel*                m_pCanvasPanel;
    wxAuiNotebook*          m_pCanvasNotebook;
    wxPanel*                m_pObjectViewPanel;
    wxAuiNotebook*          m_pObjectViewNotebook;
    wxPanel*                m_pGLWindowPanel;
    wxAuiNotebook*          m_pGLWindowNotebook;
    wxPanel*                m_pPropertyPanel;
    wxAuiNotebook*          m_pPropertyNotebook;
    wxPanel*                m_pTimeBarPanel;
    wxAuiNotebook*          m_pTimeBarNotebook;
    wxPanel*                m_pCtrlsPanel;
    wxAuiNotebook*          m_pCtrlsNotebook;

    FCGUI::Window*          m_pTempGUI;
    FCGUI::Window*          m_pSelectedGUI;
    wxMenu*                 m_pObjectViewMenu;
    SharePtr<CTexture>      m_texture;
    wxListBox*              m_pCanvasList;
    wxTreeCtrl*             m_pCtrlsTree;
    CUIGLWindow*            m_pGLWindow;
    TimeBarDataViewCtrl*    m_pObjectView;
    wxPropertyGrid*         m_pPropGrid;
    TimeBarFrame*           m_pTimeBar;
    EnginePropertyGirdManager* m_pPropGridManager;

    DECLARE_EVENT_TABLE()
};

#endif