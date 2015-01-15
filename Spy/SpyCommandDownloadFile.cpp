#include "stdafx.h"
#include "SpyCommandDownloadFile.h"
#include "SpyCommandMessageBox.h"
#include "SpyCommandManager.h"
#include <process.h>
#include <string>
#include "../Network/Network.h"
#include "ServerDownloadMissionContext.h"
#include "SpyManager.h"
#include "../Utility/FileFilter/FileFilter.h"
#include "../Utility/FilePath/FilePathTool.h"

static const uint32_t WORK_THREAD_COUNT = 5;
static const uint32_t WORK_SEND_PIECE_SIZE = 1024 * 1024; // 1M, each send call will send 1M data for most.

CSpyCommandDownloadFile::CSpyCommandDownloadFile()
: super(eSCT_Download)
, m_bUploadMode(false)
, m_bHasFilterData(false)
, m_pFileFilter(NULL)
, m_step(eDMS_ClientRequest)
, m_pSvrDownloadMissionContext(NULL)
, m_pClientDownloadMissionContext(NULL)
{

}

CSpyCommandDownloadFile::~CSpyCommandDownloadFile()
{
}

bool CSpyCommandDownloadFile::ExecuteImpl( SharePtr<SSocketContext>& pSocketContext )
{
    bool bRet = false;
    switch (m_step)
    {
    case eDMS_ClientRequest:
        {
            std::map<TString, SDirectory*> lookUpMap;
            unsigned long long uAllFilesSize = 0;
            for (uint32_t i = 0; i < m_downloadFiles.size(); ++i)
            {
                WIN32_FILE_ATTRIBUTE_DATA fileData;
                bool bFileExists = GetFileAttributesEx(m_downloadFiles[i].c_str(), GetFileExInfoStandard, &fileData) == TRUE;
                TString strFileName = CFilePathTool::GetInstance()->FileName(m_downloadFiles[i].c_str());
                TString strFilePath = CFilePathTool::GetInstance()->ParentPath(m_downloadFiles[i].c_str());
                if (bFileExists)
                {
                    SDirectory* pFileList = NULL;
                    if ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    {
                        std::map<TString, SDirectory*>::iterator iter = lookUpMap.find(strFilePath);
                        if (iter == lookUpMap.end())
                        {
                            pFileList = new SDirectory(NULL, strFilePath.c_str());
                            ZeroMemory(&pFileList->m_data, sizeof(pFileList->m_data));
                            lookUpMap[strFilePath] = pFileList;
                        }
                        else
                        {
                            pFileList = iter->second;
                        }

                        WIN32_FIND_DATA* pFindFileData = new WIN32_FIND_DATA;
                        _tcscpy(pFindFileData->cAlternateFileName, _T(""));
                        _tcscpy(pFindFileData->cFileName, strFileName.c_str());
                        pFindFileData->dwFileAttributes = fileData.dwFileAttributes;
                        pFindFileData->ftCreationTime = fileData.ftCreationTime;
                        pFindFileData->ftLastWriteTime = fileData.ftLastWriteTime;
                        pFindFileData->ftLastAccessTime = fileData.ftLastAccessTime;
                        pFindFileData->nFileSizeHigh = fileData.nFileSizeHigh;
                        pFindFileData->nFileSizeLow = fileData.nFileSizeLow;
                        pFindFileData->dwReserved0 = 0;
                        pFindFileData->dwReserved1 = (DWORD)pFileList;
                        pFileList->m_pFileList->push_back(pFindFileData);

                        unsigned long long uFileSize = fileData.nFileSizeHigh;
                        uFileSize = uFileSize << 32;
                        uFileSize += fileData.nFileSizeLow;
                        uAllFilesSize += uFileSize;
                    }
                    else
                    {
                        if (*m_downloadFiles[i].rbegin() != _T('/'))
                        {
                            m_downloadFiles[i].append(_T("/"));
                        }
                        std::map<TString, SDirectory*>::iterator iter = lookUpMap.find(m_downloadFiles[i]);
                        if (iter == lookUpMap.end())
                        {
                            pFileList = new SDirectory(NULL, m_downloadFiles[i].c_str());
                            pFileList->m_data.ftCreationTime = fileData.ftCreationTime;
                            pFileList->m_data.ftLastAccessTime = fileData.ftLastAccessTime;
                            pFileList->m_data.ftLastWriteTime = fileData.ftLastWriteTime;
                            pFileList->m_data.dwFileAttributes = fileData.dwFileAttributes;
                            //NOTICE: I don't know the meaning of dirctory's size. it's not right!
                            pFileList->m_data.nFileSizeHigh = fileData.nFileSizeHigh;
                            pFileList->m_data.nFileSizeLow = fileData.nFileSizeLow;
                            _tcscpy(pFileList->m_data.cFileName, strFileName.c_str());
                            pFileList->m_data.dwReserved0 = 0;
                            pFileList->m_data.dwReserved1 = (DWORD)pFileList;
                            lookUpMap[m_downloadFiles[i]] = pFileList;
                        }
                        else
                        {
                            pFileList = iter->second;
                        }
                        CUtilityManager::GetInstance()->FillDirectory(*pFileList, true, m_pFileFilter, &uAllFilesSize);
                        if (m_bHasFilterData)
                        {
                            BEATS_SAFE_DELETE(m_pFileFilter);
                        }
                    }
                }
                else
                {
                    if (!m_bUploadMode)
                    {
                        CSpyCommandMessageBox msgCmd;
                        msgCmd.SetCaption(_T("Error"));
                        msgCmd.SetMessage(_T("下载文件失败。"));
                        CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, &msgCmd);
                    }
                }
            }
            std::vector<SDirectory*>* pDownloadFiles = new std::vector<SDirectory*>;
            for (std::map<TString, SDirectory*>::iterator iter = lookUpMap.begin(); iter != lookUpMap.end(); ++iter)
            {
                pDownloadFiles->push_back(iter->second);
            }
            CServerDownloadMissionContext* pDownloadMissionContext = new CServerDownloadMissionContext(pSocketContext, pDownloadFiles, this->m_bUploadMode, eDR_PieceFirst, m_storePath);
            pDownloadMissionContext->SetTotalSize(uAllFilesSize);
            uint32_t uDownloadMissionThreadId;
            _beginthreadex(NULL, 0, (unsigned(_stdcall*)(void*))SvrDownloadMissionThreadFunc, pDownloadMissionContext, 0, &uDownloadMissionThreadId);
        }
        break;
    case eDMS_ServerFeedBack:
        {
            m_pClientDownloadMissionContext->m_pSocketContext = pSocketContext;
            m_pClientDownloadMissionContext->m_bUploadMode = this->m_bUploadMode;
            uint32_t uClientDownloadMissionThreadId;
            _beginthreadex(NULL, 0, (unsigned(_stdcall*)(void*))ClientDownloadMissionThreadFunc, m_pClientDownloadMissionContext, 0, &uClientDownloadMissionThreadId);
        }
        break;
    case eDMS_ServerFinished:
        {
            if (!m_bUploadMode)
            {
                MessageBox(NULL, _T("下载完成"), _T("Finished"), MB_OK);
            }
        }
        break;
    }

    return bRet;
}

