#include "stdafx.h"
#include "SpyManager.h"
#include "SpyCommandManager.h"
#include "SpyCommandMessageBox.h"
#include "SpyHookBase.h"
#include <process.h>
#include "../Utility/UtilityManager.h"


CSpyManager* CSpyManager::m_pInstance = NULL;

CSpyManager::CSpyManager()
: m_bInitialized(false)
, m_bExitLoopThreadSignal(false)
, m_bExitRecvThreadSignal(false)
, m_bExitSendThreadSignal(false)
, m_listenSocket(INVALID_SOCKET)
, m_hRecvThread(NULL)
, m_iMaxFD(0)
{
    FD_ZERO(&m_readSet);
    FD_ZERO(&m_exceptionSet);
}

CSpyManager::~CSpyManager()
{
    for (size_t i = 0; i < m_spyHookList.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_spyHookList[i]);
    }
    m_spyHookList.clear();
    CSpyCommandManager::Destroy();
    CUtilityManager::Destroy();
}

bool CSpyManager::Initialize()
{
    if (!m_bInitialized)
    {        
        m_bInitialized = CSpyCommandManager::GetInstance()->Initialize() && InitializeNetwork();
    }
    return m_bInitialized;
}

bool CSpyManager::Uninitialize()
{
    if (m_bInitialized)
    {
        m_bExitLoopThreadSignal = true;
        m_bExitRecvThreadSignal = true;
        m_bExitSendThreadSignal = true;
        WaitForSingleObject(m_hLoopThread, INFINITE);
        WaitForSingleObject(m_hRecvThread, INFINITE);
        WaitForSingleObject(m_hSendThread, INFINITE);

        UninitializeNetwork();
        CSpyCommandManager::GetInstance()->Uninitialize();

        CloseHandle(m_hLoopThread);
        CloseHandle(m_hRecvThread);
        CloseHandle(m_hSendThread);

        m_bInitialized = false;
    }
    
    return !m_bInitialized;
}

SharePtr<SSocketContext> CSpyManager::ConnectToSpy( SOCKET sock, sockaddr_in* pAddr, bool bRegister/* = true*/ )
{
    bool bConnect = Beats_Connect(sock, pAddr);
    SharePtr<SSocketContext> pContext;
    if (bConnect)
    {
        if (bRegister)
        {
            pContext = new SSocketContext;
            pContext->m_socket = sock;
            RegisterSocket(pContext);
        }
    }
    return pContext;
}

void CSpyManager::RegisterHook(CSpyHookBase* pHook)
{
    bool bFound = false;
    for (size_t i = 0; i < m_spyHookList.size(); ++i)
    {
        if (m_spyHookList[i] == pHook)
        {
            bFound = true;
            break;
        }
    }
    BEATS_ASSERT(!bFound, _T("Can't register a same hook twice!"));
    if (!bFound)
    {
        m_spyHookList.push_back(pHook);
    }
}

void CSpyManager::UnRegisterHook(CSpyHookBase* pHook)
{
    bool bFound = false;
    for (size_t i = 0; i < m_spyHookList.size(); ++i)
    {
        if (m_spyHookList[i] == pHook)
        {
            m_spyHookList[i] = m_spyHookList.back();
            m_spyHookList.pop_back();
            bFound = true;
            break;
        }
    }
    BEATS_ASSERT(bFound, _T("The Hook is not registered!"));
}

SharePtr<SSocketContext> CSpyManager::GetSocket(SOCKET sock)
{
    SharePtr<SSocketContext> ret;
    for (size_t i = 0; i < m_socketContainer.size(); ++i)
    {
        if (m_socketContainer[i]->m_socket == sock)
        {
            ret = m_socketContainer[i];
            break;
        }
    }
    return ret;
}

