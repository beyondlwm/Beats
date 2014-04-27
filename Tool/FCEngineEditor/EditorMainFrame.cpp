#include "stdafx.h"
#include "EditorMainFrame.h"
#include "ConstantCurveProperty.h"
#include "ConstantCurveEditor.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "FCEditorGLWindow.h"
#include "EngineEditor.h"
#include "ComponentFileTreeItemData.h"
#include "FCEditorComponentWindow.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "ComponentTreeItemData.h"
#include "Utility/TinyXML/tinyxml.h"
#include "ComponentGraphics_GL.h"
#include "Resource/ResourcePathManager.h"
#include "EditLanguageDialog.h"
#include "EditUIDialog.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProject.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentGraphic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentProjectData.h"

#include <wx/artprov.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/numdlg.h>
#include <wx/srchctrl.h>


#define MAINFRAMESIZE wxSize(1024, 800)
#define MAINFRAMEPOSITION wxPoint(40, 40)
#define BUTTONSIZE wxSize(20, 20)
#define SIZERBORDERWIDTH 5
#define IDBEGIN 1111

enum 
{
    PARTICLE_SIMULATION_WORLD = 1,
    PARTICLE_SIMULATION_LOCAL
};

enum ETreeCtrlIconType
{
    eTCIT_File,
    eTCIT_FileSelected,
    eTCIT_Folder,
    eTCIT_FolderSelected,
    eTCIT_FolderOpened,
    eTCIT_Count,
    eTCIT_Force32Bit = 0xFFFFFFFF
};

enum
{
    eMB_File,
    eMB_Edit,
    eMB_Assets,
    eMB_GameObj,
    eMB_Conponent,
    eMB_Window,
    eMB_Help,
};
enum MenuID
{
    Menu_File_Open,
    Menu_File_Close,
    Menu_File_Save,
    Menu_File_Export,

    Menu_Edit_Animation,
    Menu_Edit_UI,
    Menu_Edit_Language,

    Menu_Help_Language,
    Menu_Help_Language_Chinese,
    Menu_Help_Language_English,

    eFLMS_AddFile,
    eFLMS_AddFileFolder,
    eFLMS_Delete,
    eFLMS_OpenFileDirectory,

    MenuID_Count,
    MenuID_Force32bit = 0xffffffff
};

enum CtrlID
{
    Ctrl_Tree_CompontentFile,
    Ctrl_Tree_Compontent
};

BEGIN_EVENT_TABLE(CEditorMainFrame, wxFrame)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, CEditorMainFrame::OnAuiButton)
EVT_TREE_ITEM_ACTIVATED(Ctrl_Tree_CompontentFile, CEditorMainFrame::OnActivateComponentFile)
EVT_TREE_BEGIN_DRAG(Ctrl_Tree_CompontentFile,CEditorMainFrame::OnComponentFileStartDrag)
EVT_TREE_END_DRAG(Ctrl_Tree_CompontentFile,CEditorMainFrame::OnComponentFileEndDrag)
EVT_TREE_ITEM_RIGHT_CLICK(Ctrl_Tree_CompontentFile,CEditorMainFrame::OnRightClickComponentFileList)
EVT_TREE_BEGIN_DRAG(Ctrl_Tree_Compontent,CEditorMainFrame::OnComponentStartDrag)
EVT_TREE_END_DRAG(Ctrl_Tree_Compontent,CEditorMainFrame::OnComponentEndDrag)
EVT_TREE_SEL_CHANGED(Ctrl_Tree_Compontent,CEditorMainFrame::OnTemplateComponentItemChanged)
EVT_SEARCHCTRL_SEARCH_BTN(wxID_ANY, CEditorMainFrame::OnSearch)
EVT_SPLITTER_DCLICK(wxID_ANY, CEditorMainFrame::OnSplitterSashDClick)
EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, CEditorMainFrame::OnSplitterSashDClick)
EVT_TEXT_ENTER(wxID_ANY, CEditorMainFrame::OnSearch)
EVT_IDLE(CEditorMainFrame::OnIdle)
END_EVENT_TABLE()

CEditorMainFrame::CEditorMainFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE)
    , m_pWEditAnimation(NULL)
    , m_pWEditLanguage(NULL)
    , m_bIsLanguageSwitched(false)
    ,m_pWEditUI(NULL)
{
    SetIcon(wxICON(sample));
}

CEditorMainFrame::~CEditorMainFrame()
{
    BEATS_SAFE_DELETE(m_pComponentFileMenu);
    m_Manager.UnInit();
}

void CEditorMainFrame::InitMenu()
{
    m_pMenuBar              = new wxMenuBar;
    m_pFileMenu             = new wxMenu;
    m_pEditMenu             = new wxMenu;
    m_pAssetsMenu           = new wxMenu;
    m_pGameObjMenu          = new wxMenu;
    m_pConponentMenu        = new wxMenu;
    m_pWindowMenu           = new wxMenu;
    m_pHelpMenu             = new wxMenu;
    m_pHelpLanguageMenu     = new wxMenu;
    m_pComponentFileMenu    = new wxMenu;

    m_pComponentFileMenu->Append(eFLMS_AddFile, _T("添加文件"));
    m_pComponentFileMenu->Append(eFLMS_AddFileFolder, _T("添加文件夹"));
    m_pComponentFileMenu->Append(eFLMS_Delete, _T("删除文件"));
    m_pComponentFileMenu->Append(eFLMS_OpenFileDirectory, _T("打开所在的文件夹"));

    m_pMenuBar->Append(m_pFileMenu, wxT("&File"));
    m_pMenuBar->Append(m_pEditMenu, wxT("&Edit"));
    m_pMenuBar->Append(m_pAssetsMenu, wxT("&Assets"));
    m_pMenuBar->Append(m_pGameObjMenu, wxT("&GameObject"));
    m_pMenuBar->Append(m_pConponentMenu, wxT("&Conponent"));
    m_pMenuBar->Append(m_pWindowMenu, wxT("&Window"));
    m_pMenuBar->Append(m_pHelpMenu, wxT("&Help"));

    m_pFileMenu->Append(Menu_File_Open, wxT("Open"));
    m_pFileMenu->Append(Menu_File_Close, wxT("Close"));
    m_pFileMenu->Append(Menu_File_Save, wxT("&Save"));
    m_pFileMenu->Append(Menu_File_Export, wxT("Export"));
    m_pEditMenu->Append(Menu_Edit_Animation, wxT("Edit Animation"));
    m_pEditMenu->Append(Menu_Edit_UI, wxT("Edit UI"));
    m_pEditMenu->Append(Menu_Edit_Language, wxT("Edit Language"));
    m_pHelpMenu->Append(Menu_Help_Language, wxT("Language"), m_pHelpLanguageMenu);

    m_pHelpLanguageMenu->Append(Menu_Help_Language_Chinese, wxT("Chinese"));
    m_pHelpLanguageMenu->Append(Menu_Help_Language_English, wxT("English"));

    SetMenuBar(m_pMenuBar);
}

