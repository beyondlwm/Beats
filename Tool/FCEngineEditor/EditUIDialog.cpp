#include "stdafx.h"
#include "EngineEditor.h"
#include "timebarframe.h"
#include "EditUIDialog.h"
#include "timebardataviewctrl.h"
#include "UIGLWindow.h"
#include "EngineProperGridManager.h"
#include "GUI/Window/WindowDefs.h"
#include "Resource/ResourceManager.h"
#include "Render/Texture.h"
#include "Render/TextureFragManager.h"
#include "GUI/WindowManager.h"
#include "GUI/Window/Window.h"
#include "GUI/Renderer/BaseRenderer.h"
#include "ComponentTreeItemData.h"
#include "wx/dataview.h"
#include "wx/variant.h"
#include "timebarlistmodel.h"
#include "timebarevent.h"

#include <wx/artprov.h>


static const TCHAR* FCGUIName[] = 
{
    _T("WindowBase"),
    _T("Button"),
    _T("CheckBox"),
    _T("Slider"),
    _T("Progress"),
    _T("TextEdiit")
};
enum EMenuObject
{
    Menu_ObjectView_Delete,
    Menu_ObjectView_Paste,
    Menu_ObjectView_Copy
};

BEGIN_EVENT_TABLE(EditUIDialog, EditDialogBase)
EVT_AUITOOLBAR_BEGIN_DRAG(wxID_ANY, EditUIDialog::OnToolBarDrag)
EVT_TREE_BEGIN_DRAG(wxID_ANY,EditUIDialog::OnCtrlsTreeBeginDrag)
EVT_TREE_END_DRAG(wxID_ANY,EditUIDialog::OnCtrlsTreeEndDrag)
EVT_DATAVIEW_SELECTION_CHANGED(ID_DataView_ObjectView, EditUIDialog::OnObjectViewSeletionChange)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_DataView_ObjectView, EditUIDialog::OnRightClick)
EVT_MENU(wxID_ANY, EditUIDialog::OnPopMenu)
EVT_DATAVIEW_CUSTRENDER_ACTIVE(wxID_ANY, EditUIDialog::OnDataViewRenderer)
END_EVENT_TABLE()

EditUIDialog::EditUIDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : EditDialogBase(parent, id, title, pos, size, style, name)
    , m_pTempGUI(NULL)
    , m_pSelectedGUI(NULL)
{
    m_Manager.SetManagedWindow(this);
    CreateAuiToolBar();
    InitCtrls();
    m_Manager.Update();

}

EditUIDialog::~EditUIDialog()
{
    m_Manager.UnInit();
}

