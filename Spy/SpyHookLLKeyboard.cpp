#include "stdafx.h"
#include "SpyHookLLKeyboard.h"

CSpyHookLLKeyboard::CSpyHookLLKeyboard()
{

}

CSpyHookLLKeyboard::~CSpyHookLLKeyboard()
{

}

LRESULT CALLBACK CSpyHookLLKeyboard::LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* pData = (KBDLLHOOKSTRUCT*)(lParam);
        BEATS_PRINT(_T("vkCode %d scanCode %d flags %d time %d extra %x\n"), pData->vkCode, pData->scanCode, pData->flags, pData->time, pData->dwExtraInfo);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool CSpyHookLLKeyboard::StartImpl()
{
    m_hHookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, m_hModuleHandle, 0);
    return m_hHookHandle != NULL;
}