void CEditorMainFrame::InitCtrls()
{
    CreateAuiToolBar();
    CreateAuiNoteBook();
    CreateTreeCtrl();
    CreateTimeBar();
    CreatePropertyGrid();
    CreatSplitter();

    AddPageToBook();
    AddTreeItem();
    m_Manager.Update();
}

void CEditorMainFrame::CreateAuiToolBar()
{
    wxAuiToolBarItemArray prepend_items;
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);

    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->SetToolBitmapSize(wxSize(16,16));
    tb1->AddTool(ID_ToolBar1_Radio1, wxT("Radio 1"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 1"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio2, wxT("Radio 2"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 2"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio3, wxT("Radio 3"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 3"), wxITEM_RADIO);
    tb1->AddTool(ID_ToolBar1_Radio4, wxT("Radio 4"), wxArtProvider::GetBitmap(wxART_QUESTION), wxT("Radio 4"), wxITEM_RADIO);
    tb1->SetCustomOverflowItems(prepend_items, append_items);
    tb1->Realize();

    wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
    tb2->SetToolBitmapSize(wxSize(16,16));
    tb2->AddTool(ID_ToolBar2_Button1, wxT("Disabled"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb2->AddTool(ID_ToolBar2_Button2, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb2->SetCustomOverflowItems(prepend_items, append_items);
    tb2->Realize();

    m_Manager.AddPane(tb1,
        wxAuiPaneInfo().Name(wxT("tb1")).
        ToolbarPane().Top().Position(0).
        LeftDockable(false).RightDockable(false).Dockable(false));
    m_Manager.AddPane(tb2,
        wxAuiPaneInfo().Name(wxT("tb2")).
        ToolbarPane().Top().Position(1).
        LeftDockable(false).RightDockable(false).Dockable(false));
}

void CEditorMainFrame::CreateAuiNoteBook()
{
    wxSize client_size = GetClientSize();
    long lStyle = wxAUI_NB_DEFAULT_STYLE & ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;

    client_size = wxSize(client_size.GetWidth() * 0.12, client_size.GetHeight() * 0.15);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pLeftPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pLeftPanel->SetSizer(pSizer);
    m_pLeft = new wxAuiNotebook(m_pLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pLeft, 1, wxGROW|wxALL, 0);
    m_pSearch = new wxSearchCtrl(m_pLeftPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    pSizer->Add(m_pSearch, 0, wxGROW|wxALL, 0);
    m_pSearch->ShowCancelButton(true);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pRightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pRightPanel->SetSizer(pSizer);
    m_pRight = new wxAuiNotebook(m_pRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pRight, 1, wxGROW|wxALL, 0);

    client_size = wxSize(client_size.GetWidth(), client_size.GetHeight() * 1.3);
    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pBottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pBottomPanel->SetSizer(pSizer);
    m_pBottom = new wxAuiNotebook(m_pBottomPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pBottom, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCenterPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_pCenterPanel->SetSizer(pSizer);
    m_pCenter = new wxAuiNotebook(m_pCenterPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pCenter, 1, wxGROW|wxALL, 0);
}

void CEditorMainFrame::CreateTimeBar()
{
    m_pTimeBar = new TimeBarFrame(m_pBottom);
}

void CEditorMainFrame::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new EnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pRight, wxID_ANY, wxDefaultPosition, wxSize(100, 100), style );
    m_propGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_propGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));
}

void CEditorMainFrame::CreateTreeCtrl()
{
    m_pComponentFileTC = new wxTreeCtrl(m_pLeft, Ctrl_Tree_CompontentFile, wxPoint(0,0), wxSize(160,250), wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
    wxImageList *pFileIconImages = new wxImageList(15, 15, true);
    wxIcon fileIcons[eTCIT_Count];
    wxSize iconSize(15, 15);
    fileIcons[eTCIT_File] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_LIST, iconSize);
    fileIcons[eTCIT_FileSelected] = wxArtProvider::GetIcon(wxART_FILE_OPEN, wxART_LIST, iconSize);
    fileIcons[eTCIT_Folder] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderSelected] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_LIST, iconSize);
    fileIcons[eTCIT_FolderOpened] = wxArtProvider::GetIcon(wxART_FOLDER_OPEN, wxART_LIST, iconSize);
    for (size_t i = 0; i < eTCIT_Count; ++i)
    {
        pFileIconImages->Add(fileIcons[i]);
    }
    m_pComponentFileTC->AssignImageList(pFileIconImages);
    m_pComponentFileTC->AddRoot(wxT("Component File"), eTCIT_Folder, -1, NULL);

    m_pComponentTC = new wxTreeCtrl(m_pLeft, Ctrl_Tree_Compontent, wxPoint(0,0), wxSize(160,250), wxTR_DEFAULT_STYLE | wxNO_BORDER);    
    wxTreeItemId rootId = m_pComponentTC->AddRoot(wxT("Components"), eTCIT_Folder, -1, NULL);
    m_componentCatalogNameMap[_T("Root")] = rootId;
    m_componentTreeIdMap[0] = rootId;
    m_pComponentTC->Hide();
}

void CEditorMainFrame::AddPageToBook()
{
    m_pLeft->Freeze();
    m_pLeft->AddPage( m_pComponentFileTC, wxT("Component Model") );
    m_pLeft->Thaw();

    m_pRight->Freeze();
    m_pRight->AddPage( m_pPropGridManager, wxT("Inspector") );
    m_pRight->Thaw();

    m_pBottom->Freeze();
    m_pBottom->AddPage( new wxTextCtrl( m_pBottom, wxID_ANY, wxT("Some more text")), wxT("wxTextCtrl 1") );
    m_pBottom->AddPage( m_pTimeBar, wxT("wxTextCtrl 2") );
    m_pBottom->Thaw();

    m_pCenter->Freeze();
    m_pCenter->AddPage( m_pSplitter, wxT("wxTextCtrl 1") );
    m_pCenter->Thaw();

    m_Manager.AddPane(m_pLeftPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane1")).
        Left());
    m_Manager.AddPane(m_pRightPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane2")).
        Right());
    m_Manager.AddPane(m_pBottomPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane3")).Caption(wxT("Pane Caption")).
        Bottom());
    m_Manager.AddPane(m_pCenterPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane4")).Caption(wxT("Pane Caption")).
        Center());
}

void CEditorMainFrame::GetEditAnimationDialog()
{
    if (m_pWEditAnimation == NULL)
    {
        m_pWEditAnimation = new EditAnimationDialog(this, wxID_ANY, wxT("Edit Animation"));
    }
    if (m_bIsLanguageSwitched)
    {
        m_pWEditAnimation->LanguageSwitch();
    }
    m_pWEditAnimation->ShowModal();
    BEATS_SAFE_DELETE(m_pWEditAnimation);
}

void CEditorMainFrame::GetEditLanguageDialog()
{
    if (m_pWEditLanguage == NULL)
    {
        m_pWEditLanguage = new EditLanguageDialog(this, wxID_ANY, wxT("Edit Language"), wxPoint(500,200), wxSize(450,700));
    }
    if (m_bIsLanguageSwitched)
    {
        m_pWEditLanguage->LanguageSwitch();
    }
    m_pWEditLanguage->wxDialog::ShowModal(); 
}

void CEditorMainFrame::GetEditUIDialog()
{
    if (m_pWEditUI == NULL)
    {
        m_pWEditUI = new EditUIDialog(this, wxID_ANY, wxT("Edit UI"), wxDefaultPosition, wxSize(700,450));
    }
    if (m_bIsLanguageSwitched)
    {
        m_pWEditUI->LanguageSwitch();
    }
    m_pWEditUI->ShowModal(); 
}

void CEditorMainFrame::OnAuiButton(wxCommandEvent& event)
{
    int id = event.GetId();
    switch (id)
    {
    case Menu_File_Open:
        OpenProject();
        break;
    case Menu_File_Close:
        CloseProject();
        break;
    case Menu_File_Save:
        SaveProject();
        break;
    case Menu_File_Export:
        Export();
        break;
    case Menu_Edit_Animation:
        GetEditAnimationDialog();
        break;
    case Menu_Edit_UI:
        GetEditUIDialog();
        break;
    case Menu_Edit_Language:
        GetEditLanguageDialog();
        break;
    case Menu_Help_Language_Chinese:
        LanguageSwitch(Menu_Help_Language_Chinese);
        break;
    case Menu_Help_Language_English:
        LanguageSwitch(Menu_Help_Language_English);
        break;
    case eFLMS_AddFile:
        AddComponentFile();
        break;
    case eFLMS_AddFileFolder:
        AddComponentFileFolder();
        break;
    case eFLMS_Delete:
        DeleteComponentFile();
        break;
    case eFLMS_OpenFileDirectory:
        OpenComponentFileDirectory();
        break;
    default:
        break;
    }
    return;
}

void CEditorMainFrame::CreatSplitter()
{
    m_pSplitter = new wxSplitterWindow(m_pCenter);
    m_pSplTop = new CFCEditorGLWindow(m_pSplitter);
    static_cast<CEngineEditor*>(wxApp::GetInstance())->CreateGLContext(m_pSplTop);
    wxGLContext* pContext = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGLContext();

    m_pSplTop->SetGLContext(pContext);
    m_pComponentRenderWindow = new CFCEditorComponentWindow(m_pSplitter, pContext);
    m_pComponentRenderWindow->SetMinSize(wxSize(MINWINDOWSIZE, MINWINDOWSIZE));
    m_pSplitter->SplitHorizontally(m_pSplTop, m_pComponentRenderWindow, 500);
}

void CEditorMainFrame::AddTreeItem()
{
}

void CEditorMainFrame::AddChilditemToItem( wxTreeItemId& idParent, std::vector<TString>& vecName, size_t iLevel )
{
    if (vecName.size() > iLevel)
    {
        TString str = vecName[iLevel];
        wxTreeItemIdValue cookie;
        wxTreeItemId idChild = m_pComponentFileTC->GetFirstChild(idParent, cookie);
        wxTreeItemId idLastChild = m_pComponentFileTC->GetLastChild(idParent);
        if (!idChild.IsOk())
        {
            idChild = m_pComponentFileTC->AppendItem(idParent, str, iLevel);
        }
        else
        {
            while (idChild.IsOk() && str != m_pComponentFileTC->GetItemText(idChild))
            {
                if (idLastChild == idChild)
                {
                    idChild = m_pComponentFileTC->AppendItem(idParent, str, iLevel);
                    break;
                }
                idChild = m_pComponentFileTC->GetNextChild(idParent, cookie);
            }
        }
        if (idChild.IsOk())
        {
            AddChilditemToItem(idChild, vecName, ++iLevel);
        }
    }
}

void CEditorMainFrame::OnActivateComponentFile( wxTreeEvent& event )
{
    wxTreeItemId itemId = event.GetItem();
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTC->GetItemData(itemId);
    const TString& curWorkingFile = CComponentManager::GetInstance()->GetCurrentWorkingFilePath();
    if (pData->IsDirectory())
    {
        m_pComponentFileTC->Toggle(itemId);
    }
    else if (_tcsicmp(pData->GetFileName().c_str(), curWorkingFile.c_str()) != 0)
    {
        if (curWorkingFile.length() > 0)
        {
            CloseComponentFile();
        }
        SelectComponent(NULL);
        const TCHAR* pszFileName = pData->GetFileName().c_str();
        OpenComponentFile(pszFileName);
        m_pComponentRenderWindow->UpdateAllDependencyLine();
    }

}

void CEditorMainFrame::OnComponentFileStartDrag( wxTreeEvent& event )
{
    CComponentFileTreeItemData* pData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(event.GetItem()));
    if (pData != NULL && !pData->IsDirectory())
    {
        m_pComponentFileTC->SelectItem(event.GetItem());
        m_pComponentRenderWindow->SetDraggingFileName(pData->GetFileName().c_str());
        event.Allow();
    }
}

