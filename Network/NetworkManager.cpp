#include "stdafx.h"
#include "NetworkManager.h"
#include <Iphlpapi.h>

CNetworkManager* CNetworkManager::m_pInstance = NULL;

static const BYTE VersionHigh = 2;
static const BYTE VersionLow = 2;
static const size_t MaxResendDataTimes = 5;

CNetworkManager::CNetworkManager()
: m_bInitialized(false)
{
    InitializeCriticalSection(&m_socketMapSection);
    Initialize();
}

CNetworkManager::~CNetworkManager()
{
    std::map<SOCKET, SBeatsSocket*>::iterator iter = m_socketMap.begin();
    while (iter != m_socketMap.end())
    {
        DestroySocket(iter->first);
        iter = m_socketMap.begin();
    }
    DeleteCriticalSection(&m_socketMapSection);
    UnInitialize();
}

bool CNetworkManager::Initialize()
{
    if (!m_bInitialized)
    {
        // Initialize Win sock.
        WSADATA wsaData;
        int nResult = WSAStartup(MAKEWORD(VersionLow, VersionHigh), &wsaData);
        BEATS_ASSERT(nResult == NO_ERROR, _T("SAStartup failed! Error code = %d."), nResult)
        if (nResult == NO_ERROR)
        {
            bool bCheckVersion = LOBYTE( wsaData.wVersion ) == VersionLow && HIBYTE( wsaData.wVersion ) == VersionHigh;
            BEATS_ASSERT(bCheckVersion, _T("Current WinSock dll verion is tool old! we can't set it up."));
            if ( bCheckVersion )
            {
                m_bInitialized = true;
            }
            else
            {
                WSACleanup();
            }
        }
    }
    return TRUE;
}

bool CNetworkManager::UnInitialize()
{
    BEATS_ASSERT(m_bInitialized, _T("Can't Unintialize network manager before it is initialized!"));
    if(m_bInitialized)
    {
        int nResult = WSACleanup();
        BEATS_ASSERT(nResult == NO_ERROR, _T("WSACleanup dsfailed! Error code = %d.\n"), nResult);
        m_bInitialized = nResult == NO_ERROR;
    }
    return m_bInitialized;
}

bool CNetworkManager::IsInitialized()
{
    return m_bInitialized;
}

SOCKET CNetworkManager::CreateSocket(int type, int protocol)
{
    SOCKET newSocket = socket(AF_INET, type, protocol);
    bool bRet = newSocket != INVALID_SOCKET;
    BEATS_ASSERT(bRet, _T("Create socket failed! Error code = %d.\n"), ::WSAGetLastError());
    if (bRet)
    {
        RegisterSocket(newSocket);
    }
    return newSocket;
}

bool CNetworkManager::DestroySocket(SOCKET socket)
{
    BEATS_ASSERT(socket != INVALID_SOCKET, _T("Can't destroy invalid socket!"));
    shutdown(socket, SD_BOTH);
    bool bRet = closesocket(socket) != SOCKET_ERROR;
    BEATS_ASSERT(bRet, _T("Close socket %d failed! Error code = %d.\n"), socket, ::WSAGetLastError());
    if(bRet)
    {
        UnregisterSocket(socket);
    }
    return bRet;
}

bool CNetworkManager::Bind(SOCKET socketToBind, const sockaddr_in* pAddr, int iAddrSize, bool bAvoidConflictPort/* = false */, size_t uTryRebindCount/* = 50*/)
{
    bool bRet = false;
    bool bParameterValid = socketToBind != INVALID_SOCKET && pAddr != NULL && iAddrSize > 0;
    BEATS_ASSERT(bParameterValid);
    if (bParameterValid)
    {
        bRet = ::bind(socketToBind, (sockaddr*)pAddr, iAddrSize) != SOCKET_ERROR;
        if (bRet == false && WSAGetLastError() == WSAEADDRINUSE)
        {
            sockaddr_in addrTmp = *pAddr;
            while (bAvoidConflictPort && uTryRebindCount > 0 && !bRet)
            {
                addrTmp.sin_port++;
                bRet = Bind(socketToBind, &addrTmp, iAddrSize);
                --uTryRebindCount;
            }
        }
    }
    return bRet;
}

