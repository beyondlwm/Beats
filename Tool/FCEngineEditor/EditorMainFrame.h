#ifndef FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE
#define FCENGINEEDITOR_FCENGINEEDITOR_H__INCLUDE

#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include "EngineProperGridManager.h"
#include "GLCanvas.h"
#include "wx/splitter.h"

class wxTreeCtrl;
class wxSearchCtrl;
class wxSplitterWindow;
class TimeBarFrame;
class CComponentGraphic;
class CFCEditorGLWindow;
class CFCEditorComponentWindow;
class EditDialogBase;
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
    void InitFrame();
    void InitMenu();
    void InitCtrls();
    void CreateAuiToolBar();
    void CreateAuiNoteBook();
    void CreateTreeCtrl();
    void CreateTimeBar();
    void CreatePropertyGrid();
    void CreatSplitter();
    void AddPageToBook();
    void AddTreeItem();
    void UpdatePropertyGrid();
    void CloseProjectFile();
    void OpenProject();
    void CloseProject();
    void Export();
    void SaveProject();
    void InitComponentsPage();
    void GetEditAnimationDialog();
    void GetEditLanguageDialog();
    void GetEditUIDialog();
    void AddComponentFile();
    void AddComponentFileFolder();
    void OpenComponentFileDirectory();
    void DeleteComponentFile();
    void OpenProjectFile( const TCHAR* pPath );
    void ActivateFile(const TCHAR* pszFileName);
    void OpenComponentFile( const TCHAR* pFilePath );
    void CloseComponentFile(bool bRemindSave = true);
    void SelectComponent(CComponentEditorProxy* pComponentInstance);
    void InitializeComponentFileTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id);
    void ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap);
    void DeleteItemInComponentFileList(wxTreeItemId itemId, bool bDeletePhysicalFile);
    CComponentEditorProxy* GetSelectedComponent();
    void LanguageSwitch(int id);
    void ChangeLanguage();
    
protected:
    void OnActivateComponentFile(wxTreeEvent& event);
    void OnRightClickComponentFileList( wxTreeEvent& event );
    void OnComponentFileStartDrag( wxTreeEvent& event );
    void OnComponentFileEndDrag( wxTreeEvent& event );
    void OnComponentStartDrag( wxTreeEvent& event );
    void OnComponentEndDrag( wxTreeEvent& event );
    void OnIdle(wxIdleEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnAuiButton(wxCommandEvent& event);
    void OnTemplateComponentItemChanged(wxTreeEvent& event);
    void OnSplitterSashDClick(wxSplitterEvent& event);
    
private:
    void AddChilditemToItem(wxTreeItemId& idParent, std::vector<TString>& vecName, size_t iLevel);

    wxMenuBar*          m_pMenuBar;
    wxMenu*             m_pFileMenu;
    wxMenu*             m_pEditMenu;
    wxMenu*             m_pAssetsMenu;
    wxMenu*             m_pGameObjMenu;
    wxMenu*             m_pConponentMenu;
    wxMenu*             m_pWindowMenu;
    wxMenu*             m_pHelpMenu;
    wxMenu*             m_pHelpLanguageMenu;

    bool                m_bIsLanguageSwitched;

    wxPanel*            m_pLeftPanel;
    wxAuiNotebook*      m_pLeft;
    wxPanel*            m_pRightPanel;
    wxAuiNotebook*      m_pRight;
    wxPanel*            m_pBottomPanel;
    wxAuiNotebook*      m_pBottom;
    wxPanel*            m_pCenterPanel;
    wxAuiNotebook*      m_pCenter;

    wxMenu*             m_pComponentFileMenu;
    wxSearchCtrl*       m_pSearch;
    wxTreeCtrl*         m_pComponentFileTC;
    wxTreeCtrl*         m_pComponentTC;
    wxAuiManager        m_Manager;
    TimeBarFrame*       m_pTimeBar;
    EnginePropertyGirdManager* m_pPropGridManager;
    wxPropertyGrid*     m_propGrid;
    EditDialogBase*           m_pWEditAnimation;
    EditDialogBase*           m_pWEditLanguage;
    EditDialogBase*           m_pWEditUI;
    wxSplitterWindow*   m_pSplitter;
    CComponentEditorProxy* m_pSelectedComponent;
    CFCEditorGLWindow*  m_pSplTop;
    CFCEditorComponentWindow*  m_pComponentRenderWindow;
    std::map<size_t, wxTreeItemId> m_componentTreeIdMap; //use this map to quick find the wxItemId for a specific id of component
    std::map<TString, wxTreeItemId> m_componentCatalogNameMap; //use this map to quick find the wxItemId for a catalog
    std::map<TString, wxTreeItemId> m_componentFileListMap; //use this map to quick find the wxItemId for a component file name.

DECLARE_EVENT_TABLE();
};

#endif