#include "stdafx.h"
#include "BDTWxFrame.h"
#include "BDTWxApp.h"
#include "MemoryDetector/MemoryDetector.h"
#include "RightsDlg.h"
#include "../../Utility/Serializer/Serializer.h"

CBDTWxFrame::CBDTWxFrame( const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: wxFrame(NULL, wxID_ANY, title, pos, size,  style)
, m_samplingFrequency(30) 
, m_curSamplingCount(0)
, m_lineMaxValue(5)
, m_lineStep(5)
, m_memoryViewType(0)
, m_latestRecordTimeForUI(0)
, m_memoryRecordCountForUI(0)
, m_lastSelectedFilterPos(-1)
, m_memoryRecordCacheStartPos(0)
, m_initFlag(false)
, m_bMemoryUINeedUpdate(0)
, m_pFunc(NULL)
, m_uLastComponentSearchTextUpdateTime(0)
, m_uLastComponentFileSearchTextUpdateTime(0)
, m_bComponentSearchTextUpdate(false)
, m_bComponentFileSearchTextUpdate(false)
, m_pSelectedComponent(NULL)
, m_pComponentRenderWindow(NULL)
, m_bNeedUpdatePropertyGrid(false)
{
    m_pRightsDialog = new CRightsDialog(this);
}

CBDTWxFrame::~CBDTWxFrame()
{
    if (m_initFlag)
    {
        UnInit();
    }
}

void CBDTWxFrame::OnCloseWindow( wxCloseEvent& /*event*/ )
{
    Exit(true);
}

void CBDTWxFrame::OnClickAbout( wxCommandEvent& /*event*/ )
{
    m_pRightsDialog->ShowModal();
}

void CBDTWxFrame::OnClickExit( wxCommandEvent& /*event*/ )
{
    Exit(true);
}

void CBDTWxFrame::OnNoteBookChanged( wxBookCtrlEvent& event )
{
    size_t pageType = event.GetSelection();
    if (pageType == eBDTFT_MemoryDetect)
    {
        m_bMemoryUINeedUpdate = true;
    }
}

void CBDTWxFrame::Exit(bool bRemind)
{
    int result = 0;
    if (bRemind)
    {
        result = MessageBox(NULL, _T("退出后，debugtool将彻底停止工作，确定要退出吗？"),_T("退出"), MB_YESNO);
    }
    if (!bRemind || result == IDYES)
    {
        CMemoryDetector::GetInstance()->Uninit();
        CBDTWxApp::GetBDTWxApp()->Terminate();
    }
}

size_t CBDTWxFrame::GetMemoryViewType()
{
    // Don't try to call m_pMemoryViewChoice->GetSelection(), because it is an async method.
    // if we do so, we may get dead lock for critical section.
    return m_memoryViewType;
}

void CBDTWxFrame::SetGridRowsCount( wxGrid* gird, size_t count )
{
    int rowDelta = gird->GetRows() - count;
    if (rowDelta > 0)
    {
        gird->DeleteRows(count, rowDelta);
    }
    else if (rowDelta < 0)
    {
        gird->AppendRows(-rowDelta);
    }
}

bool CBDTWxFrame::IsAutoUpdateMemory()
{
    return m_pAutoUpdateMemoryCheckBox->IsChecked();
}

void CBDTWxFrame::Init(size_t funcSwitcher)
{
    SetSizeHints( wxDefaultSize, wxDefaultSize );

    m_pMainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    m_pMenuBar = new wxMenuBar( 0 );
    m_pFileMenu = new wxMenu();
    wxMenuItem* pExitMenuItem;
    pExitMenuItem = new wxMenuItem( m_pFileMenu, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
    m_pFileMenu->Append( pExitMenuItem );

    m_pMenuBar->Append( m_pFileMenu, wxT("文件") ); 

    m_pHelpMenu = new wxMenu();
    wxMenuItem* pAboutMenuItem;
    pAboutMenuItem = new wxMenuItem( m_pHelpMenu, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
    m_pHelpMenu->Append( pAboutMenuItem );

    m_pMenuBar->Append( m_pHelpMenu, wxT("帮助") ); 

    this->SetMenuBar( m_pMenuBar );

    m_pMainBoxSizer = new wxBoxSizer( wxVERTICAL );

    m_pNoteBook = new wxAuiNotebook( m_pMainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
    if (funcSwitcher & (1 << eBDTFT_SystemInfo))
    {
        CreateSystemInfoPage();
    }
    if (funcSwitcher & (1 << eBDTFT_Performance))
    {
        CreatePerformancePage();
    }
    if (funcSwitcher & (1 << eBDTFT_MemoryDetect))
    {
        CreateMemoryPage();
    }
    if (funcSwitcher & (1 << eBDTFT_InfoBoard))
    {
        CreateInfoSystemPage();
    }
    if (funcSwitcher & (1 << eBDTFT_ComponentSystem))
    {
        CreateComponentPage();
    }
    if (funcSwitcher & (1 << eBDTFT_Spy))
    {
        CreateSpyPage();
    }
    m_pCrashRptPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pNoteBook->AddPage( m_pCrashRptPanel, wxT("崩溃报告"), false );
    m_pNetworkInfoPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pNoteBook->AddPage( m_pNetworkInfoPanel, wxT("网络性能"), false );

    m_pMainBoxSizer->Add( m_pNoteBook, 1, wxEXPAND | wxALL, 5 );

    m_pMainPanel->SetSizer( m_pMainBoxSizer );
    m_pMainPanel->Layout();

    m_pMainPanel->Centre( wxBOTH );

    // Connect Events
    Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CBDTWxFrame::OnCloseWindow), NULL, this );
    Connect( pAboutMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnClickAbout ) );
    Connect( pExitMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnClickExit ) );
    m_pNoteBook->Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( CBDTWxFrame::OnNoteBookChanged ), NULL, this );
    m_pNoteBook->SetSelection(0);
    m_initFlag = true;
}

