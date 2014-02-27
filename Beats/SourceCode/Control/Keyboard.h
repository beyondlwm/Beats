#ifndef BEATS_CONTROL_KEYBOARD_H__INCLUDE
#define BEATS_CONTROL_KEYBOARD_H__INCLUDE

#include "KeyboardDef.h"
struct IDirectInputDevice8;
struct IDirectInput8;

class CKeyboard
{
    BEATS_DECLARE_SINGLETON(CKeyboard);
public:
    void Init(const HWND& hwnd, IDirectInput8* device);
    void Capture();
    bool IsKeyDown(const EKeyCode keyCode) const;
    EKeyState GetKeyState(const EKeyCode keyCode) const;

private:
    //! Bit field that holds status of Alt, Ctrl, Shift
    unsigned int m_funcKeyModifiers;
    IDirectInputDevice8* m_pKeyBoardDevice;
    char m_keyBuffer[256];
    char m_lastKeyBuffer[256];

};


#endif