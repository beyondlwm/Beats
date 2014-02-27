// Network.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "Network.h"
#include "NetworkManager.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            bool bInitialized = CNetworkManager::GetInstance()->IsInitialized();
            BEATS_ASSERT(bInitialized, _T("Initialize network failed!"));
        }
        break;
    case DLL_PROCESS_DETACH:
        if (CNetworkManager::GetInstance()->IsInitialized())
        {
            CNetworkManager::Destroy();
        }
        break;
    default:
        break;
    }
    return TRUE;
}

extern "C"
{
    bool Beats_IsNetworkInitialized()
    {
        return CNetworkManager::GetInstance()->IsInitialized();
    }

    SOCKET Beats_CreateSocket(int type, int protocol)
    {
        return CNetworkManager::GetInstance()->CreateSocket(type, protocol);
    }

    bool Beats_DestroySocket(SOCKET socket)
    {
        return CNetworkManager::GetInstance()->DestroySocket(socket);
    }

    bool Beats_Bind(SOCKET socketToBind, const sockaddr_in* pSockInfo, INT nSockInfoSize, bool bAvoidConflictPort/* = false*/, size_t uTryRebindCount/* = 50*/)
    {
        return CNetworkManager::GetInstance()->Bind(socketToBind, pSockInfo, nSockInfoSize, bAvoidConflictPort, uTryRebindCount);
    }

    bool Beats_Listen(SOCKET socketToListen, int iBackLog)
    {
        return CNetworkManager::GetInstance()->Listen(socketToListen, iBackLog);
    }

    bool Beats_Accept(SOCKET listenSocket, SOCKET& newSocket, sockaddr_in* addr, int* addrlen)
    {
        return CNetworkManager::GetInstance()->Accept(listenSocket, newSocket, addr, addrlen);
    }

    bool Beats_SendTo(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uIP, size_t uPort, size_t uRetryCount/* = 0*/)
    {
        return CNetworkManager::GetInstance()->SendTo(sendingSocket, pData, uDataLength, uIP, uPort, uRetryCount);
    }

    bool Beats_RecvFrom(SOCKET receiveSocket, void* pData, size_t& uDataLength, sockaddr_in* pReceiveInfo/* = NULL*/, int* pInfoSize/* = NULL*/)
    {
        return CNetworkManager::GetInstance()->RecvFrom(receiveSocket, pData, uDataLength, pReceiveInfo, pInfoSize);
    }

    bool Beats_Send(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uFlag/* = 0*/)
    {
        return CNetworkManager::GetInstance()->Send(sendingSocket, pData, uDataLength, uFlag);
    }

    bool Beats_Receive(SOCKET receiveSocket, void* pData, size_t& uDataLength, size_t uFlag/* = 0*/)
    {
        return CNetworkManager::GetInstance()->Receive(receiveSocket, pData, uDataLength, uFlag);
    }

    bool Beats_Connect(SOCKET socketToConnect, const sockaddr_in* pSockAddress)
    {
        return CNetworkManager::GetInstance()->Connect(socketToConnect, pSockAddress, sizeof(*pSockAddress));
    }
    
    const hostent* Beats_GetHostInfo()
    {
        return CNetworkManager::GetInstance()->GetHostInfo();
    }

    const SBeatsSocket* Beats_GetSockInfo(SOCKET sock)
    {
        return CNetworkManager::GetInstance()->GetSocketInfo(sock);
    }

    PIP_ADAPTER_INFO Beats_CreateNetAdapterInfo()
    {
        return CNetworkManager::GetInstance()->CreateNetAdapterInfo();
    }

};


#ifdef _MANAGED
#pragma managed(pop)
#endif

