#include "stdafx.h"
#include <wx/artprov.h>
#include "FCEngineEditor.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include "ConstantCurveProperty.h"
#include "ConstantCurveEditor.h"
#include "EditAnimationDialog.h"
#include "timebarframe.h"

#define MAINFRAMESIZE wxSize(1024, 800)
#define MAINFRAMEPOSITION wxPoint(40, 40)
#define BUTTONSIZE wxSize(20, 20)
#define SIZERBORDERWIDTH 5
enum 
{
    PARTICLE_SIMULATION_WORLD = 1,
    PARTICLE_SIMULATION_LOCAL
};

enum
{
    Menu_Menu_Append,
    Menu_EditAnimation = 1111
};

BEGIN_EVENT_TABLE(CEditorMainFrame, wxFrame)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_CLICKED, CEditorMainFrame::OnAuiButton)
EVT_MENU(Menu_EditAnimation, CEditorMainFrame::OnEditAnimationMenuItem)
END_EVENT_TABLE()

CEditorMainFrame::CEditorMainFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, MAINFRAMEPOSITION, MAINFRAMESIZE)
{
    SetIcon(wxICON(sample));
    m_Manager.SetManagedWindow(this);
    m_pWEditAnimation = NULL;
    InitMenu();
    InitCtrls();

    SetStatusBar(new wxStatusBar(this));
    SetStatusText(wxT("Welcome!"));
    SetMinSize(wxSize(400,300));

    Centre();
 }

CEditorMainFrame::~CEditorMainFrame()
{
    m_Manager.UnInit();
}

void CEditorMainFrame::InitMenu()
{
    wxMenuBar* pMenuBar     = new wxMenuBar;
    wxMenu* pFileMenu       = new wxMenu;
    wxMenu* pEditMenu       = new wxMenu;
    wxMenu* pAssetsMenu     = new wxMenu;
    wxMenu* pGameObjMenu    = new wxMenu;
    wxMenu* pConponentMenu  = new wxMenu;
    wxMenu* pWindowMenu     = new wxMenu;
    wxMenu* pHelpMenu       = new wxMenu;
    
    pMenuBar->Append(pFileMenu, wxT("&File"));
    pMenuBar->Append(pEditMenu, wxT("&Edit"));
    pMenuBar->Append(pAssetsMenu, wxT("&Assets"));
    pMenuBar->Append(pGameObjMenu, wxT("&GameObject"));
    pMenuBar->Append(pConponentMenu, wxT("&Conponent"));
    pMenuBar->Append(pWindowMenu, wxT("&Window"));
    pMenuBar->Append(pHelpMenu, wxT("&Help"));

    pEditMenu->Append(Menu_EditAnimation, wxT("Edit Animation"));

    SetMenuBar(pMenuBar);
}

void CEditorMainFrame::InitCtrls()
{
    CreateAuiToolBar();
    CreateAuiNoteBook();
    CreateTreeCtrl();
    CreateGLCanvas();
    CreateTimeBar();
    CreatePropertyGrid();

    AddPageToBook();
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
    m_pLeft = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pRight = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pBottom = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
    m_pCenter = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, client_size, lStyle);
}

void CEditorMainFrame::CreateGLCanvas()
{
    //m_pGLCanvas = new GLAnimationCanvas(m_pCenter);
}

void CEditorMainFrame::CreateTimeBar()
{
    m_pTimeBar = new TimeBarFrame(m_pBottom);
}

