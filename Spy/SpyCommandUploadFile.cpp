#include "stdafx.h"
#include "SpyCommandUploadFile.h"
#include "SpyCommandDownloadFile.h"
#include "SpyCommandManager.h"
#include "../Utility/FileFilter/FileFilter.h"
#include <boost/filesystem.hpp>

CSpyCommandUploadFile::CSpyCommandUploadFile()
: super(eSCT_Upload)
, m_pFileFilter(NULL)
{

}

CSpyCommandUploadFile::~CSpyCommandUploadFile()
{
}
void CSpyCommandUploadFile::SetUploadFile(const std::vector<TString>& uploadFileNames)
{
    m_uploadFileNames = uploadFileNames;
}

void CSpyCommandUploadFile::SetStorePath(const TCHAR* pszStorePath)
{
    m_storePath.assign(pszStorePath);
}

bool CSpyCommandUploadFile::ExecuteImpl( SharePtr<SSocketContext>& pSocketContext )
{
    if (m_uploadFileNames.size() > 0)
    {
        WIN32_FILE_ATTRIBUTE_DATA fileData;
        bool bFileExists = GetFileAttributesEx(m_storePath.c_str(), GetFileExInfoStandard, &fileData) == TRUE;

        if (bFileExists && (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            CSpyCommandDownloadFile downloadCmd;
            downloadCmd.SetStorePath(m_storePath.c_str());
            downloadCmd.SetDownloadFiles(m_uploadFileNames);
            downloadCmd.SetUploadMode(true);
            if (m_pFileFilter != NULL)
            {
                downloadCmd.SetFileFilter(m_pFileFilter);
            }
            CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, &downloadCmd);
            BEATS_SAFE_DELETE(m_pFileFilter);
        }
    }

    return true;
}

void CSpyCommandUploadFile::Deserialize( CSerializer& serializer )
{
    super::Deserialize(serializer);

    serializer << m_uploadFileNames.size();
    for (size_t i = 0; i < m_uploadFileNames.size(); ++i)
    {
        bool bParamValid = m_uploadFileNames[i].length() > 0 && m_storePath.length() > 0 && boost::filesystem::exists(m_uploadFileNames[i].c_str());
        BEATS_ASSERT(bParamValid, _T("Upload file %s failed!"));
        serializer << bParamValid;
        if (bParamValid)
        {
            serializer << m_uploadFileNames[i];
            serializer << m_storePath;
            serializer << (m_pFileFilter != NULL);
            if (m_pFileFilter != NULL)
            {
                m_pFileFilter->Deserialize(serializer);
            }
        }
    }
}

void CSpyCommandUploadFile::Serialize( CSerializer& serializer )
{
    super::Serialize(serializer);
    size_t uFileCount = 0;
    serializer >> uFileCount;
    m_uploadFileNames.resize(uFileCount);
    for (size_t i = 0; i < uFileCount; ++i)
    {
        bool bParamValid = false;
        serializer >> bParamValid;
        if (bParamValid)
        {
            serializer >> m_uploadFileNames[i];
            serializer >> m_storePath;
            bool bHasFileFilter;
            serializer >> bHasFileFilter;
            if (bHasFileFilter)
            {
                m_pFileFilter = new CFileFilter;
                m_pFileFilter->Serialize(serializer);
            }
        }
    }
}

void CSpyCommandUploadFile::SetFileFilter( CFileFilter* pFileFilter )
{
    m_pFileFilter = pFileFilter;
}