bool CSpyManager::InitializeNetwork()
{
    bool bRet = false;
    if (Beats_IsNetworkInitialized())
    {
        m_listenSocket = Beats_CreateSocket(SOCK_STREAM, 0);
        if (m_listenSocket != INVALID_SOCKET)
        {
            char szHostName[MAX_PATH];
            ::gethostname(szHostName, MAX_PATH);
            HOSTENT* pHostEnt = ::gethostbyname(szHostName);
            in_addr hostaddr[32];
            int nAddressCount = 0;
            if(pHostEnt != NULL)
            {
                while(pHostEnt->h_addr_list[nAddressCount] != NULL)
                {
                    hostaddr[nAddressCount] = *(in_addr*)pHostEnt->h_addr_list[nAddressCount];
                    nAddressCount++;
                }
            }
            if (nAddressCount > 0)
            {
                sockaddr_in listenSocketAddr;
                listenSocketAddr.sin_family = AF_INET;
                listenSocketAddr.sin_addr.s_addr = hostaddr->S_un.S_addr;
                listenSocketAddr.sin_port = htons(SPY_DEFAULT_PORT);

                if (Beats_Bind(m_listenSocket, &listenSocketAddr, sizeof(listenSocketAddr), true) &&
                    Beats_Listen(m_listenSocket, SOMAXCONN))
                {
                    InitializeCriticalSection(&m_socketContextSection);
                    FD_SET(m_listenSocket, &m_readSet);
                    m_iMaxFD = m_listenSocket;
                    size_t uLoopThreadId, uRecvThreadId, uSendThreadId;
                    m_hLoopThread = (HANDLE)(_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))LoopThreadFunc, (void*)this, 0, &uLoopThreadId));
                    CUtilityManager::GetInstance()->SetThreadName(uLoopThreadId, "NetLoop");
                    m_hRecvThread = (HANDLE)(_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))NetworkReceiveThreadFunc, (void*)this, 0, &uRecvThreadId));
                    CUtilityManager::GetInstance()->SetThreadName(uRecvThreadId, "NetRecv");
                    m_hSendThread = (HANDLE)(_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))NetworkSendThreadFunc, (void*)this, 0, &uSendThreadId));
                    CUtilityManager::GetInstance()->SetThreadName(uSendThreadId, "NetSend");

                    bRet = true;
                }
            }
        }
    }

    return bRet;
}

bool CSpyManager::UninitializeNetwork()
{
    Beats_DestroySocket(m_listenSocket);
    DeleteCriticalSection(&m_socketContextSection);
    return true;
}

void CSpyManager::RegisterSocket(SharePtr<SSocketContext>& pNewSocket)
{
    EnterCriticalSection(&m_socketContextSection);
    m_socketContainer.push_back(pNewSocket);
    FD_SET(pNewSocket->m_socket, &m_readSet);
    if (pNewSocket->m_socket > (size_t)m_iMaxFD)
    {
        m_iMaxFD = (int)pNewSocket->m_socket;
    }
    LeaveCriticalSection(&m_socketContextSection);
}

void CSpyManager::UnregisterSocket(SharePtr<SSocketContext>& pSocketContext)
{
    EnterCriticalSection(&m_socketContextSection);
    for(size_t i = 0; i < m_socketContainer.size(); ++i)
    {
        if(m_socketContainer[i] == pSocketContext)
        {
            FD_CLR(pSocketContext->m_socket, &m_readSet);
            if ((int)pSocketContext->m_socket == m_iMaxFD)
            {
                m_iMaxFD = -1;
                for (size_t i = 0; i < m_readSet.fd_count; ++i)
                {
                    if ((int)m_readSet.fd_array[i] > m_iMaxFD)
                    {
                        m_iMaxFD = m_readSet.fd_array[i];
                    }
                }
            }
            m_socketContainer[i]->SetBitFlag(eSCFB_Invalid, true);
            m_socketContainer[i] = m_socketContainer.back();
            m_socketContainer.pop_back();
            Beats_DestroySocket(pSocketContext->m_socket);
            break;
        }
    }
    LeaveCriticalSection(&m_socketContextSection);
}

bool CSpyManager::GetSocket(SharePtr<SSocketContext>& pOutSocket, size_t uIndex)
{
    bool bRet = false;
    EnterCriticalSection(&m_socketContextSection);
    if (uIndex < m_socketContainer.size())
    {
        bRet = true;
        pOutSocket = m_socketContainer[uIndex];
    }
    LeaveCriticalSection(&m_socketContextSection);
    return bRet;
}