void CEditorMainFrame::OnComponentFileEndDrag( wxTreeEvent& event )
{
    wxTreeItemId currentItemId = event.GetItem();
    if (currentItemId.IsOk())
    {
        CComponentFileTreeItemData* pCurrentItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(currentItemId));
        wxTreeItemId lastItemId = m_pComponentFileTC->GetSelection();
        CComponentFileTreeItemData* pLastItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(lastItemId));
        BEATS_ASSERT(pCurrentItemData != NULL && pLastItemData != NULL && !pLastItemData->IsDirectory());
        if (lastItemId != currentItemId)
        {
            CComponentFileTreeItemData* pNewData = new CComponentFileTreeItemData(pLastItemData->GetProjectDirectory(), pLastItemData->GetFileName());
            wxString lastItemText = m_pComponentFileTC->GetItemText(lastItemId);
            if (pCurrentItemData->IsDirectory())
            {
                m_pComponentFileTC->AppendItem(currentItemId, lastItemText, EnginePropertyGirdManager::eTCIT_File, -1, pNewData);
            }
            else
            {
                wxTreeItemId parentId = m_pComponentFileTC->GetItemParent(currentItemId);
                m_pComponentFileTC->InsertItem(parentId, currentItemId, lastItemText, EnginePropertyGirdManager::eTCIT_File, -1, pNewData);
            }
            m_pComponentFileTC->Expand(currentItemId);
            wxTreeItemId lastItemDirectory = m_pComponentFileTC->GetItemParent(lastItemId);
            m_pComponentFileTC->Delete(lastItemId);
            if (lastItemDirectory.IsOk())
            {
                if (m_pComponentFileTC->HasChildren(lastItemDirectory) == false)
                {
                    m_pComponentFileTC->SetItemImage(lastItemDirectory, EnginePropertyGirdManager::eTCIT_Folder);
                }
            }
        }
    }
    if (m_pComponentRenderWindow->IsMouseInWindow())
    {
        const TCHAR* pDraggingFileName = m_pComponentRenderWindow->GetDraggingFileName();
        if (pDraggingFileName != NULL && PathFileExists(pDraggingFileName))
        {
            const TString& strCurWorkingFile = CComponentManager::GetInstance()->GetCurrentWorkingFilePath();
            if (!strCurWorkingFile.empty())
            {
                int iRet = wxMessageBox(wxString::Format(_T("是否要将组件从\n%s\n拷贝到\n%s？"), pDraggingFileName, strCurWorkingFile.c_str()), _T("自动添加组件"), wxYES_NO);
                if (iRet == wxYES)
                {
                    CComponentManager::GetInstance()->OpenFile(pDraggingFileName, true);
                    m_pComponentRenderWindow->UpdateAllDependencyLine();
                }
            }
            else
            {
                wxMessageBox(_T("请先打开一个文件!"));
            }
        }
        else
        {
            wxMessageBox(wxString::Format(_T("添加文件%s失败，可能文件不存在。"), pDraggingFileName));
        }
    }
    m_pComponentRenderWindow->SetDraggingFileName(NULL);
}

