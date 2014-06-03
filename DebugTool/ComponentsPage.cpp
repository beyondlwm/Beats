#include "stdafx.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include <wx/propgrid/manager.h>
#include <wx/artprov.h>
#include <wx/numdlg.h>
#include <d3dx9.h>
#include <boost/filesystem.hpp>
#include "../Components/Component/ComponentProxyManager.h"
#include "../Components/Component/ComponentProject.h"
#include "../Components/Component/ComponentProjectData.h"
#include "../Components/ComponentPublic.h"
#include "../Components/Component/ComponentEditorProxy.h"
#include "../Components/Component/ComponentGraphic.h"
#include "../Utility/Serializer/Serializer.h"
#include "../Utility/TinyXML/tinyxml.h"
#include "../Utility/StringHelper/StringHelper.h"
#include "../Utility/UtilityManager.h"
#include "ComponentSystem/ComponentRenderWindow.h"
#include "ComponentSystem/wxWidgetsProperty/wxWidgetsPropertyBase.h"
#include "ComponentSystem/wxWidgetsProperty/PtrPropertyDescription.h"
#include "ComponentSystem/wxWidgetsProperty/ListPropertyDescriptionEx.h"
#include "ComponentSystem/PtrEditor/wxPtrButtonEditor.h"
#include "ComponentSystem/ComponentGraphics_DX.h"

enum EPropertyMenuSelection
{
    ePMS_ResetValue,
    ePMS_Expand,
    ePMS_Collapsed,

    ePMS_Count,
    ePMS_Force32bit = 0xffffffff
};

enum EFileListMenuSelection
{
    eFLMS_AddFile,
    eFLMS_AddFileFolder,
    eFLMS_Delete,
    eFLMS_OpenFileDirectory,

    eFLMS_Count,
    eFLMS_Force32bit = 0xffffffff
};

class CComponentTreeItemData : public wxTreeItemData
{
public:
    CComponentTreeItemData(bool bIsDirectory, size_t guid)
        : m_bIsDirectory(bIsDirectory)
        , m_componentGUID(guid)
    {
    }
    virtual ~CComponentTreeItemData()
    {
    }
    bool IsDirectory()
    {
        return m_bIsDirectory;
    }
    size_t GetGUID()
    {
        return m_componentGUID;
    }
private:
    bool m_bIsDirectory;
    size_t m_componentGUID;
};

class CComponentFileTreeItemData : public wxTreeItemData
{
public:
    CComponentFileTreeItemData(CComponentProjectDirectory* pData, const TString& fileName)
        : m_pData(pData)
        , m_fileName(fileName)
    {
    }
    virtual ~CComponentFileTreeItemData()
    {
    }
    bool IsDirectory()
    {
        return m_pData != NULL;
    }
    const TString& GetFileName() const
    {
        return m_fileName;
    }
    CComponentProjectDirectory* GetProjectDirectory() const
    {
        return m_pData;
    }
    void SetData(CComponentProjectDirectory* pData)
    {
        m_pData = pData;
    }
private:
    TString m_fileName;
    CComponentProjectDirectory* m_pData;
};

