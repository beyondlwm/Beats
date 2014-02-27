#ifndef BEATS_CONTROL_INPUTMANAGER_H__INCLUDE
#define BEATS_CONTROL_INPUTMANAGER_H__INCLUDE

#include "InputPublicDef.h"
#include "../Function/Signal/Signal.h"

struct IDirectInput8;

class CInputManager
{
    typedef std::map<WORD, Signal<void(void)>*> TCallbackMap;
    typedef std::map<WORD, std::vector<Signal<void(void)>*>> TCallbackStack;
    BEATS_DECLARE_SINGLETON(CInputManager);
public:
    void Init(const HWND& hwnd, const HINSTANCE& instance);
    void Update();
    void CheckCallback();
    void Clear();

    void RegisterKeyBoardCallback(SKeyTriggerType type, const Function<void(void)>& function, bool exclusive);
    void UnRegisterKeyBoardCallback(SKeyTriggerType type, const Function<void(void)>& function);

private:
    void PopStack(WORD id);

private:
    IDirectInput8* m_pDirectInput;
    TCallbackMap m_callBacks;
    TCallbackStack m_stack;
};


#endif