void CEditorMainFrame::OnComponentStartDrag( wxTreeEvent& event )
{
    CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTC->GetItemData(event.GetItem()));
    if (pData != NULL && !pData->IsDirectory())
    {
        m_pComponentRenderWindow->SetDraggingComponent(static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->GetComponentTemplate(pData->GetGUID())));
        event.Allow();
        BEATS_PRINT(_T("StartDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
    }
}

void CEditorMainFrame::OnComponentEndDrag( wxTreeEvent& event )
{
    if (m_pComponentRenderWindow->IsMouseInWindow())
    {
        CComponentEditorProxy* pDraggingComponent = m_pComponentRenderWindow->GetDraggingComponent();
        CComponentEditorProxy* pNewComponent = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->CreateComponent(pDraggingComponent->GetGuid(), true));
        int x = 0;
        int y = 0;
        pDraggingComponent->GetGraphics()->GetPosition(&x, &y);
        pNewComponent->GetGraphics()->SetPosition(x, y);
        SelectComponent(pNewComponent);
    }
    m_pComponentRenderWindow->SetDraggingComponent(NULL);
    BEATS_PRINT(_T("EndDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
}

void CEditorMainFrame::OnIdle( wxIdleEvent& /*event*/ )
{
    if (m_pPropGridManager->IsNeedUpdatePropertyGrid())
    {
        m_pPropGridManager->SetUpdateFlag(false);
        UpdatePropertyGrid();
    }
}

void CEditorMainFrame::UpdatePropertyGrid()
{
    m_pPropGridManager->ClearPage(0);
    if (GetSelectedComponent() != NULL)
    {
        m_pPropGridManager->InsertComponentsInPropertyGrid(GetSelectedComponent());
    }
}

void CEditorMainFrame::SelectComponent(CComponentEditorProxy* pComponentInstance)
{
    m_pPropGridManager->ClearPage(0);
    m_pSelectedComponent = pComponentInstance;
    if (pComponentInstance != NULL)
    {
        // If we select an instance(not a template).
        if (pComponentInstance->GetId() != -1)
        {
            m_pComponentFileTC->Unselect();
        }
        m_pPropGridManager->InsertComponentsInPropertyGrid(pComponentInstance);
    }
}

CComponentEditorProxy* CEditorMainFrame::GetSelectedComponent()
{
    return m_pSelectedComponent;
}

void CEditorMainFrame::OnSearch( wxCommandEvent& /*event*/ )
{
    //TODO : add search instance
}

void CEditorMainFrame::ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap)
{
    if (conflictIdMap.size() > 0)
    {
        TCHAR szConflictInfo[10240];
        _stprintf(szConflictInfo, _T("工程中有%d个ID发生冲突，点击\"是\"开始解决,点击\"否\"退出."), conflictIdMap.size());
        int iRet = MessageBox(NULL, szConflictInfo, _T("解决ID冲突"), MB_YESNO);
        if (iRet == IDYES)
        {
            CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
            size_t lAnswer = 0;
            std::map<size_t, std::vector<size_t>>::const_iterator iter = conflictIdMap.begin();
            for (; iter != conflictIdMap.end(); ++iter)
            {
                _stprintf(szConflictInfo, _T("Id为%d的组件出现于文件:\n"), iter->first);
                for (size_t i = 0; i < iter->second.size(); ++i)
                {
                    TCHAR szFileName[1024];
                    _stprintf(szFileName, _T("%d.%s\n"), i, pProject->GetComponentFileName(iter->second[i]).c_str());
                    _tcscat(szConflictInfo, szFileName);
                }
                _tcscat(szConflictInfo, _T("需要保留ID的文件序号是(填-1表示全部分配新ID):"));
                lAnswer = wxGetNumberFromUser(szConflictInfo, _T("promote"), _T("请填入序号"), lAnswer, -1, iter->second.size() - 1);
                for (size_t i = 0; i < iter->second.size(); ++i)
                {
                    pProject->ResolveIdForFile(iter->second[i], iter->first, i == lAnswer);
                }
            }
        }
        else
        {
            _exit(0);
        }
    }
}