void CBDTWxFrame::CreateComponentPage()
{
    m_pComponentPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* pComponentSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pComponentSpliterWindow = new wxSplitterWindow(m_pComponentPanel);
    pComponentSizer->Add(m_pComponentSpliterWindow, 1, wxEXPAND, 5);

    wxPanel* pLeftPanel = new wxPanel(m_pComponentSpliterWindow);
    wxBoxSizer* pLeftSizer = new wxBoxSizer(wxVERTICAL);

    m_pComponentPageNoteBook = new wxNotebook( pLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

    m_pComponentFileSelectePanel = new wxPanel( m_pComponentPageNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* pComponentFileListSizer = new wxBoxSizer(wxVERTICAL);
    m_pComponentFileTreeControl = new wxTreeCtrl( m_pComponentFileSelectePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
    m_pComponentFileTreeControl->SetMinSize(wxSize(150, -1));
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
    m_pComponentFileTreeControl->AssignImageList(pFileIconImages);

    pComponentFileListSizer->Add( m_pComponentFileTreeControl, 1, wxEXPAND, 5 );
    m_pComponentFileSearchCtrl = new wxSearchCtrl(m_pComponentFileSelectePanel, wxID_ANY); 
    m_pComponentFileSearchCtrl->ShowSearchButton(true);
    m_pComponentFileSearchCtrl->ShowCancelButton(true);
    pComponentFileListSizer->Add( m_pComponentFileSearchCtrl, 0, wxEXPAND, 5 );
    m_pComponentFileSelectePanel->SetSizerAndFit(pComponentFileListSizer);
    m_pComponentPageNoteBook->AddPage(m_pComponentFileSelectePanel, _T("文件选择"));

    m_pComponentListPanel = new wxPanel( m_pComponentPageNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* pComponentListSizer = new wxBoxSizer( wxVERTICAL);
    m_pComponentTreeControl = new wxTreeCtrl( m_pComponentListPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE );
    m_pComponentTreeControl->SetMinSize(wxSize(150, -1));
    pComponentListSizer->Add( m_pComponentTreeControl, 1, wxEXPAND, 5 );
    m_pComponentSearchCtrl = new wxSearchCtrl(m_pComponentListPanel, wxID_ANY);
    m_pComponentSearchCtrl->ShowSearchButton(true);
    m_pComponentSearchCtrl->ShowCancelButton(true);
    pComponentListSizer->Add( m_pComponentSearchCtrl, 0, wxEXPAND, 5 );
    m_pComponentListPanel->SetSizerAndFit(pComponentListSizer);
    m_pComponentListPanel->Hide();

    pLeftSizer->Add( m_pComponentPageNoteBook, 1, wxEXPAND, 5 );
    pLeftPanel->SetSizer(pLeftSizer);
    pLeftPanel->Layout();
    pLeftSizer->Fit(pLeftPanel);

    wxPanel* pRightPanel = new wxPanel(m_pComponentSpliterWindow);
    wxBoxSizer* pRightSizer = new wxBoxSizer(wxHORIZONTAL);

    m_pComponentRenderWindowSizer = new wxStaticBoxSizer( new wxStaticBox( pRightPanel, wxID_ANY, wxT("渲染区") ), wxVERTICAL );

    m_pComponentRenderWindow = new CComponentRenderWindow(pRightPanel, wxID_ANY);
    m_pComponentRenderWindowSizer->Add( m_pComponentRenderWindow, 1, wxEXPAND, 35 );
    
    wxSize renderWindowSize = m_pComponentRenderWindow->GetSize();

    pRightSizer->Add(m_pComponentRenderWindowSizer, 1, wxEXPAND, 5);

    wxStaticBoxSizer* pFunctionSizer = new wxStaticBoxSizer( new wxStaticBox( pRightPanel, wxID_ANY, wxT("功能区") ), wxVERTICAL );
    wxStaticBoxSizer* pComponentPropertySizer = new wxStaticBoxSizer( new wxStaticBox( pRightPanel, wxID_ANY, wxT("属性选项") ), wxVERTICAL );

    m_pPtrButtonEditor = wxPropertyGrid::RegisterEditorClass(new wxPtrButtonEditor());

    wxPropertyGridManager* pProertyGridManager = CreatePropertyGrid(pRightPanel);
    pComponentPropertySizer->Add(pProertyGridManager, 1, wxEXPAND);
    pFunctionSizer->Add(pComponentPropertySizer, 1, wxEXPAND, 5);

    wxBoxSizer* p1stRowSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pOperateFileButton = new wxButton(pRightPanel, wxID_ANY, wxT("打开工程"));
    p1stRowSizer->Add(m_pOperateFileButton, 0, wxEXPAND, 5);
    m_pComponentSaveButton = new wxButton(pRightPanel, wxID_ANY, wxT("保存工程"));
    m_pComponentSaveButton->Enable(false);
    p1stRowSizer->Add(m_pComponentSaveButton, 0, wxEXPAND, 5);
    m_pSerializeButton = new wxButton(pRightPanel, wxID_ANY, wxT("导出数据"));
    m_pSerializeButton->Enable(false);
    p1stRowSizer->Add(m_pSerializeButton, 0, wxEXPAND, 5);
    pFunctionSizer->Add(p1stRowSizer, 0, wxEXPAND, 5);
    pRightSizer->Add( pFunctionSizer, 0, wxEXPAND, 5 );

    pRightPanel->SetSizer(pRightSizer);
    pRightPanel->Layout();
    pRightSizer->Fit(pRightPanel);

    m_pComponentSpliterWindow->SplitVertically(pLeftPanel, pRightPanel, 150);
    m_pComponentSpliterWindow->SetMinimumPaneSize(100);
    long style = m_pComponentSpliterWindow->GetWindowStyleFlag();
    if ((style & wxSP_BORDER) > 0)
    {
        style &= ~wxSP_BORDER;
        m_pComponentSpliterWindow->SetWindowStyleFlag(style);
    }

    m_pComponentPanel->SetSizer( pComponentSizer );
    m_pComponentPanel->Layout();
    pComponentSizer->Fit( m_pComponentPanel );
    m_pNoteBook->AddPage( m_pComponentPanel, wxT("组件系统"), true );

    TString rootName = _T("文件列表");
    m_pComponentFileTreeControl->AddRoot(rootName, eTCIT_Folder, -1, NULL);
    wxTreeItemId rootId = m_pComponentTreeControl->AddRoot(wxT("组件列表"));
    m_componentCatalogNameMap[_T("Root")] = rootId;
    m_componentTreeIdMap[0] = rootId;
    InitComponentsPage();

    m_pPropertyMenu = new wxMenu;
    m_pPropertyMenu->Append(ePMS_ResetValue, _T("撤销修改"));
    m_pPropertyMenu->Append(ePMS_Expand, _T("展开"));
    m_pPropertyMenu->Append(ePMS_Collapsed, _T("收拢"));

    m_pComponentFileMenu = new wxMenu;
    m_pComponentFileMenu->Append(eFLMS_AddFile, _T("添加文件"));
    m_pComponentFileMenu->Append(eFLMS_AddFileFolder, _T("添加文件夹"));
    m_pComponentFileMenu->Append(eFLMS_Delete, _T("删除文件"));
    m_pComponentFileMenu->Append(eFLMS_OpenFileDirectory, _T("打开所在的文件夹"));

    m_pComponentTreeControl->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentStartDrag ), NULL, this );
    m_pComponentTreeControl->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentEndDrag ), NULL, this );
    m_pComponentTreeControl->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CBDTWxFrame::OnTemplateComponentItemChanged ), NULL, this );
    m_pComponentFileTreeControl->Connect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentFileStartDrag ), NULL, this );
    m_pComponentFileTreeControl->Connect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentFileEndDrag ), NULL, this );
    m_pComponentFileTreeControl->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(CBDTWxFrame::OnRightClickComponentFileList), NULL, this);
    m_pComponentFileTreeControl->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CBDTWxFrame::OnActivateComponentFile), NULL, this);
    m_pComponentFileTreeControl->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(CBDTWxFrame::OnExpandComponentFile), NULL, this);
    m_pComponentFileTreeControl->Connect(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(CBDTWxFrame::OnCollapseComponentFile), NULL, this);
    m_pComponentFileSearchCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CBDTWxFrame::OnComponentFileSearchTextUpdate ), NULL, this );
    m_pComponentFileSearchCtrl->Connect( wxEVT_IDLE, wxCommandEventHandler( CBDTWxFrame::OnComponentFileSearchIdle ), NULL, this );
    m_pComponentSearchCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CBDTWxFrame::OnComponentSearchTextUpdate ), NULL, this );
    m_pComponentSearchCtrl->Connect( wxEVT_IDLE, wxCommandEventHandler( CBDTWxFrame::OnComponentSearchIdle ), NULL, this );

    m_pPropertyGridManager->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertyChanged), NULL, this);
    m_pPropertyGridManager->Connect(wxEVT_PG_SELECTED, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertySelected), NULL, this);
    m_pPropertyGridManager->Connect(wxEVT_PG_RIGHT_CLICK, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertyRightClick), NULL, this);
    m_pPropertyGridManager->Connect(wxEVT_IDLE, wxIdleEventHandler( CBDTWxFrame::OnComponentPropertyGridIdle ), NULL, this);
    m_pPropertyMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBDTWxFrame::OnComponentPropertyMenuClicked), NULL, this);
    m_pComponentFileMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBDTWxFrame::OnComponentFileListMenuClicked), NULL, this);
    m_pComponentSaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnComponentSaveButtonClick ), NULL, this );
    m_pOperateFileButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnOperateFileButtonClick ), NULL, this );
    m_pSerializeButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnExportButtonClick ), NULL, this );
}

