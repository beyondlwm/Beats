#ifndef BEATS_NETWORK_NETWORKMANAGER_H__INCLUDE
#define BEATS_NETWORK_NETWORKMANAGER_H__INCLUDE

#include "Network.h"
typedef struct _IP_ADAPTER_INFO *PIP_ADAPTER_INFO;

class CNetworkManager
{
    BEATS_DECLARE_SINGLETON(CNetworkManager);
public:
    bool IsInitialized();
    // Common Interface
    SOCKET CreateSocket(int type, int protocol);
    bool DestroySocket(SOCKET pSocket);

    bool Bind(SOCKET socketToBind, const sockaddr_in* pSockInfo, INT nSockInfoSize, bool bAvoidConflictPort = false, uint32_t uTryRebindCount = 50);
    bool Listen(SOCKET socketToListen, int iBackLog);
    bool Accept(SOCKET listenSocket, SOCKET& newSocket, sockaddr_in* pAddress, int* iAddrLen);
    bool Connect(SOCKET connectSockent, const sockaddr_in* pAddress, int iAddrLen);

    bool SendTo(SOCKET sendingSocket, void* pData, uint32_t& uDataLength, uint32_t uIP, uint32_t uPort, uint32_t uRetryCount = 0);
    bool RecvFrom(SOCKET receiveSocket, void* pData, uint32_t& uDataLength, sockaddr_in* pReceiveInfo = NULL, int* pInfoSize = NULL);
    bool Send(SOCKET sendingSocket, void* pData, uint32_t& uDataLength, uint32_t uFlag = 0);
    bool Receive(SOCKET receiveSocket, void* pData, uint32_t& uDataLength, uint32_t uFlag = 0);

    const SBeatsSocket* GetSocketInfo(SOCKET socket);
    const hostent* GetHostInfo();

    PIP_ADAPTER_INFO CreateNetAdapterInfo();

private:
    bool Initialize();
    bool UnInitialize();

    bool RegisterSocket(SOCKET socketToRegister);
    bool UnregisterSocket(SOCKET socketToRegister);

private:
    bool m_bInitialized;
    std::map<SOCKET, SBeatsSocket*> m_socketMap;
    CRITICAL_SECTION m_socketMapSection;

};

#endif