void CEditorMainFrame::OpenProjectFile( const TCHAR* pPath )
{
    if (pPath != NULL && _tcslen(pPath) > 0)
    {
        CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
        std::map<size_t, std::vector<size_t>> conflictIdMap;
        CComponentProjectDirectory* pProjectData = pProject->LoadProject(pPath, conflictIdMap);
        bool bEmptyProject = pProjectData == NULL;
        if (!bEmptyProject)
        {
            InitializeComponentFileTree(pProjectData, m_pComponentFileTC->GetRootItem());
            ResolveIdConflict(conflictIdMap);
        }

        m_pComponentFileTC->ExpandAll();
    }
}

void CEditorMainFrame::CloseProjectFile()
{
    SelectComponent(NULL);
    if (CComponentManager::GetInstance()->GetCurrentWorkingFilePath().length() > 0)
    {
        CloseComponentFile();
    }
    CComponentManager::GetInstance()->GetProject()->CloseProject();
    wxTreeItemId rootItem = m_pComponentFileTC->GetRootItem();
    m_pComponentFileTC->DeleteChildren(rootItem);
    CComponentFileTreeItemData* pRootItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(rootItem));
    pRootItemData->SetData(NULL);
    m_componentFileListMap.clear(); 
}

void CEditorMainFrame::Export()
{
    SaveProject();
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导出的文件"), BINARIZE_FILE_EXTENSION_FILTER, NULL);
    if (szBinaryPath.length() > 0)
    {
        std::vector<TString> filesToExport;
        for (std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
        {
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTC->GetItemData(iter->second);
            BEATS_ASSERT(pData != NULL && !pData->IsDirectory());
            TString filePath(pData->GetFileName());
            filesToExport.push_back(filePath);
        }
        const TCHAR* pExtensionStr = PathFindExtension(szBinaryPath.c_str());
        if (_tcsicmp(pExtensionStr, BINARIZE_FILE_EXTENSION) != 0)
        {
            szBinaryPath.append(BINARIZE_FILE_EXTENSION);
        }
        CComponentManager::GetInstance()->Export(filesToExport, szBinaryPath.c_str());
        //Export function will cause these operation: open->export->close->change file->open->...->restore open the origin file.
        //So we will update the dependency line as if we have just open a new file.
        m_pComponentRenderWindow->UpdateAllDependencyLine();
        wxMessageBox(wxT("导出完毕!"));
    }
}

void CEditorMainFrame::SaveProject()
{
    CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
    if (pProject->GetRootDirectory() != NULL)
    {
        TString szSavePath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
        szSavePath.append(_T("\\")).append(EXPORT_STRUCTURE_DATA_PATCH_XMLFILENAME);
        CComponentManager::GetInstance()->SaveTemplate(szSavePath.c_str());
        // Save Instance File
        CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
        TString strProjectFullPath = pProject->GetProjectFilePath();
        if (strProjectFullPath.length() == 0)
        {
            CUtilityManager::GetInstance()->AcquireSingleFilePath(true, NULL, strProjectFullPath, _T("保存文件"), COMPONENT_PROJECT_EXTENSION, NULL);
        }
        else
        {
            strProjectFullPath.append(pProject->GetProjectFileName());
        }
        if (strProjectFullPath.length() > 0)
        {
            FILE* pProjectFile = _tfopen(strProjectFullPath.c_str(), _T("w+"));
            BEATS_ASSERT(pProjectFile != NULL)
                if (pProjectFile)
                {
                    fclose(pProjectFile);
                    CComponentManager::GetInstance()->GetProject()->SaveProject();
                    const TString curWorkingFile = CComponentManager::GetInstance()->GetCurrentWorkingFilePath();
                    if (curWorkingFile.length() > 0)
                    {
                        CComponentManager::GetInstance()->SaveToFile(curWorkingFile.c_str());
                    }
                }
                UpdatePropertyGrid();
        }
    }
}

void CEditorMainFrame::OpenComponentFile( const TCHAR* pFilePath )
{
    BEATS_ASSERT(pFilePath != NULL && pFilePath[0] != 0, _T("Invalid file path"));
    CComponentManager::GetInstance()->OpenFile(pFilePath);
    size_t uPageCount = m_pLeft->GetPageCount();
    int iComponentListPageIndex = -1;
    for (size_t i = 0; i < uPageCount; ++i)
    {
        if (m_pLeft->GetPage(i) == m_pComponentTC)
        {
            iComponentListPageIndex = i;
            break;
        }
    }
    if (iComponentListPageIndex == -1)
    {
        m_pLeft->AddPage(m_pComponentTC, _T("组件选择"));
    }
}

void CEditorMainFrame::CloseComponentFile(bool bRemindSave /*= true*/)
{
    CComponentManager* pComponentManager = CComponentManager::GetInstance();
    const TString& strCurWorkingFile = pComponentManager->GetCurrentWorkingFilePath();
    if (strCurWorkingFile.length() > 0)
    {
        if (bRemindSave)
        {
            int iRet = wxMessageBox(wxString::Format(_T("是否要保存当前文件?\n%s"), strCurWorkingFile.c_str()), _T("保存文件"), wxYES_NO);
            if (iRet == wxYES)
            {
                pComponentManager->SaveToFile(strCurWorkingFile.c_str());
            }
        }
        pComponentManager->CloseFile();
    }
    size_t uPageCount = m_pLeft->GetPageCount();
    int iComponentListPageIndex = -1;
    for (size_t i = 0; i < uPageCount; ++i)
    {
        if (m_pLeft->GetPage(i) == m_pComponentTC)
        {
            iComponentListPageIndex = i;
            break;
        }
    }
    if (iComponentListPageIndex >= 0)
    {
        m_pLeft->RemovePage(iComponentListPageIndex);
    }
}

void CEditorMainFrame::OpenProject()
{
    CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
    TString strProjectFullPath = pProject->GetProjectFilePath();

    if (strProjectFullPath.length() == 0)
    {
        TString strWorkingPath = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetWorkingPath();
        CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, strProjectFullPath, _T("选择要读取的文件"), COMPONENT_PROJECT_EXTENSION, strWorkingPath.c_str());
        if (strProjectFullPath.length() > 0)
        {
            OpenProjectFile(strProjectFullPath.c_str());
        }
    }
    else
    {
        CloseProjectFile();
    }
}

