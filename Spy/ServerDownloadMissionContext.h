#ifndef BEATS_SPY_COMMAND_FILETRANSFER_SERVERDOWNLOADMISSIONCONTEXT_H__INCLUDE
#define BEATS_SPY_COMMAND_FILETRANSFER_SERVERDOWNLOADMISSIONCONTEXT_H__INCLUDE

#include <WinSock2.h>

struct SDownloadingTaskInfo
{
    SDownloadingTaskInfo()
        : m_pData(NULL)
        , m_uFileIndex(0)
        , m_uStartPos(0)
    {

    }

    SDownloadingTaskInfo(WIN32_FIND_DATA* pData, uint32_t uFileIndex, const long long& uStartPos)
        : m_pData(pData)
        , m_uFileIndex(uFileIndex)
        , m_uStartPos(uStartPos)
    {

    }
    ~SDownloadingTaskInfo()
    {

    }
    WIN32_FIND_DATA* m_pData;
    uint32_t m_uFileIndex;
    unsigned long long m_uStartPos;
};

enum EDownloadRule
{
    eDR_PieceFirst, // If a file is too big, we will only download a piece of it, we will continue downloading it when all other small files are trasfered.
    eDR_FileFirst,  // Always start to download a new file before the last file is transfered.

    //Keep eDR_Random is the last value please, it's easy to know how many other rules just by eDR_Random - 1.
    eDR_Random,     // When thread gets task, it will random select a new task or downloading task.

    eDR_Count,
    eDR_Force32Bit = 0xFFFFFFFF
};

struct SSocketContext;
struct SDirectory;

class CServerDownloadMissionContext
{
public:
    CServerDownloadMissionContext(SharePtr<SSocketContext>& pContext, std::vector<SDirectory*>* pDirectory, bool bIsUploadMode, EDownloadRule rule, const TString& storePath);
    ~CServerDownloadMissionContext();
    bool GetTask(SDownloadingTaskInfo& taskInfo);
    void SetListenSocket(SOCKET sock);
    SOCKET GetListenSocket();
    const std::vector<SDirectory*>* GetDownloadFiles() const;
    SharePtr<SSocketContext> GetSocketContext();
    bool IsUploadMode();
    const TString& GetStorePath() const;
    void SetDownloadRule(EDownloadRule rule);
    CRITICAL_SECTION& GetContextCriticalSection();
    const long long& GetUploadSize() const;
    void SetUploadSize(const long long& uSize);
    const unsigned long long& GetTotalSize() const;
    void SetTotalSize(const unsigned long long& uFileSize);

private:
    bool GetTaskFromDownloading(SDownloadingTaskInfo& taskInfo);
    bool GetTaskFromList(SDownloadingTaskInfo& taskInfo);
    bool GetTaskByRule(EDownloadRule rule, SDownloadingTaskInfo& taskInfo);

public:
    static const uint32_t WORK_LOCK_FILE_LENGTH = 25 * 1024 * 1024; // 25M, it means a file with 100M size will be sent by 4 threads at the same time.

private:
    bool m_bUploadMode;
    EDownloadRule m_downloadRule;
    std::vector<SDirectory*>* m_pDownloadFiles;
    SOCKET m_listenSocket;
    uint32_t m_uCurDownloadFileIndex;
    SharePtr<SSocketContext> m_pSocketContext;
    unsigned long long m_uTotalFileSize;
    long long m_uUploadedSize;
    TString m_strStorePath;
    std::vector<WIN32_FIND_DATA*> m_downloadTaskList;
    std::vector<SDownloadingTaskInfo> m_downloadingTaskList;
    CRITICAL_SECTION m_taskSection;
    CRITICAL_SECTION m_criticalSection;
};


#endif