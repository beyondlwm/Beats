#ifndef BEATS_SPY_HOOK_SPYHOOKBASE_H__INCLUDE
#define BEATS_SPY_HOOK_SPYHOOKBASE_H__INCLUDE

enum ECustomMsgType
{
    eCMT_Flush = WM_USER + 1,
};

class CSpyHookBase
{
public:
    CSpyHookBase();
    virtual ~CSpyHookBase();

    bool Start();
    bool Stop();

    virtual bool StartImpl() = 0;


protected:
    bool m_bInit;
    HHOOK m_hHookHandle;
    HMODULE m_hModuleHandle;
};

#endif