void EditUIDialog::CreateAuiToolBar()
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
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
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

    wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    tb3->SetToolBitmapSize(wxSize(16,16));
    tb3->AddTool(ID_ToolBar2_Button1, wxT("Disabled"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb3->AddTool(ID_ToolBar2_Button2, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb3->SetCustomOverflowItems(prepend_items, append_items);
    tb3->Realize();

    m_Manager.AddPane(tb1,
        wxAuiPaneInfo().Name(wxT("tb1")).
        ToolbarPane().Top().Position(0).Dockable(false));
    m_Manager.AddPane(tb2,
        wxAuiPaneInfo().Name(wxT("tb2")).
        ToolbarPane().Top().Position(1).Dockable(false));
    m_Manager.AddPane(tb3,
        wxAuiPaneInfo().Name(wxT("tb3")).
        ToolbarPane().Left().Position(0).Dockable(false));
}

void EditUIDialog::CreateAuiNoteBook()
{
    wxSize client_size = GetClientSize();
    long lStyle = wxAUI_NB_DEFAULT_STYLE & ~wxAUI_NB_CLOSE_ON_ACTIVE_TAB;
    
    client_size = wxSize(client_size.GetWidth() * 0.4, client_size.GetHeight() * 0.3);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCanvasPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pCanvasPanel->SetSizer(pSizer);
    m_pCanvasNotebook = new wxAuiNotebook(m_pCanvasPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pCanvasNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pObjectViewPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pObjectViewPanel->SetSizer(pSizer);
    m_pObjectViewNotebook = new wxAuiNotebook(m_pObjectViewPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pObjectViewNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pGLWindowPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pGLWindowPanel->SetSizer(pSizer);
    m_pGLWindowNotebook = new wxAuiNotebook(m_pGLWindowPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pGLWindowNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pPropertyPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pPropertyPanel->SetSizer(pSizer);
    m_pPropertyNotebook = new wxAuiNotebook(m_pPropertyPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pPropertyNotebook, 1, wxGROW|wxALL, 0);

    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pTimeBarPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pTimeBarPanel->SetSizer(pSizer);
    m_pTimeBarNotebook = new wxAuiNotebook(m_pTimeBarPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pTimeBarNotebook, 1, wxGROW|wxALL, 0);
    
    pSizer = new wxBoxSizer(wxVERTICAL);
    m_pCtrlsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, client_size);
    m_pCtrlsPanel->SetSizer(pSizer);
    m_pCtrlsNotebook = new wxAuiNotebook(m_pCtrlsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, lStyle);
    pSizer->Add(m_pCtrlsNotebook, 1, wxGROW|wxALL, 0);
}

void EditUIDialog::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropGridManager = new EnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pPropertyNotebook, wxID_ANY, wxDefaultPosition, wxSize(100, 100), style );
    m_pPropGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_pPropGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));

    m_pPropertyNotebook->Freeze();
    m_pPropertyNotebook->AddPage( m_pPropGridManager, wxT("PropGrid") );
    m_pPropertyNotebook->Thaw();

    m_Manager.AddPane(m_pPropertyPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("PropertyPane")).
        Right());
}

void EditUIDialog::InitCtrls()
{
    wxGLContext* pContext = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGLContext();
    CreateMenu();
    CreateAuiNoteBook();
    CreatePropertyGrid();
    CreateCtrlsTree();
    CreateGUITexture(_T("DemoShop0.png"));
    m_pTimeBar = new TimeBarFrame(m_pTimeBarNotebook);
    m_pGLWindow = new CUIGLWindow(m_pGLWindowNotebook, pContext);
    m_pCanvasList = new wxListBox(m_pCanvasNotebook, wxID_ANY);
    m_pObjectView = new TimeBarDataViewCtrl(m_pObjectViewNotebook, ID_DataView_ObjectView, wxDefaultPosition, wxDefaultSize, wxLB_NO_SB);


    m_pCanvasNotebook->Freeze();
    m_pCanvasNotebook->AddPage( m_pCanvasList, wxT("Canvas") );
    m_pCanvasNotebook->Thaw();

    m_pObjectViewNotebook->Freeze();
    m_pObjectViewNotebook->AddPage( m_pObjectView, wxT("ObjectView") );
    m_pObjectViewNotebook->Thaw();

    m_pGLWindowNotebook->Freeze();
    m_pGLWindowNotebook->AddPage( m_pGLWindow, wxT("GLWindow") );
    m_pGLWindowNotebook->Thaw();

    m_pTimeBarNotebook->Freeze();
    m_pTimeBarNotebook->AddPage( m_pTimeBar, wxT("TimeBar") );
    m_pTimeBarNotebook->Thaw(); 

    m_Manager.AddPane(m_pCanvasPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("CanvasPane")).
        Left().Layer(1).Position(0));
    m_Manager.AddPane(m_pObjectViewPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("ObjectViewPane")).
        Left().Layer(1).Position(1)); 
    m_Manager.AddPane(m_pGLWindowPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("GLWindowPane")).
        Center());
    m_Manager.AddPane(m_pTimeBarPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("TimeBarPane")).
        Bottom());
}

void EditUIDialog::OnToolBarDrag( wxAuiToolBarEvent& event )
{
    int iID = event.GetToolId();
    switch (iID)
    {
    case ID_ToolBar2_Button1:
        break;
    case ID_ToolBar2_Button2:
        break;
    default:
        break;
    }
}