void CBDTWxFrame::DestroyComponentPage()
{
    m_pComponentTreeControl->Disconnect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentStartDrag ), NULL, this );
    m_pComponentTreeControl->Disconnect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentEndDrag ), NULL, this );
    m_pComponentTreeControl->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CBDTWxFrame::OnTemplateComponentItemChanged ), NULL, this );
    m_pComponentFileTreeControl->Disconnect( wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentFileStartDrag ), NULL, this );
    m_pComponentFileTreeControl->Disconnect( wxEVT_COMMAND_TREE_END_DRAG, wxTreeEventHandler( CBDTWxFrame::OnComponentFileEndDrag ), NULL, this );
    m_pComponentFileTreeControl->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEventHandler(CBDTWxFrame::OnRightClickComponentFileList), NULL, this);
    m_pComponentFileTreeControl->Disconnect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CBDTWxFrame::OnActivateComponentFile), NULL, this);
    m_pComponentFileTreeControl->Disconnect(wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler(CBDTWxFrame::OnExpandComponentFile), NULL, this);
    m_pComponentFileTreeControl->Disconnect(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(CBDTWxFrame::OnCollapseComponentFile), NULL, this);
    m_pComponentFileSearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CBDTWxFrame::OnComponentFileSearchTextUpdate ), NULL, this );
    m_pComponentFileSearchCtrl->Disconnect( wxEVT_IDLE, wxCommandEventHandler( CBDTWxFrame::OnComponentFileSearchIdle ), NULL, this );
    m_pComponentSearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CBDTWxFrame::OnComponentSearchTextUpdate ), NULL, this );
    m_pComponentSearchCtrl->Disconnect( wxEVT_IDLE, wxCommandEventHandler( CBDTWxFrame::OnComponentSearchIdle ), NULL, this );

    m_pPropertyGridManager->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertyChanged), NULL, this);
    m_pPropertyGridManager->Disconnect(wxEVT_PG_SELECTED, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertySelected), NULL, this);
    m_pPropertyGridManager->Disconnect(wxEVT_PG_RIGHT_CLICK, wxPropertyGridEventHandler(CBDTWxFrame::OnComponentPropertyRightClick), NULL, this);
    m_pPropertyGridManager->Disconnect(wxEVT_IDLE, wxIdleEventHandler( CBDTWxFrame::OnComponentPropertyGridIdle ), NULL, this);
    m_pPropertyMenu->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBDTWxFrame::OnComponentPropertyMenuClicked), NULL, this);
    m_pComponentFileMenu->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CBDTWxFrame::OnComponentFileListMenuClicked), NULL, this);
    m_pComponentSaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnComponentSaveButtonClick ), NULL, this );
    m_pOperateFileButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnOperateFileButtonClick ), NULL, this );
    m_pSerializeButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnExportButtonClick ), NULL, this );

    //Because the menu was created without a parent window, so we need to delete it manually.
    BEATS_SAFE_DELETE(m_pPropertyMenu);
    BEATS_SAFE_DELETE(m_pComponentFileMenu);
}

wxPropertyGridManager* CBDTWxFrame::CreatePropertyGrid(wxWindow* pParent)
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropertyGridManager = new wxPropertyGridManager(pParent, wxID_ANY, wxDefaultPosition, wxSize(200, 100), style );
    wxPropertyGrid* pPropGrid = m_pPropertyGridManager->GetGrid();
    m_pPropertyGridManager->SetExtraStyle(extraStyle);

    m_pPropertyGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    pPropGrid->SetVerticalSpacing( 2 );
    wxPGCell cell;
    cell.SetText("Unspecified");
    cell.SetFgCol(*wxLIGHT_GREY);
    pPropGrid->SetUnspecifiedValueAppearance(cell);
    wxPropertyGridPage* myPage = new wxPropertyGridPage();
    m_pPropertyGridManager->AddPage(wxT("Property"), wxNullBitmap, myPage);

    //HACK: select -1 first, because there is a bug in wxwidgets, to init current page -1 is necessary.
    m_pPropertyGridManager->SelectPage(-1);
    m_pPropertyGridManager->SelectPage(myPage);
    return m_pPropertyGridManager;
}

CComponentEditorProxy* CreateComponentProxy(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* pszClassName)
{
    return new CComponentEditorProxy(pGraphics, guid, parentGuid, pszClassName);
}

CComponentGraphic* CreateComponentGraphics()
{
    CComponentRenderWindow* pRenderWindow = CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetComponentRenderWindow();
    CComponentGraphic_DX* pGraphics = new CComponentGraphic_DX(pRenderWindow->GetRenderDevice(), pRenderWindow->GetRenderFont());
    return pGraphics;
}

