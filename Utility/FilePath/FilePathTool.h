#ifndef BEATS_UTILITY_FILEPATH_FILEPATHTOOL_H__INCLUDE
#define BEATS_UTILITY_FILEPATH_FILEPATHTOOL_H__INCLUDE

#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    #define ASSET_ROOT_PATH _T("assets/")
#endif
class CSerializer;
class CFilePathTool
{
    BEATS_DECLARE_SINGLETON(CFilePathTool)
public:
    bool Exists(const TCHAR* pszPath);
    bool IsAbsolute(const TCHAR* pszPath);
    bool IsDirectory(const TCHAR* pszPath);
    TString ParentPath(const TCHAR* pszPath);
    TString Extension(const TCHAR* pszPath);
    TString FileName(const TCHAR* pszPath);
    TString MakeRelative(const TCHAR* pszFrom, const TCHAR* pszTo);
    TString MakeAbsolute(const TCHAR* pszStartPath, const TCHAR* pszRelativePath);
    TString RootPath(const TCHAR* pszPath);
    bool Canonical(TCHAR* pszOutBuffer, const TCHAR* pszOriginPath);

#if (BEATS_PLATFORM == BEATS_PLATFORM_ANDROID)
    void SetAssetManager(AAssetManager* pMgr);
    AAssetManager* GetAssetManager() const;
private:
    AAssetManager* m_pAssetManager;
#endif

};




#endif