void CSpyCommandDownloadFile::Deserialize( CSerializer& serializer )
{
    super::Deserialize(serializer);
    serializer << m_step;
    serializer << m_bUploadMode;
    switch (m_step)
    {
    case eDMS_ClientRequest:
        {
            BEATS_ASSERT(m_downloadFiles.size() > 0 && m_storePath.length() > 0, _T("Length of file path must not be empty!"));
            bool bIsDirectory = CFilePathTool::GetInstance()->IsDirectory(m_storePath.c_str());
            BEATS_ASSERT(bIsDirectory, _T("%s is not a valid path to store files!"), m_storePath.c_str());
            bool bExamSuccess = bIsDirectory && m_downloadFiles.size() > 0;
            serializer << bExamSuccess;
            if (bExamSuccess)
            {
                serializer << m_downloadFiles.size();
                for (uint32_t i = 0; i< m_downloadFiles.size(); ++i)
                {
                    serializer << m_downloadFiles[i];
                }
                serializer << m_storePath;
                serializer << m_bHasFilterData;
                if (m_bHasFilterData)
                {
                    m_pFileFilter->Deserialize(serializer);
                }
            }
        }
        break;
    case eDMS_ServerFeedBack:
        {
            BEATS_ASSERT(m_pSvrDownloadMissionContext != NULL);
            // Get the random port of this download mission.
            sockaddr_in sockAddr;
            int iAddrSize = sizeof(sockAddr);
            bool bGetSockInfo = getsockname(m_pSvrDownloadMissionContext->GetListenSocket(), (sockaddr*)&sockAddr, &iAddrSize) != SOCKET_ERROR;
            BEATS_ASSERT(bGetSockInfo);
            serializer << m_pSvrDownloadMissionContext->GetStorePath();
            serializer << sockAddr.sin_port;
            serializer << m_pSvrDownloadMissionContext->GetDownloadFiles()->size();
            for (uint32_t i = 0; i < m_pSvrDownloadMissionContext->GetDownloadFiles()->size(); ++i)
            {
                CUtilityManager::GetInstance()->SerializeDirectory(m_pSvrDownloadMissionContext->GetDownloadFiles()->at(i), serializer);
            }
        }
        break;
    case eDMS_ServerFinished:
        {

        }
        break;
    }
}

