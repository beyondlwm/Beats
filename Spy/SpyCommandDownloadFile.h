#ifndef BEATS_SPY_COMMAND_SPYCOMMANDDOWNLOADFILE_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDDOWNLOADFILE_H__INCLUDE

#include "SpyCommandBase.h"
#include "../Utility/UtilityManager.h"
#include <WinSock2.h>
#include <Shlwapi.h>
#include <string>

enum EDownloadMissionStep
{
    eDMS_ClientRequest,
    eDMS_ServerFeedBack,
    eDMS_ServerFinished,

    eDMS_Count,
    eDMS_Force32Bit = 0xFFFFFFFF
};

struct SClientDownloadMissionContext
{
    SClientDownloadMissionContext()
        : m_bUploadMode(false)
        , m_pSvrListenPort(0xFFFF)
        , m_uTotalFileCount(0)
        , m_uTotalFileSize(0)
        , m_uDownloadedSize(0)
    {
        InitializeCriticalSection(&m_criticalSection);
    }
    ~SClientDownloadMissionContext()
    {
        for (size_t i = 0; i < m_pDownloadFiles.size(); ++i)
        {
            BEATS_SAFE_DELETE(m_pDownloadFiles[i]);
        }
        DeleteCriticalSection(&m_criticalSection);
    }
    void BuildTaskList(SDirectory* pDirectory, const TCHAR* pszRootPath)
    {
        if (pDirectory->m_pParent == NULL) // this is the root directory, change its path to the save root path.
        {
            m_pDownloadFiles.push_back(pDirectory);
            if (pDirectory->m_data.cFileName[0] != 0) // This is a directory.
            {
                TString strNewDirectoryPath = m_saveRootPath;
                if (*strNewDirectoryPath.rbegin() != _T('\\'))
                {
                    strNewDirectoryPath.append(_T("\\"));
                }
                strNewDirectoryPath.append(pDirectory->m_data.cFileName);
                CreateDirectory(strNewDirectoryPath.c_str(), NULL);
                SetFileAttributes(strNewDirectoryPath.c_str(), pDirectory->m_data.dwFileAttributes);
                pDirectory->m_szPath = strNewDirectoryPath;
                pDirectory->m_szPath.append(_T("\\"));
            }
            else
            {
                pDirectory->m_szPath = m_saveRootPath;
            }
        }
        for (size_t i = 0; i < pDirectory->m_pFileList->size(); ++i)
        {
            m_downloadTaskList.push_back((*pDirectory->m_pFileList)[i]);
        }
        for (size_t i = 0; i < pDirectory->m_pDirectories->size(); ++i)
        {
            TString strNewDirectoryPath = pDirectory->m_szPath;
            strNewDirectoryPath.append((*pDirectory->m_pDirectories)[i]->m_data.cFileName);
            CreateDirectory(strNewDirectoryPath.c_str(), NULL);
            SetFileAttributes(strNewDirectoryPath.c_str(), (*pDirectory->m_pDirectories)[i]->m_data.dwFileAttributes);
            strNewDirectoryPath.append(_T("\\"));
            (*pDirectory->m_pDirectories)[i]->m_szPath = strNewDirectoryPath;
            BuildTaskList((*pDirectory->m_pDirectories)[i], pszRootPath);
        }
    }
    bool m_bUploadMode;
    u_short m_pSvrListenPort;
    std::vector<SDirectory*> m_pDownloadFiles;
    SharePtr<SSocketContext> m_pSocketContext;
    size_t m_uTotalFileCount;
    long long m_uTotalFileSize;
    long long m_uDownloadedSize;
    TString m_saveRootPath;
    CRITICAL_SECTION m_criticalSection;
    std::vector<WIN32_FIND_DATA*> m_downloadTaskList;
};

class CServerDownloadMissionContext;

class CSpyCommandDownloadFile : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandDownloadFile();
    virtual ~CSpyCommandDownloadFile();

    virtual bool ExecuteImpl(SharePtr<SSocketContext>& pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

    void SetDownloadFiles(const std::vector<TString>& downloadFiles);
    void SetStorePath(const TCHAR* pszStorePath);
    void SetMissionStep(EDownloadMissionStep step);
    void SetSvrDownloadMissionContext(CServerDownloadMissionContext* pContext);
    void SetUploadMode(bool bUpload);
    void SetFileFilter(CFileFilter* pFileFilter);

private:
    static DWORD SvrDownloadMissionThreadFunc(LPVOID param);
    static DWORD SvrDownloadWorkThreadFunc(LPVOID param);
    static void SvrDownloadSendFileData(SOCKET sock, HANDLE hFile, BYTE* pBuffer, const unsigned long long& uStartPos, size_t uSendLength, 
                                        const unsigned long long& uFileSize);

    static DWORD ClientDownloadMissionThreadFunc(LPVOID param);
    static DWORD ClientDownloadWorkThreadFunc(LPVOID param);

private:
    bool m_bUploadMode;
    bool m_bHasFilterData;
    CFileFilter* m_pFileFilter;
    CServerDownloadMissionContext* m_pSvrDownloadMissionContext;
    SClientDownloadMissionContext* m_pClientDownloadMissionContext;
    EDownloadMissionStep m_step;
    std::vector<TString> m_downloadFiles;
    TString m_storePath;

};
#endif