bool CNetworkManager::Listen(SOCKET socketToListen, int iBackLog)
{
    bool bRet = SOCKET_ERROR != ::listen(socketToListen, iBackLog);
    BEATS_ASSERT(bRet, _T("Listen a socket failed, Error code is %d"), WSAGetLastError());
    return bRet;
}

bool CNetworkManager::Accept(SOCKET listenSocket, SOCKET& newSocket, sockaddr_in* pAddr, int* pAddrLen)
{
    newSocket = ::accept(listenSocket, (sockaddr*)pAddr, pAddrLen);
    bool bRet = newSocket != INVALID_SOCKET;
    if (bRet)
    {
        RegisterSocket(newSocket);
    }
    BEATS_ASSERT(bRet, _T("Accept a socket failed, Error code is %d"), WSAGetLastError());
    return bRet;
}

bool CNetworkManager::Connect(SOCKET connectSocket, const sockaddr_in* pAddress, int iAddrLen)
{
    bool bExamParameter = connectSocket != INVALID_SOCKET && pAddress != NULL && iAddrLen > 0;
    BEATS_ASSERT(bExamParameter, _T("Parameter Is Not Valid"));
    bool bRet = ::connect(connectSocket, (const sockaddr*)pAddress, iAddrLen) != SOCKET_ERROR;
    BEATS_ASSERT(bRet, _T("Connect socket failed! Error code = %d."), ::WSAGetLastError());
    return bRet;
}

bool CNetworkManager::SendTo(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uIP, size_t uPort, size_t uRetryCount /*= 0*/)
{
    bool bRet = false;
    std::map<SOCKET, SBeatsSocket*>::iterator iter = m_socketMap.find(sendingSocket);
    bool bExamParameter = sendingSocket != INVALID_SOCKET && pData != NULL && uDataLength > 0;
    BEATS_ASSERT(bExamParameter);
    if (bExamParameter && uRetryCount < MaxResendDataTimes && iter != m_socketMap.end())
    {
        static sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_addr.S_un.S_addr = (u_long)uIP;
        saddr.sin_port = htons((u_short)uPort);

        int iMaxMessageSize = 0;
        int iValueLength = sizeof(iMaxMessageSize);
        getsockopt(sendingSocket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&iMaxMessageSize, &iValueLength);
        BEATS_ASSERT(iValueLength == 4);
        bool bSizeValid = uDataLength < (size_t)iMaxMessageSize;
        BEATS_ASSERT(bSizeValid, _T("Data is too long for sendto, datasize: %d, max message size %d"), uDataLength, iMaxMessageSize);
        if (bSizeValid)
        {
            int iSendCount = ::sendto(sendingSocket, (const char*)pData, (int)uDataLength, 0, (sockaddr*)&saddr, sizeof(saddr));
            if(iSendCount != SOCKET_ERROR)
            {
                iter->second->m_uSendDataCount += iSendCount;
                uDataLength = iSendCount;
                bRet = true;
            }
            else
            {
                int iErrorCode = ::WSAGetLastError();
                switch (iErrorCode)
                {
                case SO_MAX_MSG_SIZE:
                    BEATS_ASSERT(false, _T("Impossible to get this case, because we have deal with the situation about data is too long."));
                    break;
                case WSAENOBUFS:
                    Sleep(1);
                    bRet = SendTo(sendingSocket, pData, uDataLength, uIP, uPort, ++uRetryCount);
                    break;
                case WSAECONNRESET:
                default:
                    BEATS_ASSERT(false, _T("Send data by UDP failed! Data length %d, IP %d port %d, Error code %d"), uDataLength, uIP, uPort, ::WSAGetLastError());
                    break;
                }
            }
        }
    }
    return bRet;
}