void CBDTWxFrame::UnInit()
{
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_SystemInfo))
    {
        DestroySystemInfoPage();
    }
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_Performance))
    {
        DestroyPerformancePage();
    }
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_MemoryDetect))
    {
        DestroyMemoryPage();
    }
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_InfoBoard))
    {
        DestroyInfoSystemPage();
    }
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_ComponentSystem))
    {
        DestroyComponentPage();
    }
    if (CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_Spy))
    {
        DestroySpyPage();
    }
    Destroy();
    // Disconnect Events
    Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( CBDTWxFrame::OnCloseWindow), NULL, this );
    Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnClickAbout ) );
    Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnClickExit ) );
    m_pNoteBook->Disconnect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( CBDTWxFrame::OnNoteBookChanged ), NULL, this );
    CloseHandle(m_pageCreateWaitEvent);
    m_pageCreateWaitEvent = NULL;
    m_initFlag = false;
}

bool CBDTWxFrame::IsInit()
{
    return m_initFlag;
}

bool CBDTWxFrame::IsMemoryUIChanged()
{
    return m_bMemoryUINeedUpdate || IsMemoryGridNeedUpdateByScroll();
}

bool CBDTWxFrame::IsMemoryRecordChanged( size_t recordCount, size_t latestRecordTime )
{
    bool bNewRecordAddIn = latestRecordTime > m_latestRecordTimeForUI;
    bool bOldRecordDeleted = (recordCount != m_memoryRecordCountForUI) && !bNewRecordAddIn;
    if (bOldRecordDeleted || bNewRecordAddIn)
    {
        m_latestRecordTimeForUI = latestRecordTime;
        m_memoryRecordCountForUI = recordCount;
    }
    return bOldRecordDeleted || bNewRecordAddIn;
}

void CBDTWxFrame::RegisterPropertyChangedHandler( TPropertyChangedHandler pFunc )
{
    m_pFunc = pFunc;
}

TPropertyChangedHandler CBDTWxFrame::GetPropertyChangedHandler()
{
    return m_pFunc;
}