void CBDTWxFrame::InitComponentsPage()
{
    pComponentLauncherFunc();
    CComponentProxyManager::GetInstance()->DeserializeTemplateData(CBDTWxApp::GetBDTWxApp()->GetWorkingPath().c_str(), CreateComponentProxy, CreateComponentGraphics);
    const std::map<size_t, CComponentBase*>* pComponentsMap = CComponentProxyManager::GetInstance()->GetComponentTemplateMap();
    for (std::map<size_t, CComponentBase*>::const_iterator componentIter = pComponentsMap->begin(); componentIter != pComponentsMap->end(); ++componentIter )
    {
        CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(componentIter->second);
        const TString& catalogName = pComponent->GetCatalogName();
        std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(catalogName);
        //Build catalog
        if (iter == m_componentCatalogNameMap.end())
        {
            std::vector<TString> result;
            CStringHelper::GetInstance()->SplitString(catalogName.c_str(), _T("/"), result);
            BEATS_ASSERT(result.size() > 0);
            TString findStr;
            wxTreeItemId parentId = m_componentTreeIdMap[0];
            for (size_t i = 0; i < result.size(); ++i)
            {
                if (i > 0)
                {
                    findStr.append(_T("/"));
                }
                findStr.append(result[i]);
                std::map<TString, wxTreeItemId>::iterator iter = m_componentCatalogNameMap.find(findStr);

                if (iter == m_componentCatalogNameMap.end())
                {
                    CComponentTreeItemData* pComponentItemData = new CComponentTreeItemData(true, 0);
                    parentId = m_pComponentTreeControl->AppendItem(parentId, result[i], -1, -1, pComponentItemData);
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
        m_componentTreeIdMap[guid] = m_pComponentTreeControl->AppendItem(iter->second, pComponent->GetDisplayName(), -1, -1, pComponentItemData);
    }
    m_pComponentTreeControl->ExpandAll();
}

void CBDTWxFrame::OnComponentStartDrag( wxTreeEvent& event )
{
    CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTreeControl->GetItemData(event.GetItem()));
    if (pData != NULL && !pData->IsDirectory())
    {
        m_pComponentRenderWindow->SetDraggingComponent(static_cast<CComponentEditorProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(pData->GetGUID())));
        event.Allow();
        BEATS_PRINT(_T("StartDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
    }
}

void CBDTWxFrame::OnComponentEndDrag( wxTreeEvent& event )
{
    if (m_pComponentRenderWindow->IsMouseInWindow())
    {
        CComponentEditorProxy* pDraggingComponent = m_pComponentRenderWindow->GetDraggingComponent();
        CComponentEditorProxy* pNewComponent = static_cast<CComponentEditorProxy*>(CComponentProxyManager::GetInstance()->CreateComponent(pDraggingComponent->GetGuid(), true));
        int x = 0;
        int y = 0;
        pDraggingComponent->GetGraphics()->GetPosition(&x, &y);
        pNewComponent->GetGraphics()->SetPosition(x, y);
        SelecteComponent(pNewComponent);
    }
    m_pComponentRenderWindow->SetDraggingComponent(NULL);
    BEATS_PRINT(_T("EndDrag at Pos: %d %d\n"), event.GetPoint().x, event.GetPoint().y);
}

void CBDTWxFrame::OnComponentFileStartDrag( wxTreeEvent& event )
{
    CComponentFileTreeItemData* pData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(event.GetItem()));
    if (pData != NULL && !pData->IsDirectory())
    {
        m_pComponentFileTreeControl->SelectItem(event.GetItem());
        m_pComponentRenderWindow->SetDraggingFileName(pData->GetFileName().c_str());
        event.Allow();
    }
}

void CBDTWxFrame::OnComponentFileEndDrag( wxTreeEvent& event )
{
    wxTreeItemId currentItemId = event.GetItem();
    if (currentItemId.IsOk())
    {
        CComponentFileTreeItemData* pCurrentItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(currentItemId));
        wxTreeItemId lastItemId = m_pComponentFileTreeControl->GetSelection();
        CComponentFileTreeItemData* pLastItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(lastItemId));
        BEATS_ASSERT(pCurrentItemData != NULL && pLastItemData != NULL && !pLastItemData->IsDirectory());
        if (lastItemId != currentItemId)
        {
            CComponentFileTreeItemData* pNewData = new CComponentFileTreeItemData(pLastItemData->GetProjectDirectory(), pLastItemData->GetFileName());
            wxString lastItemText = m_pComponentFileTreeControl->GetItemText(lastItemId);
            if (pCurrentItemData->IsDirectory())
            {
                m_pComponentFileTreeControl->AppendItem(currentItemId, lastItemText, eTCIT_File, -1, pNewData);
            }
            else
            {
                wxTreeItemId parentId = m_pComponentFileTreeControl->GetItemParent(currentItemId);
                m_pComponentFileTreeControl->InsertItem(parentId, currentItemId, lastItemText, eTCIT_File, -1, pNewData);
            }
            m_pComponentFileTreeControl->Expand(currentItemId);
            wxTreeItemId lastItemDirectory = m_pComponentFileTreeControl->GetItemParent(lastItemId);
            m_pComponentFileTreeControl->Delete(lastItemId);
            if (lastItemDirectory.IsOk())
            {
                if (m_pComponentFileTreeControl->HasChildren(lastItemDirectory) == false)
                {
                    m_pComponentFileTreeControl->SetItemImage(lastItemDirectory, eTCIT_Folder);
                }
            }
        }
    }
    if (m_pComponentRenderWindow->IsMouseInWindow())
    {
        const TCHAR* pDraggingFileName = m_pComponentRenderWindow->GetDraggingFileName();
        if (pDraggingFileName != NULL && boost::filesystem::exists(pDraggingFileName))
        {
            const TString& strCurWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
            if (!strCurWorkingFile.empty())
            {
                int iRet = wxMessageBox(wxString::Format(_T("是否要将组件从\n%s\n拷贝到\n%s？"), pDraggingFileName, strCurWorkingFile.c_str()), _T("自动添加组件"), wxYES_NO);
                if (iRet == wxYES)
                {
                    CComponentProxyManager::GetInstance()->OpenFile(pDraggingFileName, true);
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

void CBDTWxFrame::OnTemplateComponentItemChanged(wxTreeEvent& event)
{
    wxTreeItemId id = m_pComponentTreeControl->GetSelection();
    if (id.IsOk())
    {
        CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pComponentTreeControl->GetItemData(id));
        if (pData != NULL && !pData->IsDirectory())
        {
            CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(CComponentProxyManager::GetInstance()->GetComponentTemplate(pData->GetGUID()));
            SelecteComponent(pComponent);
        }
    }
    event.Skip();
}

void CBDTWxFrame::OnRightClickComponentFileList( wxTreeEvent& event )
{
    m_pComponentFileTreeControl->SelectItem(event.GetItem());
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)(m_pComponentFileTreeControl->GetItemData(event.GetItem()));
    BEATS_ASSERT(pData);
    if (CComponentProxyManager::GetInstance()->GetProject()->GetRootDirectory() != NULL)
    {
        m_pComponentFileMenu->Enable(eFLMS_AddFile, pData->IsDirectory());
        m_pComponentFileMenu->Enable(eFLMS_AddFileFolder, pData->IsDirectory());
        m_pComponentFileMenu->Enable(eFLMS_OpenFileDirectory, !pData->IsDirectory());

        PopupMenu(m_pComponentFileMenu);
    }
    event.Skip(false);
}

void CBDTWxFrame::ActivateFile(const TCHAR* pszFileName)
{
    std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.find(pszFileName);
    if (iter != m_componentFileListMap.end())
    {
        m_pComponentFileTreeControl->SelectItem(iter->second);
        wxTreeEvent simulateEvent;
        simulateEvent.SetItem(iter->second);
        OnActivateComponentFile(simulateEvent);
    }
}

void CBDTWxFrame::OnActivateComponentFile(wxTreeEvent& event)
{
    wxTreeItemId itemId = event.GetItem();
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeControl->GetItemData(itemId);
    const TString& curWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
    if (pData->IsDirectory())
    {
        m_pComponentFileTreeControl->Toggle(itemId);
    }
    else if (_tcsicmp(pData->GetFileName().c_str(), curWorkingFile.c_str()) != 0)
    {
        if (curWorkingFile.length() > 0)
        {
            CloseComponentFile();
        }
        SelecteComponent(NULL);
        const TCHAR* pszFileName = pData->GetFileName().c_str();
        OpenComponentFile(pszFileName);
        m_pComponentRenderWindow->UpdateAllDependencyLine();
    }
}

void CBDTWxFrame::OnExpandComponentFile(wxTreeEvent& event)
{
    m_pComponentFileTreeControl->SetItemImage(event.GetItem(), eTCIT_FolderOpened);
}

void CBDTWxFrame::OnCollapseComponentFile(wxTreeEvent& event)
{
    m_pComponentFileTreeControl->SetItemImage(event.GetItem(), eTCIT_Folder);
}

void CBDTWxFrame::OnComponentSearchTextUpdate( wxCommandEvent& /*event*/ )
{
    m_uLastComponentSearchTextUpdateTime = GetTickCount();
    m_bComponentSearchTextUpdate = true;
}

void CBDTWxFrame::OnComponentFileSearchTextUpdate( wxCommandEvent& /*event*/ )
{
    m_uLastComponentFileSearchTextUpdateTime = GetTickCount();
    m_bComponentFileSearchTextUpdate = true;
}

void CBDTWxFrame::OnComponentSearchIdle( wxCommandEvent& /*event*/ )
{
    if (m_bComponentSearchTextUpdate && GetTickCount() - m_uLastComponentSearchTextUpdateTime > 700)
    {
        m_bComponentSearchTextUpdate = false;
        wxString szText = m_pComponentSearchCtrl->GetValue();
        m_pComponentTreeControl->CollapseAll();
        for (std::map<size_t, wxTreeItemId>::iterator iter = m_componentTreeIdMap.begin(); iter != m_componentTreeIdMap.end(); ++iter)
        {
            wxString lableText = m_pComponentTreeControl->GetItemText(iter->second);
            bool bMatch = lableText.Find(szText) != -1;
            // Because we can't hide tree control item, I set the text color to bg color to simulate hide.
            m_pComponentTreeControl->SetItemTextColour(iter->second, bMatch ? 0 : 0xFFFFFFFF);
            if (bMatch)
            {
                m_pComponentTreeControl->EnsureVisible(iter->second);
            }
        }
    }
}

void CBDTWxFrame::OnComponentFileSearchIdle( wxCommandEvent& /*event*/ )
{
    if (m_bComponentFileSearchTextUpdate && GetTickCount() - m_uLastComponentFileSearchTextUpdateTime > 700)
    {
        m_bComponentFileSearchTextUpdate = false;
        wxString szText = m_pComponentFileSearchCtrl->GetValue();
        m_pComponentFileTreeControl->CollapseAll();
        for (std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
        {
            wxString lableText = m_pComponentFileTreeControl->GetItemText(iter->second);
            bool bMatch = lableText.Find(szText) != -1;
            // Because we can't hide tree control item, I set the text color to bg color to simulate hide.
            m_pComponentFileTreeControl->SetItemTextColour(iter->second, bMatch ? 0 : 0xFFFFFFFF);
            if (bMatch)
            {
                m_pComponentFileTreeControl->EnsureVisible(iter->second);
            }
        }
    }
}

void CBDTWxFrame::OnComponentPropertyChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* pProperty = event.GetProperty();
    OnComponentPropertyChangedImpl(pProperty);
    event.Skip();
}

void CBDTWxFrame::OnComponentPropertyChangedImpl(wxPGProperty* pProperty)
{
    void* pClientData = pProperty->GetClientData();
    if (pClientData != NULL)
    {
        CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pClientData);
        BEATS_ASSERT(pPropertyBase != NULL);
        if (pPropertyBase->GetComboProperty() != NULL)
        {
            int nSelection = pProperty->GetValue().GetInteger();
            wxString label = pProperty->GetChoices().GetLabel(nSelection);
            pPropertyBase->GetValueByTChar(label, pPropertyBase->GetValue(eVT_CurrentValue));
            // Force update the host component, becuase we have set to the current value by GetValueByTChar.
            pPropertyBase->SetValueWithType(pPropertyBase->GetValue(eVT_CurrentValue), eVT_CurrentValue, true);
        }
        else
        {
            wxVariant var = pProperty->GetValue();
            pPropertyBase->SetValue(var, false);
        }
        bool bModified = !pPropertyBase->IsDataSame(true);
        pProperty->SetModifiedStatus(bModified);
        UpdatePropertyVisiblity(pPropertyBase);
        pProperty->RefreshEditor();
        m_pPropertyGridManager->Refresh();
    }
}

void CBDTWxFrame::UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase)
{
    // Do visible check logic.
    std::set<CWxwidgetsPropertyBase*>& effectProperties = pPropertyBase->GetEffectProperties();
    std::set<CWxwidgetsPropertyBase*>::iterator iter = effectProperties.begin();
    for (; iter != effectProperties.end(); ++iter)
    {
        bool bShouldBeVisible = (*iter)->CheckVisibleTrigger();
        wxPGProperty* pPGProperty = GetPGPropertyByBase(*iter);
        BEATS_ASSERT(pPGProperty != NULL, _T("Can't find property %s in data grid!"), (*iter)->GetBasicInfo()->m_displayName.c_str());
        if (pPGProperty != NULL)
        {
            pPGProperty->Hide(!bShouldBeVisible);
        }
    }
}

wxPGProperty* CBDTWxFrame::GetPGPropertyByBase(CPropertyDescriptionBase* pBase)
{
    wxPGProperty* pRet = NULL;
    for (wxPropertyGridIterator it = m_pPropertyGridManager->GetCurrentPage()->GetIterator(); !it.AtEnd(); ++it)
    {
        if (it.GetProperty()->GetClientData() == pBase)
        {
            pRet = it.GetProperty();
            break;
        }
    }
    return pRet;
}

void CBDTWxFrame::OnComponentPropertySelected(wxPropertyGridEvent& event)
{
    event.Veto();
}

void CBDTWxFrame::OnComponentPropertyRightClick(wxPropertyGridEvent& event)
{
    wxPGProperty* pProperty = event.GetProperty();
    m_pPropertyMenu->Enable(ePMS_ResetValue, pProperty->HasFlag(wxPG_PROP_MODIFIED) != 0);
    m_pPropertyMenu->Enable(ePMS_Expand, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) != 0);
    m_pPropertyMenu->Enable(ePMS_Collapsed, pProperty->GetChildCount() > 0 && pProperty->HasFlag(wxPG_PROP_COLLAPSED) == 0);
    PopupMenu(m_pPropertyMenu, wxDefaultPosition);
}