bool CNetworkManager::RecvFrom( SOCKET receiveSocket, void* pData, size_t& uDataLength, sockaddr_in* pReceiveInfo /*= 0*/, int* pInfoSize /*= 0*/)
{
    bool bRet = false;
    bool bExamParameter = receiveSocket != INVALID_SOCKET && pData != NULL && uDataLength > 0;
    std::map<SOCKET, SBeatsSocket*>::iterator iter = m_socketMap.find(receiveSocket);

    if (bExamParameter && iter != m_socketMap.end())
    {
        int iReceivedDataCount = ::recvfrom(receiveSocket, (char*)pData, (int)uDataLength, 0, (sockaddr*)pReceiveInfo, pInfoSize);
        if(iReceivedDataCount != SOCKET_ERROR)
        {
            iter->second->m_uReceiveDataCount += iReceivedDataCount;
            uDataLength = (size_t)iReceivedDataCount;
            bRet = true;
        }
        else
        {
            INT nLastError = ::WSAGetLastError();
            // This kind of error is legal, it means low level is doing receiving operation.
            BEATS_ASSERT(nLastError == WSAEWOULDBLOCK, _T("ReceiveUDPData failed! Received = %d, Error code = %d.\n"), iReceivedDataCount, nLastError);
        }
    }
    return bRet;
}

bool CNetworkManager::Send(SOCKET sendingSocket, void* pData, size_t& uDataLength, size_t uFlag /*= 0*/)
{
    bool bRet = sendingSocket != INVALID_SOCKET && pData != NULL && uDataLength > 0;
    BEATS_ASSERT(bRet, _T("Parameter is invalid in send! Socket %d Data Address 0x%x, DataLength %d Flag %d"), sendingSocket, pData, uDataLength, uFlag);
    if (bRet)
    {
        SBeatsSocket* pSocket = m_socketMap[sendingSocket];
        size_t uSendCount = ::send(sendingSocket, (const char*)pData, (int)uDataLength, (int)uFlag);

        bRet = uSendCount != SOCKET_ERROR;
        BEATS_WARNING(bRet, _T("Send data error, error id %d"), WSAGetLastError());
        BEATS_WARNING(uSendCount > 0, _T("Sending data failed with length 0! ErrorCode:%d"), GetLastError());
        if (bRet)
        {
            size_t uCurrentTime = GetTickCount();
            pSocket->m_uSendDataCountInCalcTime += uSendCount;
            size_t fDeltaTimeInMs = uCurrentTime - pSocket->m_uLastUploadSpeedCalcTime;
            if (fDeltaTimeInMs >= SBeatsSocket::SPEED_CALC_INTERVAL )
            {
                pSocket->m_uUploadSpeed = (size_t)(pSocket->m_uSendDataCountInCalcTime / (fDeltaTimeInMs * 0.001F));
                pSocket->m_uLastUploadSpeedCalcTime = uCurrentTime;
                pSocket->m_uSendDataCountInCalcTime = 0;
            }

            uDataLength = uSendCount;
            pSocket->m_uSendDataCount += uSendCount;
        }
        else
        {
            size_t uErrorCode = WSAGetLastError();
            switch (uErrorCode)
            {
            case WSAECONNRESET:
            case WSAENOBUFS:
                break;
            default:
                BEATS_ASSERT(false, _T("Send data failed! error id %d"), uErrorCode);
                break;
            }
        }
    }
    return bRet;
}

bool CNetworkManager::Receive(SOCKET receiveSocket, void* pData, size_t& uDataLength, size_t uFlag/* = 0*/)
{
    bool bRet = receiveSocket != INVALID_SOCKET && pData != NULL && uDataLength > 0;
    BEATS_ASSERT(bRet, _T("Parameter is invalid in send! Socket %d Data Address 0x%x, DataLength %d Flag %d"), receiveSocket, pData, uDataLength, uFlag);
    if (bRet)
    {
        static const size_t MAX_RECEIVE_BUFFER_SIZE = 1024 * 512; // 512K buffer to receive.
        uDataLength = min(uDataLength, MAX_RECEIVE_BUFFER_SIZE);
        SBeatsSocket* pSocket = m_socketMap[receiveSocket];
        int iReceivedCount = ::recv(receiveSocket, (char*)pData, (int)uDataLength, (int)uFlag);
        size_t uCurrentTime = GetTickCount();
        bRet = iReceivedCount != SOCKET_ERROR;
        if (bRet)
        {
            pSocket->m_uReceiveDataCountInCalcTime += iReceivedCount;
            size_t fDeltaTimeInMs = uCurrentTime - pSocket->m_uLastDownloadSpeedCalcTime;
            if (fDeltaTimeInMs >= SBeatsSocket::SPEED_CALC_INTERVAL )
            {
                pSocket->m_uDownloadSpeed = (size_t)(pSocket->m_uReceiveDataCountInCalcTime / (fDeltaTimeInMs * 0.001F));
                pSocket->m_uLastDownloadSpeedCalcTime = uCurrentTime;
                pSocket->m_uReceiveDataCountInCalcTime = 0;
            }

            uDataLength = iReceivedCount;
            pSocket->m_uReceiveDataCount += iReceivedCount;
        }
        else
        {
            size_t uErrorCode = WSAGetLastError();
            switch (uErrorCode)
            {
                case WSAECONNRESET:
                case WSAENOBUFS:
                    break;
                default:
                    BEATS_WARNING(false, _T("Receive data failed! error id %d"), uErrorCode);
                    break;
            }
        }
    }
    return bRet;
}