void CEditorMainFrame::CloseProject()
{
    CloseProjectFile();
}

void CEditorMainFrame::ActivateFile(const TCHAR* pszFileName)
{
    std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(pszFileName);
    if (iter != m_componentFileListMap.end())
    {
        m_pComponentFileTC->SelectItem(iter->second);
        wxTreeEvent simulateEvent;
        simulateEvent.SetItem(iter->second);
        OnActivateComponentFile(simulateEvent);
    }
}

void CEditorMainFrame::OnRightClickComponentFileList( wxTreeEvent& event )
{
    m_pComponentFileTC->SelectItem(event.GetItem());
    if (CComponentManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
    {
        CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)(m_pComponentFileTC->GetItemData(event.GetItem()));
        BEATS_ASSERT(pData);
        m_pComponentFileMenu->Enable(eFLMS_AddFile, pData->IsDirectory());
        m_pComponentFileMenu->Enable(eFLMS_AddFileFolder, pData->IsDirectory());
        m_pComponentFileMenu->Enable(eFLMS_OpenFileDirectory, !pData->IsDirectory());

        PopupMenu(m_pComponentFileMenu);
    }
    event.Skip(false);
}

void CEditorMainFrame::AddComponentFile()
{
    wxTreeItemId item = m_pComponentFileTC->GetSelection();
    if (item.IsOk())
    {
        TString result;
        CUtilityManager::GetInstance()->AcquireSingleFilePath(true, NULL, result, _T("添加文件"), COMPONENT_FILE_EXTENSION_FILTER, NULL);
        if (result.length() > 0)
        {
            std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(result);
            if (iter != m_componentFileListMap.end())
            {
                int iRet = wxMessageBox(_T("该文件已经存在于项目之中！不能重复添加！是否跳转到该项目？"), _T("文件已存在"), wxYES_NO);
                if (iRet == wxYES)
                {
                    m_pComponentFileTC->SelectItem(iter->second);
                }
            }
            else
            {
                if (!PathFileExists(result.c_str()))
                {
                    const TCHAR* pExtensionStr = PathFindExtension(result.c_str());
                    if (_tcsicmp(pExtensionStr, COMPONENT_FILE_EXTENSION) != 0)
                    {
                        result.append(COMPONENT_FILE_EXTENSION);
                    }

                    TiXmlDocument document;
                    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
                    document.LinkEndChild(pDeclaration);
                    TiXmlElement* pRootElement = new TiXmlElement("Root");
                    document.LinkEndChild(pRootElement);
                    // TCHAR to char trick.
                    wxString pathTCHAR(result.c_str());
                    document.SaveFile(pathTCHAR);
                }
                CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, result);
                wxString fileName = wxFileNameFromPath(result.c_str());
                wxTreeItemId newItemId = m_pComponentFileTC->AppendItem(item, fileName, EnginePropertyGirdManager::eTCIT_File, -1, pData);
                m_pComponentFileTC->Expand(item);
                m_componentFileListMap[result] = newItemId;

                // When Add a new file in project, we must validate the id in this file. if id is conflicted, we have to resolve it.
                CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(item));
                BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
                std::map<size_t, std::vector<size_t>> conflictMap;
                pCurItemData->GetProjectDirectory()->AddFile(result, conflictMap);
                ResolveIdConflict(conflictMap);
                ActivateFile(result.c_str());
            }
        }
    }
}