void CEditorMainFrame::CreatePropertyGrid()
{
    int style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLBAR | wxPG_DESCRIPTION | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    wxPGEditor* pEditor = wxPropertyGrid::RegisterEditorClass(new ConstantCurveEditor);
    m_pPropGridManager = new EnginePropertyGirdManager();
    m_pPropGridManager->Create(m_pRight, wxID_ANY, wxDefaultPosition, wxSize(100, 100), style );
    m_pPropGridManager->InitComponentsPage();
    m_propGrid = m_pPropGridManager->GetGrid();
    m_pPropGridManager->SetExtraStyle(extraStyle);
    m_pPropGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    m_propGrid->SetVerticalSpacing( 2 );
    m_pPropGridManager->AddPage(wxT("page"));
    wxPropertyGridPage* pPage = m_pPropGridManager->GetPage(0);
    
    pPage->Append( new wxPropertyCategory(wxT("Transform"), wxPG_LABEL));

    wxPGProperty* pID = new wxStringProperty(wxT("Position"), wxPG_LABEL, "<composed>");
    pID->AddPrivateChild(new wxFloatProperty( wxT("X"), wxPG_LABEL, 0.0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Y"), wxPG_LABEL, 0.0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Z"), wxPG_LABEL, 0.0));
    pPage->Append(pID);

    pID = new wxStringProperty(wxT("Rotation"), wxPG_LABEL, "<composed>");
    pID->AddPrivateChild(new wxFloatProperty( wxT("X"), wxPG_LABEL, 0.0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Y"), wxPG_LABEL, 0.0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Z"), wxPG_LABEL, 0.0));
    pPage->Append(pID);

    pID = new wxStringProperty(wxT("Scale"), wxPG_LABEL, "<composed>");
    pID->AppendChild (new wxFloatProperty( wxT("X"), wxPG_LABEL, 0.0));
    pID->AppendChild (new wxFloatProperty( wxT("Y"), wxPG_LABEL, 0.0));
    pID->AppendChild (new wxFloatProperty( wxT("Z"), wxPG_LABEL, 0.0));
    pPage->Append(pID);

    pPage->Append( new wxPropertyCategory(wxT("Particle System Infor"), wxPG_LABEL));

    pID = new wxStringProperty(wxT("Particle System"), wxPG_LABEL);
    pID->AddPrivateChild(new wxFloatProperty( wxT("Duration"), wxPG_LABEL, 5.0));
    pID->AddPrivateChild(new wxBoolProperty( wxT("Looping"), wxPG_LABEL, false));
    pID->AddPrivateChild(new wxBoolProperty( wxT("Prewarm"), wxPG_LABEL, false));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Start Delay"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Start LifeTime"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Start Speed"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Start Size"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Start Rotation"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxColourProperty( wxT("Start Color"), wxPG_LABEL, wxColour(125, 200, 95, 128)));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Gravity Multiplier"), wxPG_LABEL, 0));
    pID->AddPrivateChild(new wxFloatProperty( wxT("Inherit Velocity"), wxPG_LABEL, 0));
    wxPGChoices Choices;
    Choices.Add(wxT("World"), PARTICLE_SIMULATION_WORLD);
    Choices.Add(wxT("Local"), PARTICLE_SIMULATION_LOCAL);
    pID->AddPrivateChild(new wxEnumProperty( wxT("Simulation Space"), wxPG_LABEL, Choices, PARTICLE_SIMULATION_WORLD));
    pID->AddPrivateChild(new wxBoolProperty( wxT("Play On Awake"), wxPG_LABEL, false));
    pID->AddPrivateChild(new wxIntProperty( wxT("Max Particles"), wxPG_LABEL, 1000));
    pPage->Append(pID);
    
    pID = new wxStringProperty(wxT("Emossion"), wxPG_LABEL);
    pPage->Append(pID);
    ConstantCurveProperty* pIDt = new ConstantCurveProperty( wxT("Play On Awake"));
    pIDt->SetEditor(pEditor);
    pPage->Append(pIDt);
    wxBitmap myTestBitmap(600, 30, DEFAULT_DEPTH);
    wxMemoryDC mdc;
    mdc.SelectObject(myTestBitmap);
    mdc.Clear();
    mdc.SetPen(*wxBLACK);
    mdc.DrawLine(0, 0, 600, 30);
    mdc.SelectObject(wxNullBitmap);
    pIDt->SetCurveBitmap( myTestBitmap );
    
    wxPGVIterator it;
    wxPropertyGrid* pg = m_pPropGridManager->GetGrid();

    for ( it = pg->GetVIterator( wxPG_ITERATE_ALL ); !it.AtEnd(); it.Next() )
    {
        bool bIsCategory = it.GetProperty()->IsCategory();
        if (!bIsCategory)
        {
            it.GetProperty()->SetExpanded( false );
        }
    }
    pg->RefreshGrid();
}

