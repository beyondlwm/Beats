#include "stdafx.h"
#include "SpyHookBase.h"
#include "SpyManager.h"

CSpyHookBase::CSpyHookBase()
: m_bInit(false)
, m_hHookHandle(NULL)
, m_hModuleHandle(NULL)
{
    size_t eipForHandleFetch = 0;
    BEATS_ASSI_GET_EIP(eipForHandleFetch);
    m_bInit = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR) eipForHandleFetch, &m_hModuleHandle) == TRUE;
    BEATS_ASSERT(m_bInit);
}

CSpyHookBase::~CSpyHookBase()
{
    Stop();
}

bool CSpyHookBase::Start()
{
    bool bRet = false;
    if (m_bInit && m_hHookHandle == NULL)
    {
        bRet = StartImpl();
        if (bRet)
        {
            CSpyManager::GetInstance()->RegisterHook(this);
        }
    }
    return bRet;
}

bool CSpyHookBase::Stop()
{
    bool bRet = false;
    if (m_bInit && m_hHookHandle != NULL)
    {
        bRet = UnhookWindowsHookEx(m_hHookHandle) == TRUE;
        m_hHookHandle = NULL;
        if (bRet)
        {
            CSpyManager::GetInstance()->UnRegisterHook(this);
        }
    }
    return bRet;
}

