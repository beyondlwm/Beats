#include "stdafx.h"
#include "StartUpFrame.h"
#include "BDTWxApp.h"
#include "MemoryDetector/MemoryDetector.h"
#include <Psapi.h>

CStartUpFrame::CStartUpFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(NULL, wxID_ANY, title, pos, size,  style)
, m_funcSwitcher(0xffffffff)
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    m_pMainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

    wxBoxSizer* bMainSizer;
    bMainSizer = new wxBoxSizer( wxVERTICAL );

    m_pWelcomeText = new wxStaticText( m_pMainPanel, wxID_ANY, wxT("请设置启动参数"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pWelcomeText->Wrap( -1 );
    bMainSizer->Add( m_pWelcomeText, 0, wxALL, 5 );

    wxBoxSizer* bSelectionSizer;
    bSelectionSizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bFunctionSizer;
    bFunctionSizer = new wxBoxSizer( wxVERTICAL );

    wxString m_pFunctionRadioBoxChoices[] = { wxT("硬件检测"), wxT("性能分析"), wxT("内存监控"), wxT("组件系统"), wxT("游戏信息")};
    int m_pFunctionRadioBoxNChoices = sizeof( m_pFunctionRadioBoxChoices ) / sizeof( wxString );
    m_pFunctionRadioBox = new wxRadioBox( m_pMainPanel, wxID_ANY, wxT("功能选项"), wxDefaultPosition, wxDefaultSize, m_pFunctionRadioBoxNChoices, m_pFunctionRadioBoxChoices, 3, wxRA_SPECIFY_ROWS );
    m_pFunctionRadioBox->SetSelection( 1 );
    bFunctionSizer->Add( m_pFunctionRadioBox, 0, wxALL, 5 );

    m_pEnableCheckBox = new wxCheckBox( m_pMainPanel, wxID_ANY, wxT("开启"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pEnableCheckBox->SetValue(true); 
    bFunctionSizer->Add( m_pEnableCheckBox, 0, wxALL, 5 );

    m_pStartBtn = new wxButton( m_pMainPanel, wxID_ANY, wxT("启动"), wxDefaultPosition, wxDefaultSize, 0 );
    bFunctionSizer->Add( m_pStartBtn, 0, wxALL, 5 );
    this->SetDefaultItem(m_pStartBtn);
    
    m_pExitBtn = new wxButton( m_pMainPanel, wxID_ANY, wxT("退出"), wxDefaultPosition, wxDefaultSize, 0 );
    bFunctionSizer->Add( m_pExitBtn, 0, wxALL, 5 );


    bSelectionSizer->Add( bFunctionSizer, 1, wxEXPAND, 5 );

    m_pHardwareParamPanel = new wxPanel( m_pMainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bHardwarePanelSizer;
    bHardwarePanelSizer = new wxBoxSizer( wxVERTICAL );


    m_pHardwareParamPanel->SetSizer( bHardwarePanelSizer );
    m_pHardwareParamPanel->Layout();
    bHardwarePanelSizer->Fit( m_pHardwareParamPanel );
    bSelectionSizer->Add( m_pHardwareParamPanel, 1, wxEXPAND | wxALL, 5 );

    m_pPerformParamPanel = new wxPanel( m_pMainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pPerformParamPanel->Hide();

    wxBoxSizer* bPerformPanelSizer;
    bPerformPanelSizer = new wxBoxSizer( wxVERTICAL );


    m_pPerformParamPanel->SetSizer( bPerformPanelSizer );
    m_pPerformParamPanel->Layout();
    bPerformPanelSizer->Fit( m_pPerformParamPanel );
    bSelectionSizer->Add( m_pPerformParamPanel, 1, wxEXPAND | wxALL, 5 );

    m_pMemoryParamnel = new wxPanel( m_pMainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_pMemoryParamnel->Hide();

    wxBoxSizer* bMemoryPanelSizer;
    bMemoryPanelSizer = new wxBoxSizer( wxVERTICAL );

    m_pDllListTxt = new wxStaticText( m_pMemoryParamnel, wxID_ANY, wxT("选择监测模块"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pDllListTxt->Wrap( -1 );
    bMemoryPanelSizer->Add( m_pDllListTxt, 0, wxALL, 5 );

    m_pHookDllList = new wxListBox( m_pMemoryParamnel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_ALWAYS_SB|wxLB_HSCROLL|wxLB_MULTIPLE|wxLB_SORT ); 
    bMemoryPanelSizer->Add( m_pHookDllList, 0, wxALL|wxEXPAND, 5 );


    m_pMemoryParamnel->SetSizer( bMemoryPanelSizer );
    m_pMemoryParamnel->Layout();
    bMemoryPanelSizer->Fit( m_pMemoryParamnel );
    bSelectionSizer->Add( m_pMemoryParamnel, 1, wxEXPAND | wxALL, 5 );

    bMainSizer->Add( bSelectionSizer, 1, wxALL|wxEXPAND, 5 );


    m_pMainPanel->SetSizer( bMainSizer );
    m_pMainPanel->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    m_pFunctionRadioBox->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CStartUpFrame::OnFunctionButtonSelected ), NULL, this );
    m_pEnableCheckBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CStartUpFrame::OnFunctionEnabled ), NULL, this );
    m_pStartBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CStartUpFrame::OnLaunchBtnClick ), NULL, this );
    m_pExitBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CStartUpFrame::OnExitBtnClick ), NULL, this );
    FillModuleList();
}

