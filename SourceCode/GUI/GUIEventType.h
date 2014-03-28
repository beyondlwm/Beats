#ifndef GUI_EVENT_TYPE_H__INCLUDE
#define GUI_EVENT_TYPE_H__INCLUDE

#include "Event\EventType.h"

namespace FCGUI
{
    enum GUIEventType
    {
        EVENT_COMMON = EVENT_GUI + (1 << 16),
        EVENT_WINDOW = EVENT_GUI + (2 << 16),
        EVENT_BUTTON = EVENT_GUI + (3 << 16),
    };
}

#endif