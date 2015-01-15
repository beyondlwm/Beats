#include "stdafx.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include <wx/wrapsizer.h>
#include <WinSock2.h>
#include "../../Network/Network.h"
#include "Spy/SpyUserPanel.h"

void CBDTWxFrame::CreateSpyPage()
{
#ifdef _DEBUG
    m_hSpyDllHandle = LoadLibrary(_T("spy_d.dll"));
    BEATS_ASSERT(m_hSpyDllHandle != NULL, _T("加载Spy_d.dll失败！"));
#else
    m_hSpyDllHandle = LoadLibrary(_T("spy_r.dll"));
    BEATS_ASSERT(m_hSpyDllHandle != NULL, _T("加载Spy_r.dll失败！"));
#endif
    if (m_hSpyDllHandle != NULL)
    {
        typedef bool (*TInitFunc)();
        TInitFunc pInitFunc = (TInitFunc)GetProcAddress(m_hSpyDllHandle, "Init");
        pInitFunc();

        m_pSpyPanel = new wxPanel(m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
        wxBoxSizer* pSpyMainSizer = new wxBoxSizer(wxVERTICAL);
        m_pSpyNoteBook = new wxNotebook(m_pSpyPanel, wxID_ANY);
        pSpyMainSizer->Add(m_pSpyNoteBook, 1, wxEXPAND, 5);

        m_pSpyInfoPanel = new wxPanel(m_pSpyNoteBook, wxID_ANY);
        m_pSpyNoteBook->AddPage(m_pSpyInfoPanel, _T("概览"));
        wxBoxSizer* pSpyInfoSizer = new wxBoxSizer(wxHORIZONTAL);

        wxBoxSizer* pLeftSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticBoxSizer* pTextInfoSizer = new wxStaticBoxSizer(wxVERTICAL, m_pSpyInfoPanel, _T("本机信息"));

        typedef const hostent* (*TGetHostInfoFunc)();
        TGetHostInfoFunc pGetHostInfoProc = (TGetHostInfoFunc)GetProcAddress(m_hSpyDllHandle, "Spy_GetHostInfo");
        BEATS_ASSERT(pGetHostInfoProc != NULL, _T("Get Function Spy_GetHostInfo failed!"));
        const hostent* pHostEnt = pGetHostInfoProc();

        m_pSpyHostNameLabel = new wxStaticText(m_pSpyInfoPanel, wxID_ANY, wxString::Format("本机名称: %s", pHostEnt->h_name));
        m_pSpyNetAdapterInfoLabel = new wxStaticText(m_pSpyInfoPanel, wxID_ANY, _T("网卡信息:"));
        m_pSpyNetAdapterInfoWnd = new wxScrolledWindow(m_pSpyInfoPanel, wxID_ANY, wxDefaultPosition, wxSize(150, 100), wxNO_BORDER|wxVSCROLL);
        m_pSpyNetAdapterInfoWnd->SetScrollbars(1, 1, 0, 0);
        m_pSpyNetAdapterInfoText = new wxStaticText( m_pSpyNetAdapterInfoWnd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
        
        typedef bool (*TGetNetAdapterInfo)(std::string&);
        TGetNetAdapterInfo pGetNetAdapterInfo = (TGetNetAdapterInfo)GetProcAddress(m_hSpyDllHandle, "Spy_GetNetAdapterInfo");
        BEATS_ASSERT(pGetNetAdapterInfo != NULL, _T("Get Function Spy_CreateNetAdapterInfo Failed!"));
        std::string info;
        if (pGetNetAdapterInfo != NULL && pGetNetAdapterInfo(info))
        {
            m_pSpyNetAdapterInfoText->SetLabelText(info.c_str());
        }
        else
        {
            m_pSpyNetAdapterInfoText->SetLabelText(_T("获取网卡信息失败！"));
        }

        wxSize sz = m_pSpyNetAdapterInfoText->GetClientSize();
        m_pSpyNetAdapterInfoWnd->SetVirtualSize(sz);

        pTextInfoSizer->Add(m_pSpyHostNameLabel, 0, wxEXPAND, 0);
        pTextInfoSizer->Add(m_pSpyNetAdapterInfoLabel, 0, wxEXPAND, 0);
        pTextInfoSizer->Add(m_pSpyNetAdapterInfoWnd, 0, wxEXPAND, 0);

        pLeftSizer->Add(pTextInfoSizer, 1, wxEXPAND, 0);
        pSpyInfoSizer->Add(pLeftSizer, 1, wxEXPAND, 0);

        wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
        wxStaticBoxSizer* pFuncBtnSizer = new wxStaticBoxSizer(wxHORIZONTAL, m_pSpyInfoPanel, _T("功能区"));
        wxFlexGridSizer* pFuncBtnGridSizer = new wxFlexGridSizer(0, 3, wxSize(3, 3));
        
        m_pSpyConnectBtn = new wxButton(m_pSpyInfoPanel, wxID_ANY, _T("创建连接"));
        pFuncBtnGridSizer->Add(m_pSpyConnectBtn, 0, wxALL, 10);
        pFuncBtnSizer->Add(pFuncBtnGridSizer, 0, wxEXPAND, 0);
        pRightSizer->Add(pFuncBtnSizer, 1, wxEXPAND, 0);

        pSpyInfoSizer->Add(pRightSizer, 1, wxEXPAND, 0);

        m_pSpyInfoPanel->SetSizer(pSpyInfoSizer);
        m_pSpyInfoPanel->Layout();

        m_pSpyPanel->SetSizer(pSpyMainSizer);
        m_pSpyPanel->Layout();
        m_pNoteBook->AddPage( m_pSpyPanel, wxT("远程连接"), false );
    }

    m_pSpyConnectBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnSpyConnectButtonClick ), NULL, this );
}

void CBDTWxFrame::DestroySpyPage()
{
    if (m_hSpyDllHandle != NULL)
    {        
        typedef bool (*TUninitFunc)();
        TUninitFunc pUninitFunc = (TUninitFunc)GetProcAddress(m_hSpyDllHandle, "Uninit");
        pUninitFunc();

        FreeLibrary(m_hSpyDllHandle);
        m_hSpyDllHandle = NULL;
    }
}

void CBDTWxFrame::OnSpyConnectButtonClick(wxCommandEvent& /*event*/)
{
    wxString ipAddress = wxGetTextFromUser(_T("请输入连接的IP地址"), wxGetTextFromUserPromptStr, _T("192.168.1.1"));
    if (ipAddress.length() > 0)
    {
        unsigned long uIpAddress = inet_addr(ipAddress);
        BEATS_ASSERT(uIpAddress != INADDR_NONE && uIpAddress != 0, _T("输入的IP地址 %s 不合法!"), ipAddress);
        if (uIpAddress != INADDR_NONE)
        {
            typedef const SBeatsSocket* (*TConnectFunc)(uint32_t);
            TConnectFunc pConnectProc = (TConnectFunc)GetProcAddress(m_hSpyDllHandle, "Spy_Connect");
            const SBeatsSocket* pSockInfo = pConnectProc(uIpAddress);
            if (pSockInfo != NULL)
            {
                CSpyUserPanel* pUserPanel = new CSpyUserPanel(pSockInfo, m_hSpyDllHandle, m_pSpyNoteBook);
                m_pSpyNoteBook->AddPage(pUserPanel, _T("User"), true);
            }
            else
            {
                wxMessageBox(wxString::Format(_T("连接到 %s 失败!"), ipAddress));
            }
        }
    }
}