void CBDTWxFrame::OnComponentPropertyMenuClicked(wxMenuEvent& event)
{
    wxPGProperty* pProperty = m_pPropertyGridManager->GetGrid()->GetSelection();
    if (pProperty)
    {
        switch(event.GetId())
        {
        case ePMS_ResetValue:
            ResetPropertyValue(pProperty);
            break;
        case ePMS_Expand:
            pProperty->SetExpanded(true);
            break;
        case ePMS_Collapsed:
            pProperty->SetExpanded(false);
            break;
        default:
            BEATS_ASSERT(false, _T("Never reach here!"));
            break;
        }
        pProperty->RefreshEditor();
        pProperty->GetGrid()->Refresh();
    }
}

void CBDTWxFrame::OnComponentFileListMenuClicked( wxMenuEvent& event )
{
    wxTreeItemId item = m_pComponentFileTreeControl->GetSelection();
    if (item.IsOk())
    {
        switch(event.GetId())
        {
        case eFLMS_AddFile:
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
                            m_pComponentFileTreeControl->SelectItem(iter->second);
                        }
                    }
                    else
                    {
                        boost::filesystem::path filePath(result.c_str());
                        if (!boost::filesystem::exists(filePath))
                        {
                            const TCHAR* pExtensionStr = filePath.extension().c_str();
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
                        wxTreeItemId newItemId = m_pComponentFileTreeControl->AppendItem(item, fileName, eTCIT_File, -1, pData);
                        m_pComponentFileTreeControl->Expand(item);
                        m_componentFileListMap[result] = newItemId;

                        // When Add a new file in project, we must validate the id in this file. if id is conflicted, we have to resolve it.
                        CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(item));
                        BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
                        std::map<size_t, std::vector<size_t>> conflictMap;
                        pCurItemData->GetProjectDirectory()->AddFile(result, conflictMap);
                        ResolveIdConflict(conflictMap);
                        ActivateFile(result.c_str());
                    }
                }
            }
            break;
        case eFLMS_AddFileFolder:
            {
                TString strNewName = wxGetTextFromUser(_T("文件夹名"), _T("添加文件夹"), _T("New File Folder Name"));
                if (strNewName.length() > 0)
                {
                    CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(item));
                    BEATS_ASSERT(pCurItemData != NULL && pCurItemData->IsDirectory());
                    CComponentProjectDirectory* pNewProjectData = pCurItemData->GetProjectDirectory()->AddDirectory(strNewName);
                    if (pNewProjectData != NULL)
                    {
                        CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pNewProjectData, strNewName);
                        m_pComponentFileTreeControl->AppendItem(item, strNewName, eTCIT_Folder, -1, pData);
                        m_pComponentFileTreeControl->Expand(item);
                    }
                    else
                    {
                        MessageBox(NULL, _T("The name has already exist!"), _T("Error"), MB_OK);
                    }
                }
            }
            break;
        case eFLMS_Delete:
            {
                int iRet = wxMessageBox(_T("是否要删除物理文件？"), _T("删除文件"), wxYES_NO);
                DeleteItemInComponentFileList(item, iRet == wxYES);
                SelecteComponent(NULL);
                wxTreeItemId parentItem = m_pComponentFileTreeControl->GetItemParent(item);
                CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(item));
                CComponentFileTreeItemData* pParentItemData = NULL;
                CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
                if (parentItem.IsOk())
                {
                    pParentItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(parentItem));
                }
                bool bIsRootDirectory = pParentItemData == NULL;
                if (!bIsRootDirectory)
                {
                    BEATS_ASSERT(item != m_pComponentFileTreeControl->GetRootItem(), _T("Only root doesn't have a parent!"));
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
                    m_pComponentFileTreeControl->Delete(item);
                }
                else
                {
                    BEATS_ASSERT(item == m_pComponentFileTreeControl->GetRootItem(), _T("Only root doesn't have a parent!"));
                    CComponentProxyManager::GetInstance()->CloseFile();
                    pProject->GetRootDirectory()->DeleteAll(true);
                    m_pComponentFileTreeControl->DeleteChildren(item);// Never delete the root item.
                }
            }
            break;
        case eFLMS_OpenFileDirectory:
            {
                TString path = CComponentProxyManager::GetInstance()->GetProject()->GetProjectFilePath();
                CComponentFileTreeItemData* pCurItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(item));
                if (m_pComponentFileTreeControl->GetRootItem() != item)
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
            break;
        default:
            BEATS_ASSERT(false, _T("Never reach here!"));
            break;
        }
    }
}

