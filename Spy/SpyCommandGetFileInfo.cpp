#include "stdafx.h"
#include "SpyCommandGetFileInfo.h"
#include "../Utility/UtilityManager.h"
#include "SpyCommandManager.h"
#include <locale.h>

CSpyCommandGetFileInfo::CSpyCommandGetFileInfo()
: super(eSCT_GetDirectoryInfo)
, m_type(eFIT_Count)
, m_pDirectory(NULL)
, m_pDiskInfo(NULL)
{
}

CSpyCommandGetFileInfo::~CSpyCommandGetFileInfo()
{
    BEATS_SAFE_DELETE(m_pDiskInfo);
    BEATS_SAFE_DELETE(m_pDirectory);
}

void CSpyCommandGetFileInfo::SetFileName(const TCHAR* pszFileName)
{
    m_fileName.assign(pszFileName);
}

bool CSpyCommandGetFileInfo::ExecuteImpl( SharePtr<SSocketContext>&  pSocketContext )
{
    setlocale(LC_ALL, "chs");
    bool bIsQuery = m_type == eFIT_Count;
    if (bIsQuery)
    {
        // Empty string means we will get the root info.
        if (m_fileName.length() == 0)
        {
            m_type = eFIT_Disk;
            if (m_pDiskInfo != NULL)
            {
                BEATS_SAFE_DELETE(m_pDiskInfo);
            }
            m_pDiskInfo = new SDiskInfo;
            TCHAR szDiskStrBuffer[MAX_PATH] = {0};
            GetLogicalDriveStrings(MAX_PATH, szDiskStrBuffer);
            TCHAR* pDiskStrReader = szDiskStrBuffer;
            while (*pDiskStrReader != 0)
            {
                SLogicDriverInfo info;
                info.m_diskName = pDiskStrReader;
                info.m_type = (char)GetDriveType(pDiskStrReader);
                ULARGE_INTEGER uFreeBytesAvailable;
                ULARGE_INTEGER uTotalNumberOfBytes;
                ULARGE_INTEGER uTotalNumberOfFreeBytes;
                if (GetDiskFreeSpaceEx(pDiskStrReader, &uFreeBytesAvailable, &uTotalNumberOfBytes, &uTotalNumberOfFreeBytes))
                {
                    info.m_diskSize = uTotalNumberOfBytes.QuadPart;
                    info.m_diskFreeSize = uTotalNumberOfFreeBytes.QuadPart;
                }
                m_pDiskInfo->m_logicDriverInfo.push_back(info);
                pDiskStrReader += (_tcslen(pDiskStrReader) + 1);
            }
        }
        else
        {
            bool bGetFileAttr = GetFileAttributesEx(m_fileName.c_str(), GetFileExInfoStandard, &m_fileData) == TRUE;
            if (bGetFileAttr)
            {
                if((m_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                {
                    m_type = eFIT_Directory;
                    if (m_pDirectory != NULL)
                    {
                        BEATS_SAFE_DELETE(m_pDirectory);
                    }
                    m_pDirectory = new SDirectory(NULL, m_fileName.c_str());
                    CUtilityManager::GetInstance()->FillDirectory(*m_pDirectory, false);
                }
                else
                {
                    m_type = eFIT_File;
                }
            }
            else
            {
                m_type = eFIT_Error;
                TCHAR szErrorInfo[MAX_PATH];
                _stprintf(szErrorInfo, _T("Get attribute of file %s failed! Error Code : %d"), m_fileName.c_str(), GetLastError());
                m_errorInfo = szErrorInfo;
            }
        }
        CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, this);
    }
    else if (m_type == eFIT_Disk)
    {
        uint32_t uLogicDriverCount = m_pDiskInfo->m_logicDriverInfo.size();
        _tprintf(_T("There are %d logic drivers:\n"), uLogicDriverCount);
        for (uint32_t i = 0; i < uLogicDriverCount; ++i)
        {
            SLogicDriverInfo& info = m_pDiskInfo->m_logicDriverInfo[i];
            _tprintf(_T("%d. Disk(%s) %s total size %lld free size %lld\n"), i, GetDiskTypeString(info.m_type), info.m_diskName.c_str(), info.m_diskSize, info.m_diskFreeSize); 
        }        
    }
    else if (m_type == eFIT_File)
    {
        LARGE_INTEGER uFileSize;
        uFileSize.HighPart = m_fileData.nFileSizeHigh;
        uFileSize.LowPart = m_fileData.nFileSizeLow;
        _tprintf(_T("File Info:\n Name: %s size: %lld\n"), m_fileName.c_str(), uFileSize.QuadPart);
    }
    else if (m_type == eFIT_Directory)
    {
        uint32_t uFileCount = m_pDirectory->m_pFileList->size();
        uint32_t uDirectoryCount = m_pDirectory->m_pDirectories->size();
        _tprintf(_T("Directory: %s contains %d files and %d directories:\n"), m_pDirectory->m_szPath.c_str(), uFileCount, uDirectoryCount);
        _tprintf(_T("Directory:\n"));
        for (uint32_t i = 0; i < uDirectoryCount; ++i)
        {
            SDirectory* pSubDirectory = (*m_pDirectory->m_pDirectories)[i];
            _tprintf(_T("%d. %s\n"), i, pSubDirectory->m_szPath.c_str());
        }
        _tprintf(_T("File:\n"));
        LARGE_INTEGER uFileSize;
        for (uint32_t i = 0; i < uFileCount; ++i)
        {
            WIN32_FIND_DATA* pSubFile = (*m_pDirectory->m_pFileList)[i];
            uFileSize.HighPart = pSubFile->nFileSizeHigh;
            uFileSize.LowPart = pSubFile->nFileSizeLow;
            _tprintf(_T("%d. %s size: %lld \n"), i, pSubFile->cFileName, uFileSize.QuadPart);
        }
    }
    else if (m_type == eFIT_Error)
    {
        _tprintf(m_errorInfo.c_str());
        _tprintf(_T("\n"));
    }
    setlocale(LC_ALL, "C");
    if (!bIsQuery)
    {
        if (m_uFeedbackFunc != 0)
        {
            TFileInfoFeedbackFunc pFeedbackFunc = (TFileInfoFeedbackFunc)m_uFeedbackFunc;
            void* pFileData = NULL;
            switch (m_type)
            {
            case eFIT_File:
                pFileData = &m_fileData;
                break;
            case eFIT_Directory:
                pFileData = m_pDirectory;
                break;
            case eFIT_Disk:
                pFileData = m_pDiskInfo;
                break;
            case eFIT_Error:
                pFileData = (void*)m_errorInfo.c_str();
                break;
            default:
                break;
            }
            pFeedbackFunc((char)m_type, pFileData, (void*) m_uUserDataPtr);

        }
    }
    return true;
}

void CSpyCommandGetFileInfo::Deserialize( CSerializer& serializer )
{
    super::Deserialize(serializer);

    serializer << m_type;
    if (m_type == eFIT_Count)
    {
        serializer << m_fileName;
    }
    else if (m_type == eFIT_Disk)
    {
        BEATS_ASSERT(m_pDiskInfo != NULL);
        uint32_t uLogicDriverCount = m_pDiskInfo->m_logicDriverInfo.size();
        serializer << uLogicDriverCount;
        for (uint32_t i = 0; i < uLogicDriverCount; ++i)
        {
            SLogicDriverInfo& info = m_pDiskInfo->m_logicDriverInfo[i];
            serializer << info.m_type;
            serializer << info.m_diskName;
            serializer << info.m_diskSize;
            serializer << info.m_diskFreeSize;
        }
    }
    else if (m_type == eFIT_Directory)
    {
        BEATS_ASSERT(m_pDirectory != NULL);
        CUtilityManager::GetInstance()->SerializeDirectory(m_pDirectory, serializer);
    }
    else if (m_type == eFIT_File)
    {
        serializer << m_fileName;
        serializer << m_fileData;
    }
    else if (m_type == eFIT_Error)
    {
        serializer << m_errorInfo;
    }
}

void CSpyCommandGetFileInfo::Serialize( CSerializer& serializer )
{
    super::Serialize(serializer);
    serializer >> m_type;
    if (m_type == eFIT_Count)
    {
        serializer >> m_fileName;
    }
    else if (m_type == eFIT_Disk)
    {
        if (m_pDiskInfo != NULL)
        {
            BEATS_SAFE_DELETE(m_pDiskInfo);
        }
        uint32_t uLogicDriverCount = 0;
        serializer >> uLogicDriverCount;
        m_pDiskInfo = new SDiskInfo;
        for (uint32_t i = 0; i < uLogicDriverCount; ++i)
        {
            SLogicDriverInfo info;
            serializer >> info.m_type;
            serializer >> info.m_diskName;
            serializer >> info.m_diskSize;
            serializer >> info.m_diskFreeSize;
            m_pDiskInfo->m_logicDriverInfo.push_back(info);
        }
    }
    else if (m_type == eFIT_Directory)
    {
        if (m_pDirectory != NULL)
        {
            BEATS_SAFE_DELETE(m_pDirectory);
        }
        m_pDirectory = new SDirectory(NULL, NULL);
        CUtilityManager::GetInstance()->DeserializeDirectory(m_pDirectory, serializer);
    }
    else if (m_type == eFIT_File)
    {
        serializer >> m_fileName;
        serializer >> m_fileData;
    }
    else if (m_type == eFIT_Error)
    {
        serializer >> m_errorInfo;
    }
}

const TCHAR* CSpyCommandGetFileInfo::GetDiskTypeString( char diskType )
{
    const TCHAR* pszRet = _T("Unknown");
    switch (diskType)
    {
    case DRIVE_REMOVABLE:
        pszRet = _T("RemoveAble");
        break;
    case DRIVE_FIXED:
        pszRet = _T("Fixed");
        break;
    case DRIVE_REMOTE:
        pszRet = _T("Remote");
        break;
    case DRIVE_CDROM:
        pszRet = _T("CDRom");
        break;
    case DRIVE_RAMDISK:
        pszRet = _T("RamDisk");
        break;
    case DRIVE_UNKNOWN:
    case DRIVE_NO_ROOT_DIR:
    default:
        break;
    }
    return pszRet;
}

