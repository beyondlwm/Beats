#ifndef BEATS_SPY_SPYMANAGER_H__INCLUDE
#define BEATS_SPY_SPYMANAGER_H__INCLUDE
#include "../Network/Network.h"

static const UINT SPY_DEFAULT_PORT = 46041;

class CSpyHookBase;
class CSpyManager
{
    BEATS_DECLARE_SINGLETON(CSpyManager);
public:
    bool Initialize();
    bool Uninitialize();

    SharePtr<SSocketContext> ConnectToSpy(SOCKET sock, sockaddr_in* pAddr, bool bRegister = true);
    void RegisterHook(CSpyHookBase* pHook);
    void UnRegisterHook(CSpyHookBase* pHook);
    SharePtr<SSocketContext> GetSocket(SOCKET sock);

private:
    bool InitializeNetwork();
    bool UninitializeNetwork();
    void RegisterSocket(SharePtr<SSocketContext>& pNewSocket);
    void UnregisterSocket(SharePtr<SSocketContext>& pSocketContext);
    bool GetSocket(SharePtr<SSocketContext>& pOutSocket, uint32_t uIndex);

    static DWORD LoopThreadFunc(LPVOID param);
    static DWORD NetworkReceiveThreadFunc(LPVOID param);
    static DWORD NetworkSendThreadFunc(LPVOID param);


private:
    bool m_bInitialized;
    bool m_bExitLoopThreadSignal;
    bool m_bExitRecvThreadSignal;
    bool m_bExitSendThreadSignal;
    SOCKET m_listenSocket;
    HANDLE m_hLoopThread;
    HANDLE m_hRecvThread;
    HANDLE m_hSendThread;

    fd_set m_readSet;
    fd_set m_exceptionSet;
    int m_iMaxFD;
    std::vector<SharePtr<SSocketContext>> m_socketContainer;
    std::vector<CSpyHookBase*> m_spyHookList;
    CRITICAL_SECTION m_socketContextSection;
};

#endif