DWORD CSpyManager::LoopThreadFunc(LPVOID param)
{
    CSpyManager* pSpyManager = static_cast<CSpyManager*>(param);
    while (!pSpyManager->m_bExitLoopThreadSignal)
    {
        CSpyManager* pSpyManager = static_cast<CSpyManager*>(param);
        SharePtr<SSocketContext> pContext;
        size_t uIndex = 0;
        while (pSpyManager->GetSocket(pContext, uIndex++))
        {
            if (pContext->GetBitFlag(eSCFB_CanRead))
            {
                EnterCriticalSection(&pContext->m_receiveSection);
                size_t uRestSize = 0;
                while (pContext->GetBitFlag(eSCFB_CanRead))
                {
                    size_t uReadPos = pContext->m_pReceiveBuffer->GetReadPos();
                    SSpyTCPMessageHeader header;
                    *pContext->m_pReceiveBuffer >> header;
                    switch (header.m_type)
                    {
                    case eSMT_Command:
                        {
                            CSpyCommandManager::GetInstance()->ExecuteCommand(pContext, *pContext->m_pReceiveBuffer);
                        }
                        break;
                    case eSMT_FileData:
                        {
                            //TODO.
                        }
                        break;
                    }
                    size_t uCurReadPos = pContext->m_pReceiveBuffer->GetReadPos();
                    size_t uReadDataSize = uCurReadPos - uReadPos;
                    bool bReadMessageValid = header.m_size == uReadDataSize;
                    BEATS_ASSERT(bReadMessageValid, _T("Analysis message failed! type: %d, size: %d read size: %d"), header.m_type, header.m_size, uReadDataSize);
                    if (!bReadMessageValid)
                    {
                        pContext->m_pReceiveBuffer->SetReadPos(uReadPos + header.m_size);
                    }
                    uRestSize = pContext->m_pReceiveBuffer->GetWritePos() - uCurReadPos;
                    SSpyTCPMessageHeader* pHeader = NULL;
                    if (uRestSize > sizeof(SSpyTCPMessageHeader))
                    {
                        pHeader = (SSpyTCPMessageHeader*)pContext->m_pReceiveBuffer->GetReadPtr();
                    }
                    pContext->SetBitFlag(eSCFB_CanRead, pHeader && uRestSize >= pHeader->m_size);
                }
                LeaveCriticalSection(&pContext->m_receiveSection);
            }
        }
        Sleep(1);
    }
    return 0;
}

