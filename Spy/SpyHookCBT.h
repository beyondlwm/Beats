#ifndef BEATS_SPY_HOOK_SPYHOOKCBT_H__INCLUDE
#define BEATS_SPY_HOOK_SPYHOOKCBT_H__INCLUDE

#include "SpyHookBase.h"

class CSpyHookCBT : public CSpyHookBase
{
public:
    CSpyHookCBT();
    virtual ~CSpyHookCBT();

    virtual bool StartImpl();

    static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

};
#endif