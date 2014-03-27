#ifndef GUI_EVENT_WINDOW_EVENT_H__INCLUDE
#define GUI_EVENT_WINDOW_EVENT_H__INCLUDE

#include "Event/EventBase.h"

namespace FCGUI
{
    class Window;

    class WindowEvent : public EventBase
    {
    public:
        WindowEvent(int type, Window *window)
            : EventBase(type)
            , _window(window)
        {}

        Window *SourceWindow() const
        {
            return _window;
        }

    private:
        Window *_window;
    };
}

#endif