const SBeatsSocket* CNetworkManager::GetSocketInfo(SOCKET socket)
{
    SBeatsSocket* pInfo = NULL;
    std::map<SOCKET, SBeatsSocket*>::iterator iter = m_socketMap.find(socket);
    if (iter != m_socketMap.end())
    {
        pInfo = iter->second;
    }
    return pInfo;
}

const hostent* CNetworkManager::GetHostInfo()
{
    char szHostName[MAX_PATH];
    szHostName[0] = 0;
    ::gethostname(szHostName, MAX_PATH);
    return ::gethostbyname(szHostName);
}

PIP_ADAPTER_INFO CNetworkManager::CreateNetAdapterInfo()
{
    PIP_ADAPTER_INFO pRet = NULL;
    char tempChar;
    ULONG uListSize = 1;
    DWORD dwRet = GetAdaptersInfo((PIP_ADAPTER_INFO)&tempChar, &uListSize);
    if (dwRet == ERROR_BUFFER_OVERFLOW)
    {
        PIP_ADAPTER_INFO pAdapterListBuffer = (PIP_ADAPTER_INFO)new char[uListSize];
        dwRet = GetAdaptersInfo(pAdapterListBuffer, &uListSize);
        if (dwRet == NO_ERROR)
        {
            pRet = (PIP_ADAPTER_INFO)pAdapterListBuffer;
        }
        else
        {
            BEATS_ASSERT(false, _T("Call to GetAdaptersAddresses failed."));
            LPVOID lpMsgBuf;
            if (FormatMessage( 
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dwRet,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) &lpMsgBuf,
                0,
                NULL )) 
            {
                BEATS_ASSERT(false, (TCHAR*)lpMsgBuf);
            }
            LocalFree( lpMsgBuf );
        }
    }
    return pRet;
}


bool CNetworkManager::RegisterSocket(SOCKET socketToRegister)
{
    EnterCriticalSection(&m_socketMapSection);
    bool bIsExisting = m_socketMap.find(socketToRegister) != m_socketMap.end();
    BEATS_ASSERT(!bIsExisting, _T("Socket %d already exists, Can't create a same one."), socketToRegister);
    if (!bIsExisting)
    {
        SBeatsSocket* pSocketData = new SBeatsSocket;
        pSocketData->m_socket = socketToRegister;
        m_socketMap[socketToRegister] = pSocketData;
    }
    LeaveCriticalSection(&m_socketMapSection);

    return !bIsExisting;
}

bool CNetworkManager::UnregisterSocket(SOCKET socketToUnregister)
{
    EnterCriticalSection(&m_socketMapSection);
    std::map<SOCKET, SBeatsSocket*>::iterator iter = m_socketMap.find(socketToUnregister);
    bool bIsExisting = iter != m_socketMap.end();
    BEATS_ASSERT(bIsExisting, _T("Socket already exists, Can't create a same one."));
    if (bIsExisting)
    {
        BEATS_SAFE_DELETE(iter->second);
        m_socketMap.erase(iter);
    }
    LeaveCriticalSection(&m_socketMapSection);

    return bIsExisting;
}
