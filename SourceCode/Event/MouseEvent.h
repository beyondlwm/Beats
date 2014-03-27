#ifndef EVENT_MOUSE_EVENT_H__INCLUDE
#define EVENT_MOUSE_EVENT_H__INCLUDE

#include "Event/EventBase.h"

class MouseEvent : public EventBase
{
public:
    MouseEvent(int type, int button, char buttonState, kmScalar x = 0.f, kmScalar y = 0.f)
        : EventBase(type)
        , _button(button)
        , _buttonState(buttonState)
        , _x(x)
        , _y(y)
    {
    }

    int Button() const
    {
        return _button;
    }

    bool IsButtonDown(int button) const
    {
        return ((_buttonState >> button) & 1) != 0;
    }

    kmScalar X() const
    {
        return _x;
    }

    kmScalar Y() const
    {
        return _y;
    }

private:
    int _button;
    char _buttonState;
    kmScalar _x, _y;
};

#endif