void CSpyCommandDownloadFile::Serialize( CSerializer& serializer )
{
    super::Serialize(serializer);

    serializer >> m_step;
    serializer >> m_bUploadMode;

    switch (m_step)
    {
    case eDMS_ClientRequest:
        {
            m_downloadFiles.clear();
            m_storePath.clear();
            bool bExamSuccess = false;
            serializer >> bExamSuccess;
            if (bExamSuccess)
            {
                uint32_t uFilesCount = 0;
                serializer >> uFilesCount;
                m_downloadFiles.resize(uFilesCount);
                for (uint32_t i = 0; i < uFilesCount; ++i)
                {
                    serializer >> m_downloadFiles[i];
                }
                serializer >> m_storePath;
                serializer >> m_bHasFilterData;
                if (m_bHasFilterData)
                {
                    m_pFileFilter = new CFileFilter;
                    m_pFileFilter->Serialize(serializer);
                }
                BEATS_ASSERT(m_downloadFiles.size() > 0 && m_storePath.length() > 0, _T("Length of file path must not be empty!"));
            }
        }
        break;
    case eDMS_ServerFeedBack:
        {
            m_pClientDownloadMissionContext = new SClientDownloadMissionContext;
            serializer >> m_pClientDownloadMissionContext->m_saveRootPath;
            serializer >> m_pClientDownloadMissionContext->m_pSvrListenPort;
            uint32_t uDirectoriesCount = 0;
            serializer >> uDirectoriesCount;
            for (uint32_t i = 0; i < uDirectoriesCount; ++i)
            {
                SDirectory* pFilesToDownload = new SDirectory(NULL, NULL);
                CUtilityManager::GetInstance()->DeserializeDirectory(pFilesToDownload, serializer, &m_pClientDownloadMissionContext->m_uTotalFileSize, &m_pClientDownloadMissionContext->m_uTotalFileCount);
                // Make a copy of the root path in the server, cause we will change the pFilesToDownload->m_szPath in BuildTaskList
                TCHAR szRootPath[MAX_PATH];
                _tcscpy(szRootPath, pFilesToDownload->m_szPath.c_str());
                m_pClientDownloadMissionContext->BuildTaskList(pFilesToDownload, szRootPath);
            }
        }
        break;
    case eDMS_ServerFinished:
        {

        }
        break;
    }

}

void CSpyCommandDownloadFile::SetDownloadFiles(const std::vector<TString>& downloadFiles)
{
    m_downloadFiles = downloadFiles;
}

void CSpyCommandDownloadFile::SetStorePath(const TCHAR* pszStoreFilename)
{
    m_storePath = pszStoreFilename;
}

void CSpyCommandDownloadFile::SetMissionStep(EDownloadMissionStep step)
{
    m_step = step;
}

void CSpyCommandDownloadFile::SetSvrDownloadMissionContext(CServerDownloadMissionContext* pContext)
{
    m_pSvrDownloadMissionContext = pContext;
}

void CSpyCommandDownloadFile::SetUploadMode(bool bUpload)
{
    m_bUploadMode = bUpload;
}

void CSpyCommandDownloadFile::SetFileFilter(CFileFilter* pFileFilter)
{
    m_pFileFilter = pFileFilter;
    m_bHasFilterData = m_pFileFilter != NULL;
}

