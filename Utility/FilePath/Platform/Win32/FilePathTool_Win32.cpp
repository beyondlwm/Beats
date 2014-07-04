#include "stdafx.h"
#include "Utility/FilePath/FilePathTool.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool CFilePathTool::Exists(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    BEATS_ASSERT(!IsDirectory(pszPath));
    return PathFileExists(pszPath) == TRUE;
}

bool CFilePathTool::IsAbsolute(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    bool bRet = _tcslen(pszPath) > 2
        && pszPath[1] == _T(':')
        && ((pszPath[0] >= _T('a') && pszPath[0] <= _T('z')) || (pszPath[0] >= _T('A') && pszPath[0] <= _T('Z')));
    return bRet;
}

bool CFilePathTool::IsDirectory(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    return PathIsDirectory(pszPath) == TRUE;
}

TString CFilePathTool::ParentPath(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    TCHAR szBuffer[MAX_PATH];
    _tcscpy(szBuffer, pszPath);
    PathRemoveFileSpec(szBuffer);
    return szBuffer;
}

TString CFilePathTool::Extension(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    return PathFindExtension(pszPath);
}

TString CFilePathTool::FileName(const TCHAR* pszPath)
{
    BEATS_ASSERT(pszPath != NULL && _tcslen(pszPath) > 0);
    return PathFindFileName(pszPath);
}

TString CFilePathTool::MakeRelative(const TCHAR* pszFrom, const TCHAR* pszTo)
{
    TCHAR szBuffer[MAX_PATH];
    BEATS_ASSERT(pszFrom != NULL && _tcslen(pszFrom) > 0 && pszTo != NULL);
    DWORD fromFlag = IsDirectory(pszFrom) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
    DWORD toFlag = IsDirectory(pszTo) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
    PathRelativePathTo(szBuffer, pszFrom, fromFlag, pszTo, toFlag);
    return szBuffer;
}

TString CFilePathTool::MakeAbsolute(const TCHAR* pszStartPath, const TCHAR* pszRelativePath)
{
    TCHAR szBuffer[MAX_PATH];
    BEATS_ASSERT(pszStartPath != NULL && _tcslen(pszStartPath) > 0 && pszRelativePath != NULL);
    BEATS_ASSERT(IsAbsolute(pszStartPath), _T("%s is not an absolute path"), pszStartPath);
    TString tmp(pszStartPath);
    if (*tmp.rbegin() != _T('\\') || *tmp.rbegin() != _T('/'))
    {
        tmp.append(_T("\\"));
    }
    tmp.append(pszRelativePath);
    bool bRet = PathCanonicalize(szBuffer, tmp.c_str()) != FALSE;
    BEATS_ASSERT(bRet);
    return szBuffer;
}

TString CFilePathTool::RootPath( const TCHAR* pszPath )
{
    TCHAR szBuffer[MAX_PATH];
    _tcscpy(szBuffer, pszPath);
    PathStripToRoot(szBuffer);
    return szBuffer;
}

bool CFilePathTool::Canonical(TCHAR* pszOutBuffer, const TCHAR* pszOriginPath)
{
    return PathCanonicalize(pszOutBuffer, pszOriginPath) != FALSE;
}