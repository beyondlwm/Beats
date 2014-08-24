#include "stdafx.h"
#include "ServerDownloadMissionContext.h"
#include "../Utility/UtilityManager.h"

CServerDownloadMissionContext::CServerDownloadMissionContext(SharePtr<SSocketContext>& pContext, std::vector<SDirectory*>* pDirectory, bool bIsUploadMode, 
                                                             EDownloadRule rule, const TString& storePath)
: m_pSocketContext(pContext)
, m_pDownloadFiles(pDirectory)
, m_bUploadMode(bIsUploadMode)
, m_downloadRule(rule)
, m_strStorePath(storePath)
, m_listenSocket(INVALID_SOCKET)
, m_uCurDownloadFileIndex(0)
, m_uTotalFileSize(0)
, m_uUploadedSize(0)
{
    InitializeCriticalSection(&m_taskSection);
    InitializeCriticalSection(&m_criticalSection);
    BEATS_ASSERT(m_pDownloadFiles != NULL && m_pDownloadFiles->size() > 0, _T("Download Files Can't be NULL or Empty!'"));
    for (size_t i = 0; i < m_pDownloadFiles->size(); ++i)
    {
        CUtilityManager::GetInstance()->BuildDirectoryToList((*m_pDownloadFiles)[i], m_downloadTaskList);
    }
}

CServerDownloadMissionContext::~CServerDownloadMissionContext()
{
    BEATS_SAFE_DELETE_VECTOR(*m_pDownloadFiles);
    BEATS_SAFE_DELETE(m_pDownloadFiles);
    DeleteCriticalSection(&m_taskSection);
    DeleteCriticalSection(&m_criticalSection);
}

bool CServerDownloadMissionContext::GetTaskFromDownloading(SDownloadingTaskInfo& taskInfo)
{
    //TODO: should remove this assert after the function is stable, because we can change the rule dynamicaly in downloading process.
    BEATS_ASSERT(m_downloadRule != eDR_PieceFirst || m_downloadTaskList.size() == m_uCurDownloadFileIndex,
        _T("Can't get downloading task when the new download task list is not empty, under eDR_PieceFirst rule."));

    bool bRet = m_downloadingTaskList.size() > 0;
    if (bRet)
    {
        SDownloadingTaskInfo& info = m_downloadingTaskList.back();
        taskInfo = info;
        // Update the content of task info.
        WIN32_FIND_DATA* pFileData = taskInfo.m_pData;
        long long uFileSize = pFileData->nFileSizeHigh;
        uFileSize = uFileSize << 32;
        uFileSize += pFileData->nFileSizeLow;
        long long uUnlockSize = uFileSize - taskInfo.m_uStartPos;
        if (uUnlockSize <= WORK_LOCK_FILE_LENGTH)
        {
            m_downloadingTaskList.pop_back();
        }
        else
        {
            info.m_uStartPos += WORK_LOCK_FILE_LENGTH;
        }
    }
    return bRet;
}

bool CServerDownloadMissionContext::GetTaskFromList(SDownloadingTaskInfo& taskInfo)
{
    //TODO: should remove this assert after the function is stable, because we can change the rule dynamicaly in downloading process.
    BEATS_ASSERT(m_downloadRule != eDR_FileFirst || m_downloadingTaskList.size() == 0,
                _T("Can't get new task when the downloading task list is not empty, under eDR_FileFirst rule."));
    bool bRet = m_uCurDownloadFileIndex < m_downloadTaskList.size();
    if (bRet)
    {
        taskInfo.m_pData = m_downloadTaskList[m_uCurDownloadFileIndex];
        taskInfo.m_uFileIndex = m_uCurDownloadFileIndex;
        taskInfo.m_uStartPos = 0;
        long long uFileSize = taskInfo.m_pData->nFileSizeHigh;
        uFileSize = uFileSize << 32;
        uFileSize += taskInfo.m_pData->nFileSizeLow;
        if (uFileSize > WORK_LOCK_FILE_LENGTH)
        {
#ifdef _DEBUG
            for (size_t i = 0; i < m_downloadingTaskList.size(); ++i)
            {
                BEATS_ASSERT(m_downloadingTaskList[i].m_pData != taskInfo.m_pData, _T("Add downloading task info failed! it is already exists! file %s"), taskInfo.m_pData->cFileName);
            }
#endif
            m_downloadingTaskList.push_back(taskInfo);
            m_downloadingTaskList.back().m_uStartPos = WORK_LOCK_FILE_LENGTH;
        }
        ++m_uCurDownloadFileIndex;
    }
    return bRet;
}

bool CServerDownloadMissionContext::GetTaskByRule( EDownloadRule rule, SDownloadingTaskInfo& taskInfo)
{
    bool bRet = false;
    switch (rule)
    {
    case eDR_FileFirst:
        {
            bRet = GetTaskFromDownloading(taskInfo);
            if (!bRet)
            {
                bRet = GetTaskFromList(taskInfo);
            }
        }
        break;
    case eDR_PieceFirst:
        {
            bRet = GetTaskFromList(taskInfo);
            if (!bRet)
            {
                bRet = GetTaskFromDownloading(taskInfo);
            }
        }
        break;
    case eDR_Random:
        {
            EDownloadRule randomRule =(EDownloadRule)(rand() % eDR_Random);
            BEATS_ASSERT(randomRule != eDR_Random, _T("It will never be random again, check if eDR_Random is the last value of EDownloadRule!"));
            bRet = GetTaskByRule(randomRule, taskInfo);
        }
        break;
    default:
        BEATS_ASSERT(FALSE, _T("Never be here!"));
        break;
    }
    return bRet;
}

bool CServerDownloadMissionContext::GetTask(SDownloadingTaskInfo& taskInfo)
{
    EnterCriticalSection(&m_taskSection);
    bool bRet = GetTaskByRule(m_downloadRule, taskInfo);
    LeaveCriticalSection(&m_taskSection);
    return bRet;
}

void CServerDownloadMissionContext::SetListenSocket(SOCKET sock)
{
    m_listenSocket = sock;
}

SOCKET CServerDownloadMissionContext::GetListenSocket()
{
    return m_listenSocket;
}

const std::vector<SDirectory*>* CServerDownloadMissionContext::GetDownloadFiles() const
{
    return m_pDownloadFiles;
}

SharePtr<SSocketContext> CServerDownloadMissionContext::GetSocketContext()
{
    return m_pSocketContext;
}

bool CServerDownloadMissionContext::IsUploadMode()
{
    return m_bUploadMode;
}

const TString& CServerDownloadMissionContext::GetStorePath() const
{
    return m_strStorePath;
}

void CServerDownloadMissionContext::SetDownloadRule( EDownloadRule rule )
{
    if (m_downloadRule != rule)
    {
        EnterCriticalSection(&m_taskSection);
        m_downloadRule = rule;
        LeaveCriticalSection(&m_taskSection);
    }
}

CRITICAL_SECTION& CServerDownloadMissionContext::GetContextCriticalSection()
{
    return m_criticalSection;
}

const long long& CServerDownloadMissionContext::GetUploadSize() const
{
    return m_uUploadedSize;
}

void CServerDownloadMissionContext::SetUploadSize(const long long& uSize)
{
    m_uUploadedSize = uSize;
}

const unsigned long long& CServerDownloadMissionContext::GetTotalSize() const
{
    return m_uTotalFileSize;
}

void CServerDownloadMissionContext::SetTotalSize(const unsigned long long& uFileSize)
{
    m_uTotalFileSize = uFileSize;
}