void CEditorMainFrame::AddComponentFileFolder()
{
    wxTreeItemId item = m_pComponentFileTC->GetSelection();
    TString strNewName = wxGetTextFromUser(_T("文件夹名"), _T("添加文件夹"), _T("New File Folder Name"));
    if (strNewName.length() > 0)
    {
        CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(item));
        BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
        CComponentProjectDirectory* pNewProjectData = pCurItemData->GetProjectDirectory()->AddDirectory(strNewName);
        if (pNewProjectData != NULL)
        {
            CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pNewProjectData, strNewName);
            m_pComponentFileTC->AppendItem(item, strNewName, EnginePropertyGirdManager::eTCIT_Folder, -1, pData);
            m_pComponentFileTC->Expand(item);
        }
        else
        {
            MessageBox(NULL, _T("The name has already exist!"), _T("Error"), MB_OK);
        }
    }
}

void CEditorMainFrame::OpenComponentFileDirectory()
{
    wxTreeItemId item = m_pComponentFileTC->GetSelection();
    int iRet = wxMessageBox(_T("是否要删除物理文件？"), _T("删除文件"), wxYES_NO);
    DeleteItemInComponentFileList(item, iRet == wxYES);
    SelectComponent(NULL);
    wxTreeItemId parentItem = m_pComponentFileTC->GetItemParent(item);
    CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(item));
    CComponentFileTreeItemData* pParentItemData = NULL;
    CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
    if (parentItem.IsOk())
    {
        pParentItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(parentItem));
    }
    bool bIsRootDirectory = pParentItemData == NULL;
    if (!bIsRootDirectory)
    {
        BEATS_ASSERT(item != m_pComponentFileTC->GetRootItem(), _T("Only root doesn't have a parent!"));
        if (pCurItemData->IsDirectory())
        {
            bool bRet = pParentItemData->GetProjectDirectory()->DeleteDirectory(pCurItemData->GetProjectDirectory());
            BEATS_ASSERT(bRet, _T("Delete directory %s failed!"), pCurItemData->GetFileName());
        }
        else
        {
            size_t uFileId = pProject->GetComponentFileId(pCurItemData->GetFileName());
            BEATS_ASSERT(uFileId != 0xFFFFFFFF, _T("Can't find file %s at project"), pCurItemData->GetProjectDirectory()->GetName().c_str());
            bool bRet = pParentItemData->GetProjectDirectory()->DeleteFile(uFileId);
            BEATS_ASSERT(bRet, _T("Delete file %s failed!"), pCurItemData->GetFileName().c_str());
        }
        m_pComponentFileTC->Delete(item);
    }
    else
    {
        BEATS_ASSERT(item == m_pComponentFileTC->GetRootItem(), _T("Only root doesn't have a parent!"));
        CComponentManager::GetInstance()->CloseFile();
        pProject->GetRootDirectory()->DeleteAll(true);
        m_pComponentFileTC->DeleteChildren(item);// Never delete the root item.
    }
}

void CEditorMainFrame::DeleteComponentFile()
{
    wxTreeItemId item = m_pComponentFileTC->GetSelection();
    TString path = CComponentManager::GetInstance()->GetProject()->GetProjectFilePath();
    CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTC->GetItemData(item));
    if (m_pComponentFileTC->GetRootItem() != item)
    {
        BEATS_ASSERT(!pCurItemData->IsDirectory(), _T("eFLMS_OpenFileDirectory command can only use on file!"));
        if (!pCurItemData->IsDirectory())
        {
            path = pCurItemData->GetFileName();
            size_t pos = path.rfind(_T('\\'));
            path.resize(pos);
        }
    }

    TCHAR szCommand[MAX_PATH];
    GetWindowsDirectory(szCommand, MAX_PATH);
    _tcscat(szCommand, _T("\\explorer.exe "));
    _tcscat(szCommand, path.c_str());
    _tsystem(szCommand);
}

void CEditorMainFrame::DeleteItemInComponentFileList( wxTreeItemId itemId, bool bDeletePhysicalFile )
{
    BEATS_ASSERT(itemId.IsOk());
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTC->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL);
    if (pData->IsDirectory())
    {
        wxTreeItemIdValue idValue;
        wxTreeItemId childId = m_pComponentFileTC->GetFirstChild(itemId, idValue);
        while (childId.IsOk())
        {
            DeleteItemInComponentFileList(childId, bDeletePhysicalFile);
            childId = m_pComponentFileTC->GetNextChild(itemId, idValue);
        }
    }
    else
    {
        TString ItemNameStr(pData->GetFileName());
        std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(ItemNameStr);
        BEATS_ASSERT(iter != m_componentFileListMap.end());
        m_componentFileListMap.erase(iter);
        if (bDeletePhysicalFile)
        {
            ::DeleteFile(pData->GetFileName().c_str());
        }
    }
}

void CEditorMainFrame::OnTemplateComponentItemChanged( wxTreeEvent& event )
{
    wxTreeItemId id = m_pComponentTC->GetSelection();
    if (id.IsOk())
    {
        CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTC->GetItemData(id));
        if (pData != NULL && !pData->IsDirectory())
        {
            CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->GetComponentTemplate(pData->GetGUID()));
            SelectComponent(pComponent);
        }
    }
    event.Skip();
}

CComponentEditorProxy* CreateComponentProxy(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* pszClassName)
{
    return new CComponentEditorProxy(pGraphics, guid, parentGuid, pszClassName);
}

CComponentGraphic* CreateGraphic()
{
    return new CComponentGraphic_GL();
}

