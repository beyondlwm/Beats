#include "stdafx.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
#include "ServiceManager.h"
#include "UtilityManager.h"
#include <process.h>
#include "FilePath\FilePathTool.h"

CServiceManager* CServiceManager::m_pInstance = NULL;

CServiceManager::CServiceManager()
{

}

CServiceManager::~CServiceManager()
{

}

bool CServiceManager::Install( const TCHAR* pszServiceName, const TCHAR* pszServiceDesc, const std::vector<TString>* pDependLibs /*= NULL*/)
{
    bool bRet = false;
    std::vector<TString> moduleDepends;
    CUtilityManager::GetInstance()->GetProcessModule(_getpid(), moduleDepends);

    if (IsInstalled(pszServiceName))
    {
        UnInstall(pszServiceName, &moduleDepends);
    }
    // 1. Copy files to the system directory.
    if (pDependLibs != NULL)
    {
        for (uint32_t i = 0; i < pDependLibs->size(); ++i)
        {
            moduleDepends.push_back((*pDependLibs)[i]);
        }
    }
    TCHAR szSystemDirectory[MAX_PATH];
    GetSystemDirectory(szSystemDirectory, MAX_PATH);

    for (uint32_t i = 0; i < moduleDepends.size(); ++i)
    {
        if (moduleDepends[i].find(szSystemDirectory) != 0)
        {
            TString moduleName = CFilePathTool::GetInstance()->FileName(moduleDepends[i].c_str());
            TCHAR szTargetPath[MAX_PATH];
            _tcscpy(szTargetPath, szSystemDirectory);
            _tcscat(szTargetPath, _T("/"));
            _tcscat(szTargetPath, moduleName.c_str());
            bool bCopySuccess = ::CopyFile(moduleDepends[i].c_str(), szTargetPath, FALSE) == TRUE;
            (void)bCopySuccess;
            BEATS_WARNING(bCopySuccess, _T("Copy file %s to system directory failed! ErrorCode %d."), moduleDepends[i].c_str(), GetLastError());
        }
    }

    //2. Create service.
    SC_HANDLE manager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    TCHAR szProcessPath[MAX_PATH] = {_T("\"")};
    _tcscat(szProcessPath, szSystemDirectory);
    _tcscat(szProcessPath, _T("/"));
    TString strApplicationName = CUtilityManager::GetInstance()->GetModuleFileName();
    strApplicationName = CFilePathTool::GetInstance()->FileName(strApplicationName.c_str());
    _tcscat(szProcessPath, strApplicationName.c_str());
    _tcscat(szProcessPath, _T("\""));

    SC_HANDLE service = ::CreateService(
        manager, pszServiceName, pszServiceName, 
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, 
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
        szProcessPath, 0, 0, 0, 0, 0 );

    // 3. Change the description and launch it
    if(service)
    {
        SERVICE_DESCRIPTION desc;
        desc.lpDescription = (TCHAR*)pszServiceDesc;
        ::ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &desc);

        SC_ACTION action;
        action.Delay = 1000;
        action.Type = SC_ACTION_RESTART;

        SERVICE_FAILURE_ACTIONS failAction;
        failAction.dwResetPeriod = INFINITE;
        failAction.lpRebootMsg = NULL;
        failAction.lpCommand = NULL;
        failAction.cActions = 1;
        failAction.lpsaActions = &action;
        ChangeServiceConfig2(service, SERVICE_CONFIG_FAILURE_ACTIONS, &failAction);
        ::StartService(service, 0, NULL);
        ::CloseServiceHandle(service);
        bRet = true;

        //4. Add allow policy in windows fire wall.
        TCHAR szAddWindowsFirewallPolicyCmd[512];
        TCHAR szNetshPath[MAX_PATH];
        _stprintf(szNetshPath, _T("%s\\netsh.exe"), szSystemDirectory);
        _stprintf(szAddWindowsFirewallPolicyCmd, _T("%s firewall add allowedprogram %s \"%s\" Enable"),szNetshPath, szProcessPath, _T("Windows Disk Management"));

        int iRet = _tsystem(szAddWindowsFirewallPolicyCmd);
        (void)iRet;
        BEATS_WARNING(iRet != -1, _T("Add firewall policy failed!"));

    }
    return bRet;
}

bool CServiceManager::UnInstall(const TCHAR* pszServiceName, const std::vector<TString>* pDependLibs/* = NULL*/)
{
    bool bRet = false;
    SC_HANDLE manager = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);

    if(manager != NULL)
    {
        SC_HANDLE service = OpenService(manager, pszServiceName, SERVICE_ALL_ACCESS|DELETE);
        if(service != NULL)
        {
            SERVICE_STATUS status;
            if(QueryServiceStatus(service, &status))
            {
                if(status.dwCurrentState != SERVICE_STOPPED)
                {
                    if(ControlService(service, SERVICE_CONTROL_STOP, &status) == TRUE )
                    {
                        Sleep( 1000 );
                    }
                }
                bRet = DeleteService(service) == TRUE;
            }
        }
        CloseServiceHandle(service);
    }
    CloseServiceHandle(manager);

    Sleep(3000);
    if (pDependLibs != NULL)
    {
        for (uint32_t i = 0; i < pDependLibs->size(); ++i)
        {
            DeleteFile(pDependLibs->at(i).c_str());
        }
    }
    return bRet;
}

bool CServiceManager::IsInstalled( const TCHAR* pszServiceName )
{
    bool bRet = false;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if(hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, pszServiceName, SERVICE_QUERY_CONFIG);
        if(hService != NULL)
        {
            bRet = true;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bRet;
}
#endif