void CEditorMainFrame::CreateTreeCtrl()
{
    m_pTreeCtrl1 = new wxTreeCtrl(m_pLeft, wxID_ANY,
        wxPoint(0,0), wxSize(160,250),
        wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxImageList* imglist = new wxImageList(16, 16, true, 2);
    imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16,16)));
    imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16)));
    m_pTreeCtrl1->AssignImageList(imglist);

    wxTreeItemId root = m_pTreeCtrl1->AddRoot(wxT("wxAUI Project"), 0);
    wxArrayTreeItemIds items;

    items.Add(m_pTreeCtrl1->AppendItem(root, wxT("Item 1"), 0));
    items.Add(m_pTreeCtrl1->AppendItem(root, wxT("Item 2"), 0));
    items.Add(m_pTreeCtrl1->AppendItem(root, wxT("Item 3"), 0));
    items.Add(m_pTreeCtrl1->AppendItem(root, wxT("Item 4"), 0));
    items.Add(m_pTreeCtrl1->AppendItem(root, wxT("Item 5"), 0));

    int i, count;
    for (i = 0, count = items.Count(); i < count; ++i)
    {
        wxTreeItemId id = items.Item(i);
        m_pTreeCtrl1->AppendItem(id, wxT("Subitem 1"), 1);
        m_pTreeCtrl1->AppendItem(id, wxT("Subitem 2"), 1);
        m_pTreeCtrl1->AppendItem(id, wxT("Subitem 3"), 1);
        m_pTreeCtrl1->AppendItem(id, wxT("Subitem 4"), 1);
        m_pTreeCtrl1->AppendItem(id, wxT("Subitem 5"), 1);
    }

    m_pTreeCtrl1->Expand(root);
}

void CEditorMainFrame::AddPageToBook()
{
    m_pLeft->Freeze();
    m_pLeft->AddPage( m_pTreeCtrl1, wxT("wxTextCtrl 1") );
    m_pLeft->Thaw();

    m_pRight->Freeze();
    m_pRight->AddPage( m_pPropGridManager, wxT("Inspector") );
    m_pRight->Thaw();

    m_pBottom->Freeze();
    m_pBottom->AddPage( new wxTextCtrl( m_pBottom, wxID_ANY, wxT("Some more text")), wxT("wxTextCtrl 1") );
    m_pBottom->AddPage( m_pTimeBar, wxT("wxTextCtrl 2") );
    m_pBottom->Thaw();

    m_pCenter->Freeze();
    //m_pCenter->AddPage( m_pGLCanvas, wxT("wxTextCtrl 1") );
    m_pCenter->Thaw();

    m_Manager.AddPane(m_pLeft, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane1")).
        Left());
    m_Manager.AddPane(m_pRight, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane2")).
        Right());
    m_Manager.AddPane(m_pBottom, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane3")).Caption(wxT("Pane Caption")).
        Bottom());
    m_Manager.AddPane(m_pCenter, wxAuiPaneInfo().CenterPane().
        Name(wxT("Pane4")).Caption(wxT("Pane Caption")).
        Center());
}

void CEditorMainFrame::GetEditAnimationDialog()
{
    if (m_pWEditAnimation == NULL)
    {
        m_pWEditAnimation = new EditAnimationDialog(this, wxID_ANY, wxT("Edit Animation"));
    }
    m_pWEditAnimation->ShowModal();
}

void CEditorMainFrame::OnAuiButton(wxCommandEvent& event)
{
    int id = event.GetId();
    switch (id)
    {
    case Menu_EditAnimation :
        GetEditAnimationDialog();
        break;
    default:
        break;
    }
    return;
}

void CEditorMainFrame::OnEditAnimationMenuItem(wxCommandEvent& /*event*/)
{
    return;
}
