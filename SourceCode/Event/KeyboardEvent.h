#ifndef EVENT_KEYBOARD_EVENT_H__INCLUDE
#define EVENT_KEYBOARD_EVENT_H__INCLUDE

#include "Event/EventBase.h"

class KeyboardEvent : public EventBase
{
public:
    KeyboardEvent(int type, int key, int mods = 0)
        : EventBase(type)
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

private:
    int _key;
    int _mods;
};

#endif