DWORD CSpyCommandDownloadFile::SvrDownloadMissionThreadFunc( LPVOID param )
{
    CServerDownloadMissionContext* pSvrDownloadMissionContext = (CServerDownloadMissionContext*) param;
    SOCKET listenSocket = Beats_CreateSocket(SOCK_STREAM, 0);
    pSvrDownloadMissionContext->SetListenSocket(listenSocket);
    if (listenSocket != INVALID_SOCKET)
    {
        sockaddr_in sockAddr;
        int iAddrSize = sizeof(sockAddr);
        getsockname(pSvrDownloadMissionContext->GetSocketContext()->m_socket, (sockaddr*)&sockAddr, &iAddrSize);
        sockAddr.sin_port = 0;
        if (Beats_Bind(listenSocket, &sockAddr, iAddrSize) &&
            Beats_Listen(listenSocket, SOMAXCONN))
        {
            CSpyCommandDownloadFile svrFeedBackCmd;
            svrFeedBackCmd.SetMissionStep(eDMS_ServerFeedBack);
            svrFeedBackCmd.SetSvrDownloadMissionContext(pSvrDownloadMissionContext);
            svrFeedBackCmd.SetUploadMode(pSvrDownloadMissionContext->IsUploadMode());
            CSpyCommandManager::GetInstance()->SendCommand(pSvrDownloadMissionContext->GetSocketContext(), &svrFeedBackCmd);

            uint32_t uThreadCount = 0;
            HANDLE threadHandle[WORK_THREAD_COUNT];
            uint32_t uSelectCount = 0;
            // Waiting for connection
            while (true)
            {
                SOCKET listenSocket = pSvrDownloadMissionContext->GetListenSocket();
                fd_set listenSet;
                FD_ZERO(&listenSet);
                FD_SET(listenSocket, &listenSet);
                int iReadCount = ::select((int)listenSocket + 1, &listenSet, NULL, NULL, NULL);
                while (iReadCount > 0 && uThreadCount < WORK_THREAD_COUNT)
                {
                    uint32_t uSvrDownloadWorkThreadId;
                    threadHandle[uThreadCount++] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(_stdcall*)(void*))SvrDownloadWorkThreadFunc, pSvrDownloadMissionContext, 0, &uSvrDownloadWorkThreadId);
                    --iReadCount;
                }
                if (++uSelectCount >= WORK_THREAD_COUNT)
                {
                    break;
                }
            }
            WaitForMultipleObjectsEx((DWORD)uThreadCount, threadHandle, TRUE, INFINITE, FALSE);
            if (!pSvrDownloadMissionContext->IsUploadMode())
            {
                CSpyCommandDownloadFile svrFinishedCmd;
                svrFinishedCmd.SetMissionStep(eDMS_ServerFinished);
                CSpyCommandManager::GetInstance()->SendCommand(pSvrDownloadMissionContext->GetSocketContext(), &svrFinishedCmd);
            }
            else
            {
                MessageBox(NULL, _T("上传完毕!"), _T("Upload"), MB_OK);
            }
        }
        Beats_DestroySocket(pSvrDownloadMissionContext->GetListenSocket());
    }

    BEATS_SAFE_DELETE(pSvrDownloadMissionContext);
    return 0;
}

