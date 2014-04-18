#ifndef EVENT_KEYBOARD_EVENT_H__INCLUDE
#define EVENT_KEYBOARD_EVENT_H__INCLUDE

#include "Event/BaseEvent.h"

class KeyboardEvent : public BaseEvent
{
public:
    KeyboardEvent(int type, int key, int mods = 0)
        : BaseEvent(type)
        , _key(key)
        , _mods(mods)
    {}

    int Key() const
    {
        return _key;
    }

    bool IsControlKeyDown(int ctrlKey) const
    {
        return (_mods & ctrlKey) != 0;
    }

    char Char() const
    {
        return static_cast<char>(_key);
    }

    wchar_t WChar() const
    {
        return static_cast<wchar_t>(_key);
    }

private:
    int _key;
    int _mods;
};

#endif