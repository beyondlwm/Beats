#include "stdafx.h"
#include "SpyHookCBT.h"
#include "SpyHookGetMessage.h"

extern HWND g_hLastInputHwnd;

CSpyHookCBT::CSpyHookCBT()
{

}

CSpyHookCBT::~CSpyHookCBT()
{

}

bool CSpyHookCBT::StartImpl()
{
    m_hHookHandle = SetWindowsHookEx(WH_CBT, &CBTProc, m_hModuleHandle, 0);
    return m_hHookHandle != NULL;
}

LRESULT CALLBACK CSpyHookCBT::CBTProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    switch (nCode)
    {
    case HCBT_ACTIVATE:
        {
            HWND hWindow = (HWND)wParam;
            TCHAR szName[MAX_PATH] = {0};
            GetWindowText(hWindow, szName, MAX_PATH);
            CBTACTIVATESTRUCT* pData = (CBTACTIVATESTRUCT*)lParam;
            BEATS_PRINT(_T("HCBT_ACTIVATE: window handle: %d name:%s cause by mouse: %s handle:%d\n"), hWindow, szName, pData->fMouse ? _T("Yes") : _T("No"), pData->hWndActive);
        }
        break;
    case HCBT_CREATEWND:
        {
            HWND hWindow = (HWND)wParam;
            TCHAR szName[MAX_PATH] = {0};
            GetWindowText(hWindow, szName, MAX_PATH);
            BEATS_PRINT(_T("HCBT_CREATEWND: window handle: %d name:%s \n"), hWindow, szName);
        }

        break;
    case HCBT_DESTROYWND:
        {
            HWND hWindow = (HWND)wParam;
            TCHAR szName[MAX_PATH] = {0};
            GetWindowText(hWindow, szName, MAX_PATH);
            BEATS_PRINT(_T("HCBT_DESTROYWND: window handle: %d name:%s \n"), hWindow, szName);
            if (g_hLastInputHwnd == hWindow && g_hLastInputHwnd != NULL)
            {
                MSG simulateMSG;
                simulateMSG.hwnd = g_hLastInputHwnd;
                simulateMSG.message = eCMT_Flush;
                simulateMSG.time = 0;
                CSpyHookGetMessage::GetMessageProc(nCode, PM_REMOVE, (LPARAM)&simulateMSG);
                g_hLastInputHwnd = 0;
            }
        }
        break;
    case HCBT_SETFOCUS:
        {
            HWND hWindow = (HWND)wParam;
            TCHAR szName[MAX_PATH] = {0};
            GetWindowText(hWindow, szName, MAX_PATH);
            if (g_hLastInputHwnd != NULL)
            {
                ::PostMessage(g_hLastInputHwnd, eCMT_Flush, 0, 0);
            }
        }
        break;
    case HCBT_SYSCOMMAND:
        {
            int iSCCommandCode = (int)wParam;
            BEATS_PRINT(_T("HCBT_SYSCOMMAND: command code %d\n"), iSCCommandCode);
        }
        break;
    default:
        break;
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
