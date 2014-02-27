#ifndef BEATS_SPY_HOOK_SPYHOOKSYSTEMMESSAGEFILTER_H__INCLUDE
#define BEATS_SPY_HOOK_SPYHOOKSYSTEMMESSAGEFILTER_H__INCLUDE

#include "SpyHookBase.h"

class CSpyHookSystemMessageFilter : public CSpyHookBase
{
public:
    CSpyHookSystemMessageFilter();
    virtual ~CSpyHookSystemMessageFilter();

    virtual bool StartImpl();

    static LRESULT CALLBACK SystemMessageFilterProc(int nCode, WPARAM wParam, LPARAM lParam);

};
#endif