CStartUpFrame::~CStartUpFrame()
{
    // Disconnect Events
    m_pFunctionRadioBox->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( CStartUpFrame::OnFunctionButtonSelected ), NULL, this );
    m_pEnableCheckBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CStartUpFrame::OnFunctionEnabled ), NULL, this );
    m_pStartBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CStartUpFrame::OnLaunchBtnClick ), NULL, this );
    m_pExitBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CStartUpFrame::OnExitBtnClick ), NULL, this );
}

void CStartUpFrame::OnFunctionButtonSelected( wxCommandEvent& event )
{
    EBDTFuncType type = (EBDTFuncType)event.GetSelection();
    m_pHardwareParamPanel->Hide();
    m_pPerformParamPanel->Hide();
    m_pMemoryParamnel->Hide();
    
    switch(type)
    {
    case eBDTFT_SystemInfo:
        m_pHardwareParamPanel->Show(true);
        break;
    case eBDTFT_Performance:
        m_pPerformParamPanel->Show(true);
        break;
    case eBDTFT_MemoryDetect:
        m_pMemoryParamnel->Show(true);
        break;
    }
    m_pMainPanel->Layout();
    bool bEnable = (m_funcSwitcher & (1 << type)) > 0;
    m_pEnableCheckBox->SetValue(bEnable);
}

void CStartUpFrame::OnFunctionEnabled( wxCommandEvent& event )
{
    bool bEnable = event.GetInt() > 0;
    EBDTFuncType type = (EBDTFuncType)m_pFunctionRadioBox->GetSelection();
    if (bEnable)
    {
        m_funcSwitcher |= 1 << type;
    }
    else
    {
        m_funcSwitcher &= ~ (1 << type);
    }
}

void CStartUpFrame::OnExitBtnClick( wxCommandEvent& /*event*/ )
{
    static_cast<CBDTWxApp*>(CBDTWxApp::GetInstance())->Launch(0, NULL);
}

void CStartUpFrame::OnLaunchBtnClick( wxCommandEvent& /*event*/ )
{
    std::vector<std::string> moduleList;
    wxArrayInt indexArray;
    m_pHookDllList->GetSelections(indexArray);

    for (size_t i = 0;i < indexArray.GetCount(); ++i)
    {
        std::string tmp = m_pHookDllList->GetString(indexArray[i]);
        moduleList.push_back(tmp);
        //TODO: Call function below leads to crash. it's a wxwidgets bug.
        //m_pHookDllList->GetString(indexArray[i]).ToStdString();
    }
    if (moduleList.size() == 0)
    {
        m_funcSwitcher &= ~ (1 << eBDTFT_MemoryDetect);
    }
    static_cast<CBDTWxApp*>(CBDTWxApp::GetInstance())->Launch(m_funcSwitcher, &moduleList);
}

void CStartUpFrame::FillModuleList()
{
    HMODULE moduleCache[1024];
    DWORD cacheNeeded = 0;
    HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, GetCurrentProcessId() );

    EnumProcessModules(hProcess, moduleCache, sizeof(moduleCache), &cacheNeeded);
    BEATS_ASSERT(cacheNeeded < sizeof(moduleCache), _T("Moudule cache is too small!"));
    size_t moduleCount = cacheNeeded / sizeof(HMODULE);
    for (size_t i = 0; i < moduleCount; ++i)
    {
        TCHAR nameCache[256];
        GetModuleFileName(moduleCache[i],nameCache, sizeof(nameCache));

        if (CMemoryDetector::GetInstance()->IsHookableModule(moduleCache[i]))
        {
            m_pHookDllList->Append(nameCache);
        }
    }
}

size_t CStartUpFrame::GetFuncSwitcher()
{
    return m_funcSwitcher;
}
