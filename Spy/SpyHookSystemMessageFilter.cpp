#include "stdafx.h"
#include "SpyHookSystemMessageFilter.h"

CSpyHookSystemMessageFilter::CSpyHookSystemMessageFilter()
{

}

CSpyHookSystemMessageFilter::~CSpyHookSystemMessageFilter()
{

}

bool CSpyHookSystemMessageFilter::StartImpl()
{
    m_hHookHandle = SetWindowsHookEx(WH_SYSMSGFILTER, &SystemMessageFilterProc, m_hModuleHandle, 0);
    return m_hHookHandle != NULL;
}

LRESULT CALLBACK CSpyHookSystemMessageFilter::SystemMessageFilterProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    MSG* pMsg = (MSG*)lParam;
    if (pMsg != NULL)
    {
        if (pMsg->message != NULL)
        {
            TCHAR szWndName[MAX_PATH] = {0};
            GetWindowText(pMsg->hwnd, szWndName, MAX_PATH);
            BEATS_PRINT(_T("SysMsgFilter window:%d name:%s message %d wParam %d lParam %d\n"), pMsg->hwnd, szWndName, pMsg->message, pMsg->wParam, pMsg->lParam);
        }
    }
    
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
