#include "stdafx.h"
#include "FilePathTool.h"
#include "Serializer/Serializer.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#include <android/asset_manager.h>
#endif
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
#include <direct.h>
#else 
#include <sys/stat.h>
#endif
#include "StringHelper/StringHelper.h"
CFilePathTool* CFilePathTool::m_pInstance = NULL;
CFilePathTool::CFilePathTool()
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    : m_pAssetManager(NULL)
#endif
{

}

CFilePathTool::~CFilePathTool()
{

}

bool CFilePathTool::LoadFile(CSerializer* pSerializer, const TCHAR* pszFilePath, const TCHAR* pszMode, uint32_t uStartPos/* = 0*/, uint32_t uDataLength/* = 0*/)
{
    BEATS_ASSERT(pSerializer != NULL, _T("Serializer can't be null!"));
    BEATS_ASSERT(pszFilePath != NULL, _T("File path is NULL!"));
    BEATS_ASSERT(_tcslen(pszFilePath) > 0, _T("File path is empty!"));
    bool bRet = false;
    TString strFilePath = ConvertToUnixPath(pszFilePath);
    pszFilePath = strFilePath.c_str();
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    if (strFilePath[0] != _T('/'))
    {
        // Found "assets/" at the beginning of the path and we don't want it
        if (strFilePath.find(ASSET_ROOT_PATH) == 0)
        {
            pszFilePath += _tcslen(ASSET_ROOT_PATH);
        }
        BEATS_ASSERT(m_pAssetManager != NULL, _T("asset manager is null!"));
        if (m_pAssetManager) 
        {
            AAsset* pFile = AAssetManager_open(m_pAssetManager, pszFilePath, AASSET_MODE_UNKNOWN);
            BEATS_ASSERT(pFile != NULL, _T("Open file %s failed!"), pszFilePath);
            bRet = pFile != NULL;
            if (bRet)
            {
                off_t uFileSize = AAsset_getLength(pFile);
                if (uDataLength != 0)
                {
                    BEATS_ASSERT(uDataLength <= uFileSize - uStartPos);
                    uFileSize = uDataLength;
                }
                pSerializer->ValidateBuffer(uFileSize);
                AAsset_seek(pFile, uStartPos, SEEK_SET);

                int bytesread = AAsset_read(pFile, pSerializer->GetWritePtr(), uFileSize);
                BEATS_ASSERT(uFileSize == bytesread);
                uint32_t uOriginalPos = pSerializer->GetWritePos();
                pSerializer->SetWritePos(uOriginalPos + uFileSize);
                AAsset_close(pFile);
            }
        }
    }
    else
    {
#endif
        FILE* pFile = _tfopen(pszFilePath, pszMode);
        bRet = pFile != NULL;
        BEATS_ASSERT(pFile != NULL, _T("Can't open file %s"), pszFilePath);
        if (pFile != NULL)
        {
            pSerializer->Serialize(pFile, uStartPos, uDataLength);
            fclose(pFile);
        }
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    }
#endif
    return bRet;
}

TString CFilePathTool::ConvertToUnixPath(const TCHAR* pszFilePath) const
{
    TCHAR szBuffer[MAX_PATH];
    _tcscpy(szBuffer, pszFilePath);
    TCHAR* pReader = _tcsstr(szBuffer, _T("\\"));
    while(pReader != NULL)
    {
        *pReader = _T('/');
        pReader = _tcsstr(pReader, _T("\\"));
    }
    return szBuffer;
}

TString CFilePathTool::ConvertToWindowsPath(const TCHAR* pszFilePath) const
{
    TCHAR szBuffer[MAX_PATH];
    _tcscpy(szBuffer, pszFilePath);
    TCHAR* pReader = _tcsstr(szBuffer, _T("/"));
    while(pReader != NULL)
    {
        *pReader = _T('\\');
        pReader = _tcsstr(pReader, _T("/"));
    }
    return szBuffer;
}

bool CFilePathTool::MakeDirectory(const TCHAR* pszDirectoryPath) const
{
    TString strPath = ConvertToUnixPath(pszDirectoryPath);
    char lastCharacter = *strPath.rbegin();
    if (lastCharacter == '/' || lastCharacter == '\\')
    {
        strPath.resize(strPath.size() - 1);
    }
    std::vector<TString> directoryList;
    CStringHelper::GetInstance()->SplitString(strPath.c_str(), "/", directoryList, false);
    BEATS_ASSERT(directoryList.size() > 1);
    TString strCurrPath;
    strCurrPath.append(directoryList[0]).append("/");
    size_t i = 1;
    for (; i < directoryList.size(); ++i)
    {
        strCurrPath.append(directoryList[i]);
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
        _mkdir(strCurrPath.c_str());
#else
        mkdir(strCurrPath.c_str(), S_IRWXO | S_IRWXG | S_IRWXU);
#endif
        strCurrPath.append("/");
    }
    return i == directoryList.size();
}
