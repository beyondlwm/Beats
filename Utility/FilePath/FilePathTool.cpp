#include "stdafx.h"
#include "FilePathTool.h"
#include "Serializer/Serializer.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
#include <android/asset_manager.h>
#endif
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
#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    TString strFilePath(pszFilePath);
    BEATS_ASSERT(strFilePath[0] != _T('/'));
    if (strFilePath[0] != _T('/'))
    {
        const TCHAR* pszFilePath = strFilePath.c_str();
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
                pSerializer->SetWritePos(uFileSize);
                BEATS_ASSERT(uFileSize == bytesread);
                AAsset_close(pFile);
            }
        }
    }
#else
    FILE* pFile = _tfopen(pszFilePath, pszMode);
    bRet = pFile != NULL;
    BEATS_ASSERT(pFile != NULL, _T("Can't open file %s"), pszFilePath);
    if (pFile != NULL)
    {
        pSerializer->Serialize(pFile, uStartPos, uDataLength);
        fclose(pFile);
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