void CEditorMainFrame::InitComponentsPage()
{
    const TString& strWorkPath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Work);
    CComponentManager::GetInstance()->DeserializeTemplateData(strWorkPath.c_str(), CreateComponentProxy, CreateGraphic);
    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentManager::GetInstance()->GetComponentTemplateMap();
    for (std::map<size_t, CComponentBase*>::const_iterator componentIter = pComponentsMap->begin(); componentIter != pComponentsMap->end(); ++componentIter )
    {
        CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(componentIter->second);
        const TString& catalogName = pComponent->GetCatalogName();
        std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(catalogName);
        //Build catalog
        if (iter == m_componentCatalogNameMap.end())
        {
            std::vector<TString> result;
            CStringHelper::GetInstance()->SplitString(catalogName.c_str(), _T("\\"), result);
            BEATS_ASSERT(result.size() > 0);
            TString findStr;
            wxTreeItemId parentId = m_componentTreeIdMap[0];
            for (size_t i = 0; i < result.size(); ++i)
            {
                if (i > 0)
                {
                    findStr.append(_T("\\"));
                }
                findStr.append(result[i]);
                std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(findStr);

                if (iter == m_componentCatalogNameMap.end())
                {
                    CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(true, 0);
                    parentId = m_pComponentTC->AppendItem(parentId, result[i], -1, -1, pComponentItemData);
                    m_componentCatalogNameMap[findStr] = parentId;
                }
                else
                {
                    parentId = iter->second;
                }
            }
        }
        iter = m_componentCatalogNameMap.find(catalogName);
        BEATS_ASSERT(iter != m_componentCatalogNameMap.end());
        size_t guid = pComponent->GetGuid();
        CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(false, guid);
        m_componentTreeIdMap[guid] = m_pComponentTC->AppendItem(iter->second, pComponent->GetDisplayName(), -1, -1, pComponentItemData);
    }
    m_pComponentTC->ExpandAll();
}

void CEditorMainFrame::InitializeComponentFileTree( CComponentProjectDirectory* pProjectData, const wxTreeItemId& id )
{
    CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pProjectData, pProjectData->GetName());
    m_pComponentFileTC->SetItemData(id, pData);
    const std::vector<CComponentProjectDirectory*>& children = pProjectData->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        const TString& nameStr = (*iter)->GetName();
        wxTreeItemId newDirectoryId = m_pComponentFileTC->AppendItem(id, nameStr, eTCIT_Folder, -1, NULL);
        InitializeComponentFileTree(*iter, newDirectoryId);
    }

    const std::set<size_t>& files = pProjectData->GetFileList();
    for (std::set<size_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TString strComopnentFileName = CComponentManager::GetInstance()->GetProject()->GetComponentFileName(*iter);
        CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, strComopnentFileName);
        wxString pFileName = wxFileNameFromPath(strComopnentFileName);
        wxTreeItemId newFileId = m_pComponentFileTC->AppendItem(id, pFileName, eTCIT_File, -1, pData);
        BEATS_ASSERT(m_componentFileListMap.find(strComopnentFileName) == m_componentFileListMap.end(), _T("Same File in a project! %s"), strComopnentFileName.c_str());
        m_componentFileListMap[strComopnentFileName] = newFileId;
    }
}

void CEditorMainFrame::OnSplitterSashDClick( wxSplitterEvent& /*event*/ )
{
    int psn = 0;
    double gravity = 0.0;
    gravity = m_pSplitter->GetSashGravity();
    psn = m_pSplitter->GetSashPosition();
}

void CEditorMainFrame::InitFrame()
{
    m_Manager.SetManagedWindow(this);

    InitMenu();
    InitCtrls();

    SetStatusBar(new wxStatusBar(this));
    SetStatusText(wxT("Welcome!"));
    SetMinSize(wxSize(MINWINDOWSIZE,MINWINDOWSIZE));

    Centre();
}

void CEditorMainFrame::LanguageSwitch(int id)
{
    m_bIsLanguageSwitched = true;
    switch (id)
    {
    case Menu_Help_Language_Chinese:
        CLanguageManager::GetInstance()->LoadFromFile(eLT_Chinese);
        ChangeLanguage();
        break;
    case Menu_Help_Language_English:
        CLanguageManager::GetInstance()->LoadFromFile(eLT_English);
        ChangeLanguage();
        break;
    default:
        break;
    }
}

void CEditorMainFrame::ChangeLanguage()
{
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance();

    m_pMenuBar->SetMenuLabel(eMB_File, pLanguageManager->GetText(eL_File));
    m_pMenuBar->SetMenuLabel(eMB_Edit, pLanguageManager->GetText(eL_Edit));
    m_pMenuBar->SetMenuLabel(eMB_Assets, pLanguageManager->GetText(eL_Assets));
    m_pMenuBar->SetMenuLabel(eMB_GameObj, pLanguageManager->GetText(eL_GameObject));
    m_pMenuBar->SetMenuLabel(eMB_Conponent, pLanguageManager->GetText(eL_Conponent));
    m_pMenuBar->SetMenuLabel(eMB_Window, pLanguageManager->GetText(eL_Window));
    m_pMenuBar->SetMenuLabel(eMB_Help, pLanguageManager->GetText(eL_Help));

    m_pFileMenu->SetLabel(Menu_File_Open, pLanguageManager->GetText(eL_Open));
    m_pFileMenu->SetLabel(Menu_File_Close, pLanguageManager->GetText(eL_Close));
    m_pFileMenu->SetLabel(Menu_File_Save, pLanguageManager->GetText(eL_Save));
    m_pFileMenu->SetLabel(Menu_File_Export, pLanguageManager->GetText(eL_Export));

    m_pEditMenu->SetLabel(Menu_Edit_Animation, pLanguageManager->GetText(eL_Edit_Animation));
    m_pEditMenu->SetLabel(Menu_Edit_UI, pLanguageManager->GetText(eL_Edit_UI));
    m_pEditMenu->SetLabel(Menu_Edit_Language, pLanguageManager->GetText(eL_Edit_Language));

    m_pHelpMenu->SetLabel(Menu_Help_Language, pLanguageManager->GetText(eL_Language));
    m_pHelpMenu->SetLabel(Menu_Help_Language_Chinese, pLanguageManager->GetText(eL_Chinese));
    m_pHelpMenu->SetLabel(Menu_Help_Language_English, pLanguageManager->GetText(eL_English));

    m_pLeft->SetPageText(0, pLanguageManager->GetText(eL_ComponetModel));
    m_pCenter->SetPageText(0, pLanguageManager->GetText(eL_View));
    m_pRight->SetPageText(0, pLanguageManager->GetText(eL_Inspector));
    m_pBottom->SetPageText(0, pLanguageManager->GetText(eL_View));
    m_pBottom->SetPageText(1, pLanguageManager->GetText(eL_View));
}