DWORD CSpyManager::NetworkReceiveThreadFunc(LPVOID param)
{
    CSpyManager* pSpyManager = static_cast<CSpyManager*>(param);
    while (!pSpyManager->m_bExitRecvThreadSignal)
    {
        fd_set readSetRet = pSpyManager->m_readSet;
        fd_set exceptionSetRet = pSpyManager->m_exceptionSet;
        struct timeval timeout = {0,100};
        int iReadyCount = ::select(pSpyManager->m_iMaxFD + 1, &readSetRet, NULL, &exceptionSetRet, &timeout);
        if (iReadyCount != SOCKET_ERROR)
        {
            if (iReadyCount > 0 && FD_ISSET(pSpyManager->m_listenSocket, &readSetRet))
            {
                SOCKET newSocket;
                if (Beats_Accept(pSpyManager->m_listenSocket, newSocket, NULL, NULL))
                {
                    SharePtr<SSocketContext> pContext = new SSocketContext;
                    pContext->m_socket = newSocket;
                    pSpyManager->RegisterSocket(pContext);
                }
                --iReadyCount;
            }
            if (iReadyCount > 0)
            {
                EnterCriticalSection(&pSpyManager->m_socketContextSection);
                SharePtr<SSocketContext> pSocketContext;
                size_t uIndex = 0;
                while (iReadyCount > 0 && pSpyManager->GetSocket(pSocketContext, uIndex++))
                {
                    SOCKET curSocket = pSocketContext->m_socket;
                    if (FD_ISSET(curSocket, &readSetRet))
                    {
                        EnterCriticalSection(&pSocketContext->m_receiveSection);
                        CSerializer* pReceiveBuffer = pSocketContext->m_pReceiveBuffer;
                        size_t uCurrentWritePos = pReceiveBuffer->GetWritePos();
                        size_t uRestSize = pReceiveBuffer->GetBufferSize() - uCurrentWritePos;
                        if (uRestSize == 0)
                        {
                            pReceiveBuffer->IncreaseBufferSize();
                            uRestSize = pReceiveBuffer->GetBufferSize() - uCurrentWritePos;
                        }
                        void* pWriteBuffer = pReceiveBuffer->GetWritePtr();
                        if (Beats_Receive(curSocket, pWriteBuffer, uRestSize) && uRestSize > 0)
                        {
                            uCurrentWritePos += uRestSize;
                            pReceiveBuffer->SetWritePos(uCurrentWritePos);
                            size_t uCurrentReadPos = pReceiveBuffer->GetReadPos();
                            size_t unReadDataSize = uCurrentWritePos - uCurrentReadPos;
                            if (!pSocketContext->GetBitFlag(eSCFB_CanRead) && unReadDataSize >= sizeof(SSpyTCPMessageHeader))
                            {
                                SSpyTCPMessageHeader* pHeader = (SSpyTCPMessageHeader*)(pReceiveBuffer->GetReadPtr());
                                pSocketContext->SetBitFlag(eSCFB_CanRead, unReadDataSize >= pHeader->m_size);
                            }

                            // Arrange memory when the receive buffer is going to be used out.
                            if (pReceiveBuffer->GetBufferSize() - pReceiveBuffer->GetWritePos() < 256)
                            {
                                memcpy((void*)pReceiveBuffer->GetBuffer(), pReceiveBuffer->GetReadPtr(), unReadDataSize);
                                pReceiveBuffer->Reset();
                                pReceiveBuffer->SetWritePos(unReadDataSize);
                            }
                        }
                        else
                        {
                            pSpyManager->UnregisterSocket(pSocketContext);
                        }
                        LeaveCriticalSection(&pSocketContext->m_receiveSection);
                        --iReadyCount;
                    }
                    else if (FD_ISSET(curSocket, &exceptionSetRet))
                    {
                        --iReadyCount;
                    }
                }
                LeaveCriticalSection(&pSpyManager->m_socketContextSection);
            }
        }
    }
    return 0;
}

DWORD CSpyManager::NetworkSendThreadFunc(LPVOID param)
{
    CSpyManager* pSpyManager = static_cast<CSpyManager*>(param);
    while (!pSpyManager->m_bExitSendThreadSignal)
    {
        SharePtr<SSocketContext> pSocketContext;
        size_t uIndex = 0;
        while (pSpyManager->GetSocket(pSocketContext, uIndex++))
        {
            if(pSocketContext->GetBitFlag(eSCFB_CanSend))
            {
                EnterCriticalSection(&pSocketContext->m_sendSection);
                CSerializer* pSendBuffer = pSocketContext->m_pSendBuffer;
                void* pData = pSendBuffer->GetReadPtr();
                size_t uReadPos = pSendBuffer->GetReadPos();
                size_t uWritePos = pSendBuffer->GetWritePos();
                size_t uDataSize = uWritePos - uReadPos;
                BEATS_ASSERT(uDataSize > 0);
                if (Beats_Send(pSocketContext->m_socket, pData, uDataSize) && uDataSize > 0)
                {
                    size_t uNewReadPos = uReadPos + uDataSize;
                    pSendBuffer->SetReadPos(uNewReadPos);
                    size_t uRestDataSize = uWritePos - uNewReadPos;
                    pSocketContext->SetBitFlag(eSCFB_CanSend, uRestDataSize > 0);
                    // Arrange memory when the send buffer is going to be used out.
                    if (pSendBuffer->GetBufferSize() - uWritePos < 256)
                    {
                        memcpy((void*)pSendBuffer->GetBuffer(), pSendBuffer->GetReadPtr(), uRestDataSize);
                        pSendBuffer->Reset();
                        pSendBuffer->SetWritePos(uRestDataSize);
                    }
                }
                else
                {
                    pSpyManager->UnregisterSocket(pSocketContext);
                }
                LeaveCriticalSection(&pSocketContext->m_sendSection);
            }
        }
        Sleep(1);
    }
    return 0;
}