DWORD CSpyCommandDownloadFile::SvrDownloadWorkThreadFunc(LPVOID param)
{
    CServerDownloadMissionContext* pSvrDownloadMissionContext = (CServerDownloadMissionContext*) param;
    SOCKET newSocket;
    CSerializer fileDataHeader;

    if (Beats_Accept(pSvrDownloadMissionContext->GetListenSocket(), newSocket, NULL, NULL))
    {
        bool bBreakSignal = false;
        BYTE* pSendBuffer = new BYTE[WORK_SEND_PIECE_SIZE];
        while (!bBreakSignal)
        {
            SDownloadingTaskInfo info;
            if(pSvrDownloadMissionContext->GetTask(info))
            {
                const SDirectory* pDirectory = (SDirectory*)(info.m_pData->dwReserved1);
                TString szFilePath = pDirectory->m_szPath;
                if (szFilePath.back() != _T('\\') && szFilePath.back() != _T('/'))
                {
                    szFilePath.append(_T("/"));
                }
                szFilePath.append(info.m_pData->cFileName);
                HANDLE hFileHandle = ::CreateFile(szFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
                BEATS_WARNING(hFileHandle != INVALID_HANDLE_VALUE, _T("Create File %s failed!"), szFilePath.c_str());
                bool bSendSuccess = hFileHandle != INVALID_HANDLE_VALUE;

                unsigned long long uFileSizeInRecord = info.m_pData->nFileSizeHigh;
                uFileSizeInRecord = uFileSizeInRecord << 32;
                uFileSizeInRecord += info.m_pData->nFileSizeLow;

                unsigned long long uFileSizeForNow = uFileSizeInRecord;

                if (bSendSuccess)
                {
                    WIN32_FILE_ATTRIBUTE_DATA attrData;
                    GetFileAttributesEx(szFilePath.c_str(), GetFileExInfoStandard, &attrData);
                    uFileSizeForNow = attrData.nFileSizeHigh;
                    uFileSizeForNow = uFileSizeForNow << 32;
                    uFileSizeForNow += attrData.nFileSizeLow;

                    bool bFileSizeChanged = uFileSizeInRecord != uFileSizeForNow;
                    BEATS_ASSERT(!bFileSizeChanged, _T("File size has changed during build list (%lld) to send file data(%lld)."), uFileSizeInRecord, uFileSizeForNow)
                    // Check the data size, the data may be changed during record time and now. For safe, ignore this kind of file.
                    bSendSuccess = !bFileSizeChanged;
                }

                // 1. Send file info as a header.
                fileDataHeader.Reset();
                fileDataHeader << eSMT_FileData;
                fileDataHeader << 0;
                fileDataHeader << bSendSuccess;
                fileDataHeader << info.m_uFileIndex;
                fileDataHeader << info.m_uStartPos;
                uint32_t uSendLengthForThisTime = (uint32_t)min(CServerDownloadMissionContext::WORK_LOCK_FILE_LENGTH, uFileSizeForNow - info.m_uStartPos);
                fileDataHeader << uSendLengthForThisTime;
                uint32_t msgSize = fileDataHeader.GetWritePos();
                fileDataHeader.RewriteData(4, &msgSize, sizeof(msgSize));
                uint32_t uSendHeaderCount = 0;
                void* pHeaderData = (void*)fileDataHeader.GetBuffer();
                while (uSendHeaderCount < fileDataHeader.GetWritePos())
                {
                    uint32_t uSendCount = fileDataHeader.GetWritePos() - uSendHeaderCount;
                    if(!Beats_Send(newSocket, pHeaderData, uSendCount))
                    {
                        bBreakSignal = true;
                        break;
                    }
                    pHeaderData = (BYTE*)pHeaderData + uSendCount;
                    uSendHeaderCount += uSendCount;
                }

                // 2. If the file is ready to send, do it.
                if (bSendSuccess)
                {
                    SvrDownloadSendFileData(newSocket, hFileHandle, pSendBuffer, info.m_uStartPos, uSendLengthForThisTime, uFileSizeForNow);
                    BEATS_ASSERT(info.m_uStartPos + uSendLengthForThisTime <= uFileSizeForNow, 
                        _T("Error when sending file data, startPos %lld data length for this send %d file size %lld "), 
                        info.m_uStartPos, uSendLengthForThisTime, uFileSizeForNow);
                }

                EnterCriticalSection(&pSvrDownloadMissionContext->GetContextCriticalSection());
                unsigned long long newUploadSize = pSvrDownloadMissionContext->GetUploadSize() + uSendLengthForThisTime;
                pSvrDownloadMissionContext->SetUploadSize(newUploadSize);
#ifdef _DEBUG
                if (pSvrDownloadMissionContext->IsUploadMode())
                {
                    BEATS_ASSERT(pSvrDownloadMissionContext->GetTotalSize() != 0);
                    BEATS_PRINT(_T("Task finished File Id %d: %s file size: %lld threadId %d Progress:%f\n"), info.m_uFileIndex, info.m_pData->cFileName, 
                        uFileSizeForNow, GetCurrentThreadId(), (float)newUploadSize / pSvrDownloadMissionContext->GetTotalSize());
                }

#endif
                LeaveCriticalSection(&pSvrDownloadMissionContext->GetContextCriticalSection());

                CloseHandle(hFileHandle);
            }
            else
            {
#ifdef _DEBUG
                if (pSvrDownloadMissionContext->IsUploadMode())
                {
                    const SBeatsSocket* pSockInfo = Beats_GetSockInfo(newSocket);
                    BEATS_ASSERT(pSockInfo != NULL);
                    uint32_t uLifeTime = GetTickCount() - pSockInfo->m_uCreateTime;
                    BEATS_PRINT(_T("client upload work thread %d exit, network life time %d, send data %lld, receive data %lld, average download speed %g kb/s upload speed %g kb/s"),
                        GetCurrentThreadId(), uLifeTime, pSockInfo->m_uSendDataCount, pSockInfo->m_uReceiveDataCount, pSockInfo->m_uReceiveDataCount / (uLifeTime * 0.001) / 1024,
                        pSockInfo->m_uSendDataCount / (uLifeTime * 0.001) / 1024);
                }
#endif
                break;
            }
        }
        Beats_DestroySocket(newSocket);
        BEATS_SAFE_DELETE_ARRAY(pSendBuffer);
    }
    return 0;
}

void CSpyCommandDownloadFile::SvrDownloadSendFileData(SOCKET sock, HANDLE hFileHandle, BYTE* pBuffer, const unsigned long long& uStartPos, 
                                                      uint32_t uSendLength, const unsigned long long& uFileSize)
{
    if (uFileSize > 0)
    {
        BEATS_ASSERT(uStartPos < uFileSize, _T("Start pos (%d) should never be greater than file size (%d)!"), uStartPos, uFileSize);
        uint32_t uSendDataCounter = 0;

        LARGE_INTEGER uStartPosLargeInteger;
        uStartPosLargeInteger.QuadPart = uStartPos + uSendDataCounter;
        SetFilePointerEx(hFileHandle, uStartPosLargeInteger, NULL, FILE_BEGIN);

        bool bSendSuccess = true;
        // uSendLength is the size of data should be send. We try to read as much as uSendLength and try to send as much as uReadCountForThisTime.
        while (uSendDataCounter < uSendLength && bSendSuccess)
        {
            uint32_t uReadCountForThisTime = min(WORK_SEND_PIECE_SIZE, uSendLength - uSendDataCounter);
            ReadFile(hFileHandle, pBuffer, uReadCountForThisTime, (LPDWORD)&uReadCountForThisTime, NULL);
            BEATS_ASSERT(uReadCountForThisTime > 0, _T("Send count should never be less than 1"));
            
            uint32_t uSendCounter = 0;
            while (uSendCounter < uReadCountForThisTime && bSendSuccess)
            {
                uint32_t uSendCount = uReadCountForThisTime - uSendCounter ;
                bSendSuccess = Beats_Send(sock, pBuffer, uSendCount) && uSendCount > 0;
                BEATS_WARNING(bSendSuccess, _T("Send data failed! uSendCount = %d uSendDataCounter = %d uSendLengthForThisTime = %d."), 
                    uSendCount, uSendDataCounter, uSendLength);
                if (bSendSuccess)
                {
                    uSendCounter += uSendCount;
                    uSendDataCounter += uSendCount;
                }
            }
        }
    }
}

DWORD CSpyCommandDownloadFile::ClientDownloadMissionThreadFunc(LPVOID param)
{
    SClientDownloadMissionContext* pDownloadMissionContext = (SClientDownloadMissionContext*) param;
    HANDLE threadHandle[WORK_THREAD_COUNT];
    for (uint32_t i = 0; i < WORK_THREAD_COUNT; ++i)
    {
        uint32_t uClientDownloadWorkThreadId;
        threadHandle[i] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(_stdcall*)(void*))ClientDownloadWorkThreadFunc, pDownloadMissionContext, 0, &uClientDownloadWorkThreadId);
    }
    WaitForMultipleObjectsEx((DWORD)WORK_THREAD_COUNT, threadHandle, TRUE, INFINITE, FALSE);
    BEATS_SAFE_DELETE(pDownloadMissionContext);
    return 0;
}