void CBDTWxFrame::DeleteItemInComponentFileList( wxTreeItemId itemId, bool bDeletePhysicalFile)
{
    BEATS_ASSERT(itemId.IsOk());
    CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeControl->GetItemData(itemId);
    BEATS_ASSERT(pData != NULL);
    if (pData->IsDirectory())
    {
        wxTreeItemIdValue idValue;
        wxTreeItemId childId = m_pComponentFileTreeControl->GetFirstChild(itemId, idValue);
        while (childId.IsOk())
        {
            DeleteItemInComponentFileList(childId, bDeletePhysicalFile);
            childId = m_pComponentFileTreeControl->GetNextChild(itemId, idValue);
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

void CBDTWxFrame::OpenComponentFile( const TCHAR* pFilePath )
{
    BEATS_ASSERT(pFilePath != NULL && pFilePath[0] != 0, _T("Invalid file path"));
    CComponentProxyManager::GetInstance()->OpenFile(pFilePath);
    m_pComponentRenderWindowSizer->GetStaticBox()->SetLabel(wxString::Format(_T("渲染区    当前文件:%s"), pFilePath));
    size_t uPageCount = m_pComponentPageNoteBook->GetPageCount();
    int iComponentListPageIndex = -1;
    for (size_t i = 0; i < uPageCount; ++i)
    {
        if (m_pComponentPageNoteBook->GetPage(i) == m_pComponentListPanel)
        {
            iComponentListPageIndex = i;
            break;
        }
    }
    if (iComponentListPageIndex == -1)
    {
        m_pComponentPageNoteBook->AddPage(m_pComponentListPanel, _T("组件选择"));
    }
}

void CBDTWxFrame::CloseComponentFile(bool bRemindSave /*= true*/)
{
    CComponentProxyManager* pComponentManager = CComponentProxyManager::GetInstance();
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
        m_pComponentRenderWindowSizer->GetStaticBox()->SetLabel(_T("未命名"));
        size_t uPageCount = m_pComponentPageNoteBook->GetPageCount();
        int iComponentListPageIndex = -1;
        for (size_t i = 0; i < uPageCount; ++i)
        {
            if (m_pComponentPageNoteBook->GetPage(i) == m_pComponentListPanel)
            {
                iComponentListPageIndex = i;
                break;
            }
        }
        if (iComponentListPageIndex >= 0)
        {
            m_pComponentPageNoteBook->RemovePage(iComponentListPageIndex);
        }
    }
}

void CBDTWxFrame::ResolveIdConflict(const std::map<size_t, std::vector<size_t>>& conflictIdMap)
{
    if (conflictIdMap.size() > 0)
    {
        TCHAR szConflictInfo[10240];
        _stprintf(szConflictInfo, _T("工程中有%d个ID发生冲突，点击\"是\"开始解决,点击\"否\"退出."), conflictIdMap.size());
        int iRet = MessageBox(NULL, szConflictInfo, _T("解决ID冲突"), MB_YESNO);
        if (iRet == IDYES)
        {
            CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
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

void CBDTWxFrame::OnComponentSaveButtonClick(wxCommandEvent& /*event*/)
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    if (pProject->GetRootDirectory() != NULL)
    {
        TString szSavePath = CBDTWxApp::GetBDTWxApp()->GetWorkingPath();
        szSavePath.append(_T("/")).append(EXPORT_STRUCTURE_DATA_PATCH_XMLFILENAME);
        CComponentProxyManager::GetInstance()->SaveTemplate(szSavePath.c_str());
        // Save Instance File
        CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
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
                CComponentProxyManager::GetInstance()->GetProject()->SaveProject();
                const TString curWorkingFile = CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath();
                if (curWorkingFile.length() > 0)
                {
                    CComponentProxyManager::GetInstance()->SaveToFile(curWorkingFile.c_str());
                }
            }
            UpdatePropertyGrid();
        }
    }
}

void CBDTWxFrame::OnOperateFileButtonClick(wxCommandEvent& /*event*/)
{
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    TString strProjectFullPath = pProject->GetProjectFilePath();

    if (strProjectFullPath.length() == 0)
    {
        TString strWorkingPath = CBDTWxApp::GetBDTWxApp()->GetWorkingPath();
        CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, strProjectFullPath, _T("选择要读取的文件"), COMPONENT_PROJECT_EXTENSION, strWorkingPath.c_str());
        if (strProjectFullPath.length() > 0)
        {
            OpenProjectFile(strProjectFullPath.c_str());
            m_pOperateFileButton->SetLabel(_T("关闭工程"));
        }
    }
    else
    {
        CloseProjectFile();
        m_pOperateFileButton->SetLabel(_T("打开工程"));
    }
}

void CBDTWxFrame::OnExportButtonClick(wxCommandEvent& /*event*/)
{
    //Simulate.
    wxCommandEvent simulateEvent;
    OnComponentSaveButtonClick(simulateEvent);
    TString szBinaryPath;
    CUtilityManager::GetInstance()->AcquireSingleFilePath(false, NULL, szBinaryPath, _T("选择要导出的文件"), BINARIZE_FILE_EXTENSION_FILTER, NULL);
    if (szBinaryPath.length() > 0)
    {
        std::vector<TString> filesToExport;
        for (std::map<TString, wxTreeItemId>::iterator iter = m_componentFileListMap.begin(); iter != m_componentFileListMap.end(); ++iter)
        {
            CComponentFileTreeItemData* pData = (CComponentFileTreeItemData*)m_pComponentFileTreeControl->GetItemData(iter->second);
            BEATS_ASSERT(pData != NULL && !pData->IsDirectory());
            TString filePath(pData->GetFileName());
            filesToExport.push_back(filePath);
        }
        boost::filesystem::path binaryPath(szBinaryPath.c_str());
        TString pExtensionStr = binaryPath.extension().c_str();
        if (_tcsicmp(pExtensionStr.c_str(), BINARIZE_FILE_EXTENSION) != 0)
        {
            szBinaryPath.append(BINARIZE_FILE_EXTENSION);
        }
        CComponentProxyManager::GetInstance()->Export(filesToExport, szBinaryPath.c_str());
        //Export function will cause these operation: open->export->close->change file->open->...->restore open the origin file.
        //So we will update the dependency line as if we have just open a new file.
        m_pComponentRenderWindow->UpdateAllDependencyLine();
        wxMessageBox(wxT("导出完毕!"));
    }
}

void CBDTWxFrame::SelecteComponent(CComponentEditorProxy* pComponentInstance)
{
    m_pPropertyGridManager->ClearPage(0);
    m_pSelectedComponent = pComponentInstance;
    if (pComponentInstance != NULL)
    {
        // If we select an instance(not a template).
        if (pComponentInstance->GetId() != -1)
        {
            m_pComponentTreeControl->Unselect();
        }
        InsertComponentsInPropertyGrid(pComponentInstance);
    }
}

CComponentEditorProxy* CBDTWxFrame::GetSelectedComponent()
{
    return m_pSelectedComponent;
}

wxPGEditor* CBDTWxFrame::GetPtrEditor()
{
    return m_pPtrButtonEditor;
}

void CBDTWxFrame::InsertComponentsInPropertyGrid( CComponentEditorProxy* pComponent, wxPGProperty* pParent /*= NULL*/ )
{
    // Set Guid property.
    wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pPGProperty->SetValue(wxString::Format(_T("0x%x"), pComponent->GetGuid()));
    pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
    pPGProperty->SetName(_T("Guid"));
    pPGProperty->SetLabel(_T("Guid"));
    m_pPropertyGridManager->GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
    // Set Property for Id
    if (pComponent->GetId() != -1)
    {
        wxPGProperty* pPGProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
        pPGProperty->SetValue(wxString::Format(_T("%d"), pComponent->GetId()));
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, true);
        pPGProperty->SetName(_T("Id"));
        pPGProperty->SetLabel(_T("Id"));
        m_pPropertyGridManager->GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
    }
    InsertInPropertyGrid(*pComponent->GetPropertyPool(), pParent);
}

