#ifndef BEATS_NETWORK_NETWORK_H__INCLUDE
#define BEATS_NETWORK_NETWORK_H__INCLUDE

#include <Winsock2.h>
#include <Wsnwlink.h>

typedef struct _IP_ADAPTER_INFO *PIP_ADAPTER_INFO;
struct SBeatsSocket
{
    SBeatsSocket()
        : m_socket(INVALID_SOCKET)
        , m_uCreateTime(GetTickCount())
        , m_uSendDataCount(0)
        , m_uReceiveDataCount(0)
        , m_uDownloadSpeed(0)
        , m_uUploadSpeed(0)
        , m_uLastDownloadSpeedCalcTime(0)
        , m_uReceiveDataCountInCalcTime(0)
        , m_uLastUploadSpeedCalcTime(0)
        , m_uSendDataCountInCalcTime(0)
    {

    }
    ~SBeatsSocket()
    {

    }
    SOCKET m_socket;
    size_t m_uCreateTime;
    size_t m_uDownloadSpeed;
    size_t m_uUploadSpeed;
    size_t m_uLastDownloadSpeedCalcTime;
    size_t m_uReceiveDataCountInCalcTime;
    size_t m_uLastUploadSpeedCalcTime;
    size_t m_uSendDataCountInCalcTime;
    long long m_uSendDataCount;
    long long m_uReceiveDataCount;
    static const size_t SPEED_CALC_INTERVAL = 2000;//Ms
};


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _USRDLL
#ifdef NETWORK_EXPORTS
#define BEATS_NETWORK_DLL_DECL __declspec(dllexport)
#else
#define BEATS_NETWORK_DLL_DECL __declspec(dllimport)
#endif
#else
#define BEATS_NETWORK_DLL_DECL
#endif

    BEATS_NETWORK_DLL_DECL bool Beats_IsNetworkInitialized();
    BEATS_NETWORK_DLL_DECL SOCKET Beats_CreateSocket(int type, int protocol);
    BEATS_NETWORK_DLL_DECL bool Beats_DestroySocket(SOCKET socket);
    BEATS_NETWORK_DLL_DECL bool Beats_Bind(SOCKET socketToBind, const sockaddr_in* pSockInfo, INT nSockInfoSize, bool bAvoidConflictPort = false, size_t uTryRebindCount = 50);
    BEATS_NETWORK_DLL_DECL bool Beats_Listen(SOCKET socketToListen, int iBackLog);
    BEATS_NETWORK_DLL_DECL bool Beats_Accept(SOCKET listenSocket, SOCKET& newSocket, sockaddr_in* addr, int* addrlen);
    BEATS_NETWORK_DLL_DECL bool Beats_Connect(SOCKET connectSocket, const sockaddr_in* pAddr);
    BEATS_NETWORK_DLL_DECL bool Beats_SendTo(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uIP, size_t uPort, size_t uRetryCount = 0);
    BEATS_NETWORK_DLL_DECL bool Beats_RecvFrom(SOCKET receiveSocket, void* pData, size_t& uDataLength, sockaddr_in* pReceiveInfo = NULL, int* pInfoSize = NULL);
    BEATS_NETWORK_DLL_DECL bool Beats_Send(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uFlag = 0);
    BEATS_NETWORK_DLL_DECL bool Beats_Receive(SOCKET receiveSocket, void* pData, size_t& uDataLength, size_t uFlag = 0);
    BEATS_NETWORK_DLL_DECL const hostent* Beats_GetHostInfo();
    BEATS_NETWORK_DLL_DECL const SBeatsSocket* Beats_GetSockInfo(SOCKET sock);
    BEATS_NETWORK_DLL_DECL PIP_ADAPTER_INFO Beats_CreateNetAdapterInfo();

#ifdef __cplusplus
}
#endif

#endif