DWORD CSpyCommandDownloadFile::ClientDownloadWorkThreadFunc(LPVOID param)
{
    SClientDownloadMissionContext* pDownloadMissionContext = (SClientDownloadMissionContext*) param;

    SOCKET MySocket = Beats_CreateSocket(SOCK_STREAM, 0);
    sockaddr_in listenSocketAddr;
    int nameLen = sizeof(listenSocketAddr);
    ::getpeername(pDownloadMissionContext->m_pSocketContext->m_socket, (sockaddr*)&listenSocketAddr, &nameLen);
    listenSocketAddr.sin_port = pDownloadMissionContext->m_pSvrListenPort;
    CSerializer fileRecvBuffer(1 * 1024 * 1024); // 1M
    if(Beats_Connect(MySocket, &listenSocketAddr))
    {
        uint32_t uFileIndex = 0xFFFFFFFF;
        unsigned long long uStartPos = 0;
        uint32_t uDataLength = 0;
        uint32_t uWriteCounter = 0;
        bool bTaskFinished = false;
        while (!bTaskFinished)
        {
            int maxFd = (int)MySocket;
            fd_set downloadSet;
            FD_ZERO(&downloadSet);
            FD_SET(MySocket, &downloadSet);
            int iReadCount = ::select(maxFd + 1, &downloadSet, NULL, NULL, NULL);
            if (iReadCount > 0)
            {
                uint32_t uCurrentWritePos = fileRecvBuffer.GetWritePos();
                uint32_t uRestSize = fileRecvBuffer.GetBufferSize() - uCurrentWritePos;
                if (uRestSize == 0)
                {
                    fileRecvBuffer.IncreaseBufferSize();
                    uRestSize = fileRecvBuffer.GetBufferSize() - uCurrentWritePos;
                }
                BEATS_ASSERT(uRestSize > 0);
                if (Beats_Receive(MySocket, fileRecvBuffer.GetWritePtr(), uRestSize))
                {
                    // Means the server thread is closed.
                    if (uRestSize == 0)
                    {
                        bTaskFinished = true;
                    }
                    fileRecvBuffer.SetWritePos(uCurrentWritePos + uRestSize);
                    do 
                    {
                        bool bTransferFinished = false;
                        if (uFileIndex == 0xFFFFFFFF)
                        {
                            BEATS_ASSERT(fileRecvBuffer.GetReadPos() == 0, _T("It is supposed that every time begin to process a new file, the unread data is from beginning."));
                            if (fileRecvBuffer.GetWritePos() - fileRecvBuffer.GetReadPos() > sizeof(SSpyTCPMessageHeader))
                            {
                                SSpyTCPMessageHeader* pHeader = (SSpyTCPMessageHeader*)(fileRecvBuffer.GetBuffer());
                                BEATS_ASSERT(pHeader->m_type == eSMT_FileData, _T("File Header is Invalid!"));
                                if (fileRecvBuffer.GetWritePos() >= pHeader->m_size)
                                {
                                    fileRecvBuffer.SetReadPos(sizeof(SSpyTCPMessageHeader));
                                    bool bDownloadSuccess = false;
                                    fileRecvBuffer >> bDownloadSuccess;
                                    fileRecvBuffer >> uFileIndex;
                                    fileRecvBuffer >> uStartPos;
                                    fileRecvBuffer >> uDataLength;
                                    BEATS_ASSERT(uDataLength <= CServerDownloadMissionContext::WORK_LOCK_FILE_LENGTH, _T("Invalid data length %d"), uDataLength);
                                    if (!bDownloadSuccess) // Failed to download this file
                                    {
                                        bTransferFinished = true;
#ifdef _DEBUG
                                        WIN32_FIND_DATA* pFileData = pDownloadMissionContext->m_downloadTaskList[uFileIndex];
                                        BEATS_PRINT(_T("Download File Failed: File Id %d %s start pos %lld data length %d ThreadId %d\n"), uFileIndex, pFileData->cFileName, uStartPos, uDataLength, GetCurrentThreadId());
#endif
                                        //BEATS_PRINT(_T("Receive task: File Id %d start pos %lld data length %d ThreadId %d\n"), uFileIndex, uStartPos, uDataLength, GetCurrentThreadId());
                                    }
                                }
                            }
                        }
                        else
                        {
                            uint32_t uReadableSize = fileRecvBuffer.GetWritePos() - fileRecvBuffer.GetReadPos();
                            bTransferFinished = (uWriteCounter + uReadableSize) >= uDataLength ;
                            BEATS_ASSERT(uWriteCounter <= uDataLength);
                            bool bShouldWriteToBuffer = bTransferFinished || uReadableSize > 3 * 1024 * 1024;
                            uint32_t uWriteCount = 0;
                            if (bShouldWriteToBuffer)
                            {
                                WIN32_FIND_DATA* pFileData = pDownloadMissionContext->m_downloadTaskList[uFileIndex];
                                const SDirectory* pDirectory = (SDirectory*)(pFileData->dwReserved1);
                                TString szFilePath = pDirectory->m_szPath;
                                szFilePath.append(pFileData->cFileName);
                                FILE* pFile = _tfopen(szFilePath.c_str(), _T("rb+"));
                                if (pFile == NULL)
                                {
                                    pFile = _tfopen(szFilePath.c_str(), _T("wb+"));
                                }
                                BEATS_ASSERT(pFile != NULL, _T("Open File %s Failed"), szFilePath.c_str());
                                _fseeki64(pFile, uStartPos, SEEK_SET);
                                uWriteCount = min(uDataLength - uWriteCounter, fileRecvBuffer.GetWritePos() - fileRecvBuffer.GetReadPos());
                                uWriteCount = fwrite(fileRecvBuffer.GetReadPtr(), 1, uWriteCount, pFile);
                                uWriteCounter += uWriteCount;
                                uStartPos += uWriteCount;
                                fileRecvBuffer.SetReadPos(fileRecvBuffer.GetReadPos() + uWriteCount);
                                fclose(pFile);
                            }
                        }
                        if (bTransferFinished)
                        {
                            EnterCriticalSection(&pDownloadMissionContext->m_criticalSection);
                            pDownloadMissionContext->m_uDownloadedSize += uDataLength;
                            LeaveCriticalSection(&pDownloadMissionContext->m_criticalSection);
#ifdef _DEBUG
                            WIN32_FIND_DATA* pFileData = pDownloadMissionContext->m_downloadTaskList[uFileIndex];
                            unsigned long long uFileSize = pFileData->nFileSizeHigh;
                            uFileSize = uFileSize << 32;
                            uFileSize += pFileData->nFileSizeLow;
                            BEATS_PRINT(_T("Task finished File Id %d: %s file size: %lld start pos %lld datalength %d threadId %d Progress:%f\n"), uFileIndex, pFileData->cFileName, 
                                uFileSize, uStartPos, uDataLength, GetCurrentThreadId(), (float)pDownloadMissionContext->m_uDownloadedSize / pDownloadMissionContext->m_uTotalFileSize);
#endif
                            uFileIndex = 0xFFFFFFFF;
                            uStartPos = 0;
                            uDataLength = 0;
                            uWriteCounter = 0;

                            uint32_t uRestFileDataLength = fileRecvBuffer.GetWritePos() - fileRecvBuffer.GetReadPos();
                            memmove((void*)fileRecvBuffer.GetBuffer(), fileRecvBuffer.GetReadPtr(), uRestFileDataLength);
                            fileRecvBuffer.SetReadPos(0);
                            fileRecvBuffer.SetWritePos(uRestFileDataLength);
                        }
                    } while (bTaskFinished && fileRecvBuffer.GetReadPtr() != fileRecvBuffer.GetWritePtr()); // If the socket is closed, flush the receive buffer to file.
                }
                else
                    break;
            }
        }
    }
#ifdef _DEBUG
    const SBeatsSocket* pSockInfo = Beats_GetSockInfo(MySocket);
    BEATS_ASSERT(pSockInfo != NULL);
    uint32_t uLifeTime = GetTickCount() - pSockInfo->m_uCreateTime;
    BEATS_PRINT(_T("Client download work thread %d exit, network life time %d, send data %lld, receive data %lld, average download speed %g kb/s upload speed %g kb/s"),
        GetCurrentThreadId(), uLifeTime, pSockInfo->m_uSendDataCount, pSockInfo->m_uReceiveDataCount, pSockInfo->m_uReceiveDataCount / (uLifeTime * 0.001) / 1024,
        pSockInfo->m_uSendDataCount / (uLifeTime * 0.001) / 1024);
#endif
    Beats_DestroySocket(MySocket);
    return 0;
}