void CBDTWxFrame::InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent /* = NULL*/)
{
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        CWxwidgetsPropertyBase* pPropertyBase = const_cast<CWxwidgetsPropertyBase*>(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
        wxEnumProperty* pComboProperty = pPropertyBase->GetComboProperty();
        wxPGProperty* pPGProperty =  pComboProperty ? pPropertyBase->CreateComboProperty() : pPropertyBase->CreateWxProperty();
        BEATS_ASSERT(pPGProperty != NULL);
        BEATS_ASSERT(pParent != pPGProperty, _T("Can't insert a property in itself"));

        pPGProperty->SetName(pPropertyBase->GetBasicInfo()->m_variableName);
        pPGProperty->SetLabel(pPropertyBase->GetBasicInfo()->m_displayName);
        pPGProperty->ChangeFlag(wxPG_PROP_READONLY, !pPropertyBase->GetBasicInfo()->m_bEditable || pPropertyBase->GetType() == eRPT_Ptr || pPropertyBase->IsContainerProperty());
        pPGProperty->SetHelpString(pPropertyBase->GetBasicInfo()->m_tip);
        m_pPropertyGridManager->GetGrid()->GetState()->DoInsert(pParent, -1, pPGProperty);
        // This function can only be called after property be inserted to grid, or it will crash. It's a wxwidgets rule.
        pPGProperty->SetBackgroundColour(pPropertyBase->GetBasicInfo()->m_color);
        InsertInPropertyGrid(pPropertyBase->GetChildren(), pPGProperty);
    }
    // Update the visibility after all the properties have been inserted in the grid.
    for (size_t i = 0; i < pProperties.size(); ++i)
    {
        UpdatePropertyVisiblity(static_cast<CWxwidgetsPropertyBase*>(pProperties[i]));
    }
    m_pPropertyGridManager->Refresh();
}