void EditUIDialog::OnCtrlsTreeBeginDrag( wxTreeEvent& event )
{
    using namespace FCGUI;
    CComponentTreeItemData* pData = static_cast<CComponentTreeItemData*>(m_pCtrlsTree->GetItemData(event.GetItem()));
    switch (pData->GetGUID())
    {
    case WINDOW_BUTTON :
        m_pTempGUI = WindowManager::GetInstance()->Create<Button>(GetAvailableName(_T("TestButton")), nullptr);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonNormal"), Button::STATE_NORMAL);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonPressed"), Button::STATE_PRESSED);
        m_pTempGUI->Renderer()->AddLayer(_T("ButtonDisabled"), Button::STATE_DISABLED);
        m_pTempGUI->SetSize(100, 50);
        break;
    case WINDOW_CHECKBOX :
        m_pTempGUI = WindowManager::GetInstance()->Create<Button>(GetAvailableName(_T("TestCheckBox")), nullptr);
        m_pTempGUI->Renderer()->AddLayer(_T("SlotArmour"));
        m_pTempGUI->SetSize(100, 50);
        break;
    case WINDOW_SLIDER :

        break;
    case WINDOW_PROGRESS :

        break;
    default:
        break;
    }
    event.Allow();
    m_pGLWindow->SetTempGUI(m_pTempGUI);
}

void EditUIDialog::OnCtrlsTreeEndDrag( wxTreeEvent& /*event*/ )
{
    if (m_pGLWindow->IsMouseInWindow())
    {
        m_pGLWindow->SetActiveGUI(m_pTempGUI);
        m_pObjectView->AddItem(m_pTempGUI->Name(), true, false);
    }
    else
    {
        FCGUI::WindowManager::GetInstance()->DestroyWindow(m_pTempGUI);
    }
    m_pGLWindow->SetTempGUI(nullptr);
    m_pTempGUI = nullptr;
}

void EditUIDialog::CreateCtrlsTree()
{
    m_pCtrlsTree = new wxTreeCtrl(m_pCtrlsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_NO_LINES | wxTR_EDIT_LABELS | wxSUNKEN_BORDER);
    CComponentTreeItemData* itemData = NULL;
    wxTreeItemId rootItem = m_pCtrlsTree->AddRoot(_T("root"));
    
    for (int i = 0; i < FCGUI::WINDOW_COUNT; i++)
    {
        itemData = new CComponentTreeItemData(false, i);
        m_pCtrlsTree->AppendItem(rootItem, FCGUIName[i], -1, -1, itemData);
    }

    m_pCtrlsNotebook->Freeze();
    m_pCtrlsNotebook->AddPage( m_pCtrlsTree, wxT("Ctrls") );
    m_pCtrlsNotebook->Thaw();

    m_Manager.AddPane(m_pCtrlsPanel, 
        wxAuiPaneInfo().CenterPane().Name(wxT("CtrlsPane")).
        Left().Layer(0));
}

void EditUIDialog::CreateGUITexture(const TString& strFilePath)
{
    SharePtr<CTexture> texture = 
        CResourceManager::GetInstance()->GetResource<CTexture>(strFilePath, false);

    kmVec2 point;
    kmVec2Fill(&point, 168, 0);
    kmVec2 size;
    kmVec2Fill(&size, 421, 318);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("BG"), texture, point, size);
    kmVec2Fill(&point, 338, 18);
    kmVec2Fill(&size, 1, 1);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ShopPanel"), texture, point, size);
    kmVec2Fill(&point, 546, 320);
    kmVec2Fill(&size, 118, 44);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("SlotArmour"), texture, point, size);
    kmVec2Fill(&point, 490, 320);
    kmVec2Fill(&size, 54, 54);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("Shield"), texture, point, size);
    kmVec2Fill(&point, 591, 0);
    kmVec2Fill(&size, 50, 23);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonNormal"), texture, point, size);
    kmVec2Fill(&point, 643, 0);
    kmVec2Fill(&size, 50, 23);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonPressed"), texture, point, size);
    kmVec2Fill(&point, 602, 345);
    kmVec2Fill(&size, 1, 1);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonDisabled"), texture, point, size);

    using namespace FCGUI;
    Window *wndBG = WindowManager::GetInstance()->Create<Window>(_T("BG"), nullptr);
    wndBG->SetPosSize(0.f, 0.f, 0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f);
    wndBG->Renderer()->AddLayer(_T("BG"));
    wndBG->SetAnchor(0.5f, 0.5f);
}

