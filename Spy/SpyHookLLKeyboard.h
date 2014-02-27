#ifndef BEATS_SPY_HOOK_SPYHOOKLLKEYBOARD_H__INCLUDE
#define BEATS_SPY_HOOK_SPYHOOKLLKEYBOARD_H__INCLUDE

#include "SpyHookBase.h"

class CSpyHookLLKeyboard : public CSpyHookBase
{
public:
    CSpyHookLLKeyboard();
    virtual ~CSpyHookLLKeyboard();

    virtual bool StartImpl();

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

};
#endif