void CBDTWxFrame::ResetPropertyValue(wxPGProperty* pProperty)
{
    CWxwidgetsPropertyBase* pPropertyBase = static_cast<CWxwidgetsPropertyBase*>(pProperty->GetClientData());
    pProperty->SetValue(pProperty->GetDefaultValue());
    if (pProperty->GetChildCount() > 0)
    {
        pProperty->DeleteChildren();
    }
    if (pPropertyBase->GetType() == eRPT_Ptr)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(pPropertyBase);
        pPtrProperty->DestroyInstance();
    }
    else if (pPropertyBase->IsContainerProperty())
    {
        pPropertyBase->DeleteAllChild();
    }
    pProperty->RecreateEditor();
    InsertInPropertyGrid(pPropertyBase->GetChildren(), pProperty);
    OnComponentPropertyChangedImpl(pProperty);
}

void CBDTWxFrame::UpdatePropertyGrid()
{
    m_pPropertyGridManager->ClearPage(0);
    if (GetSelectedComponent() != NULL)
    {
        InsertComponentsInPropertyGrid(GetSelectedComponent());
    }
}

void CBDTWxFrame::InitializeComponentTree(CComponentProjectDirectory* pProjectData, const wxTreeItemId& id)
{
    CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(pProjectData, pProjectData->GetName());
    m_pComponentFileTreeControl->SetItemData(id, pData);
    const std::vector<CComponentProjectDirectory*>& children = pProjectData->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        const TString& nameStr = (*iter)->GetName();
        wxTreeItemId newDirectoryId = m_pComponentFileTreeControl->AppendItem(id, nameStr, eTCIT_Folder, -1, NULL);
        InitializeComponentTree(*iter, newDirectoryId);
    }

    const std::set<size_t>& files = pProjectData->GetFileList();
    for (std::set<size_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TString strComopnentFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(*iter);
        CComponentFileTreeItemData* pData = new CComponentFileTreeItemData(NULL, strComopnentFileName);
        wxString pFileName = wxFileNameFromPath(strComopnentFileName);
        wxTreeItemId newFileId = m_pComponentFileTreeControl->AppendItem(id, pFileName, eTCIT_File, -1, pData);
        BEATS_ASSERT(m_componentFileListMap.find(strComopnentFileName) == m_componentFileListMap.end(), _T("Same File in a project! %s"), strComopnentFileName.c_str());
        m_componentFileListMap[strComopnentFileName] = newFileId;
    }
}

void CBDTWxFrame::OpenProjectFile( const TCHAR* pPath )
{
    if (pPath != NULL && _tcslen(pPath) > 0)
    {
        CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
        std::map<size_t, std::vector<size_t>> conflictIdMap;
        CComponentProjectDirectory* pProjectData = pProject->LoadProject(pPath, conflictIdMap);
        bool bEmptyProject = pProjectData == NULL;
        if (!bEmptyProject)
        {
            InitializeComponentTree(pProjectData, m_pComponentFileTreeControl->GetRootItem());
            ResolveIdConflict(conflictIdMap);
        }

        m_pComponentTreeControl->Show();
        m_pComponentFileTreeControl->ExpandAll();
        m_pComponentSaveButton->Enable(true);
        m_pSerializeButton->Enable(true);
    }
}

void CBDTWxFrame::CloseProjectFile()
{
    SelecteComponent(NULL);
    if (CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath().length() > 0)
    {
        CloseComponentFile();
    }
    CComponentProxyManager::GetInstance()->GetProject()->CloseProject();
    wxTreeItemId rootItem = m_pComponentFileTreeControl->GetRootItem();
    m_pComponentFileTreeControl->DeleteChildren(rootItem);
    CComponentFileTreeItemData* pRootItemData = static_cast<CComponentFileTreeItemData*>(m_pComponentFileTreeControl->GetItemData(rootItem));
    pRootItemData->SetData(NULL);
    m_componentFileListMap.clear();
    m_pComponentSaveButton->Enable(false);
    m_pSerializeButton->Enable(false);
}

void CBDTWxFrame::RequestToUpdatePropertyGrid( )
{
    if (m_bNeedUpdatePropertyGrid == false)
    {
        m_bNeedUpdatePropertyGrid = true;
    }
}

CComponentRenderWindow* CBDTWxFrame::GetComponentRenderWindow() const
{
    return m_pComponentRenderWindow;
}

void CBDTWxFrame::OnComponentPropertyGridIdle( wxIdleEvent& /*event*/ )
{
    if (m_bNeedUpdatePropertyGrid)
    {
        m_bNeedUpdatePropertyGrid = false;
        UpdatePropertyGrid();
    }
}
