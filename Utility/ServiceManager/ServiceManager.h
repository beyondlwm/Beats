#ifndef BEATS_UTILITY_SERVICEMANAGER_H__INCLUDE
#define BEATS_UTILITY_SERVICEMANAGER_H__INCLUDE

class CServiceManager
{
    BEATS_DECLARE_SINGLETON(CServiceManager)

public:

    bool Install(const TCHAR* pszServiceName, const TCHAR* pszServiceDesc, const std::vector<TString>* pDependLibs = NULL);
    bool UnInstall(const TCHAR* pszServiceName, const std::vector<TString>* pDependLibs = NULL);
    bool IsInstalled(const TCHAR* pszServiceName);
};

#endif