TString EditUIDialog::GetAvailableName( const TString& strWindowName )
{
    using namespace FCGUI;
    Window* pRoot = WindowManager::GetInstance()->RootWindow();
    TString ResultName;
    int iWindowCount = WindowManager::GetInstance()->GetWindowCount() + 1;
    for (int i = 0; i < iWindowCount; i++)
    {
        TCHAR szBuffer[32];
        _stprintf(szBuffer, _T("%d"), i);
        ResultName = strWindowName + szBuffer;
        if (pRoot->GetChild(ResultName, true) == nullptr)
        {
            break;
        }
    }
    return ResultName;
}

void EditUIDialog::OnObjectViewSeletionChange( wxDataViewEvent& event )
{
    using namespace FCGUI;
    wxVariant windowName;
    Window* pRoot = WindowManager::GetInstance()->RootWindow();
    m_pObjectView->GetUserModel()->GetValue(windowName, event.GetItem(), 0);
    TString str = windowName.GetString();
    m_pSelectedGUI = pRoot->GetChild(str, true);
}

void EditUIDialog::OnRightClick( wxDataViewEvent& /*event*/ )
{
    PopupMenu(m_pObjectViewMenu, wxDefaultPosition);
}

void EditUIDialog::CreateMenu()
{
    m_pObjectViewMenu = new wxMenu;
    m_pObjectViewMenu->Append(Menu_ObjectView_Copy, _T("Copy"));
    m_pObjectViewMenu->Append(Menu_ObjectView_Paste, _T("Paste"));
    m_pObjectViewMenu->Append(Menu_ObjectView_Delete, _T("Delete"));
}

void EditUIDialog::OnPopMenu( wxCommandEvent& event )
{
    
    int eID = event.GetId();
    switch (eID)
    {
    case Menu_ObjectView_Copy:
        break;
    case Menu_ObjectView_Paste:
        break;
    case Menu_ObjectView_Delete:
        RemoveSelectedGUI();
        break;
    default:
        break;
    }

}

void EditUIDialog::RemoveSelectedGUI()
{
    using namespace FCGUI;
    if (m_pSelectedGUI)
    {
        m_pObjectView->GetUserModel()->DeleteItem(m_pObjectView->GetRow(m_pObjectView->GetSelection()));
        WindowManager::GetInstance()->DestroyWindow(m_pSelectedGUI);
        m_pSelectedGUI = NULL;
    }
}

void EditUIDialog::OnDataViewRenderer( TimeBarEvent& event )
{
    switch (event.GetType())
    {
    case Col_Visible :
        //TODO:
        break;
    case Col_Lock :

        break;
    default:
        break;
    }
}

void EditUIDialog::LanguageSwitch()
{
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance()->GetInstance();
    m_pCtrlsNotebook->SetPageText(0, pLanguageManager->GetText(eL_Ctrls));
    m_pTimeBarNotebook->SetPageText(0, pLanguageManager->GetText(eL_TimeBar));
    m_pPropertyNotebook->SetPageText(0, pLanguageManager->GetText(eL_PropGrid));
    m_pGLWindowNotebook->SetPageText(0, pLanguageManager->GetText(eL_GLWindow));
    m_pObjectViewNotebook->SetPageText(0, pLanguageManager->GetText(eL_ObjectView));
    m_pCanvasNotebook->SetPageText(0, pLanguageManager->GetText(eL_Canvas));

    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Copy, pLanguageManager->GetText(eL_Copy));
    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Paste, pLanguageManager->GetText(eL_Paste));
    m_pObjectViewMenu->SetLabel(Menu_ObjectView_Delete, pLanguageManager->GetText(eL_Delete));
}

