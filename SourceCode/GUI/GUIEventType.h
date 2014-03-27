#ifndef GUI_EVENT_TYPE_H__INCLUDE
#define GUI_EVENT_TYPE_H__INCLUDE

#include "Event\EventType.h"

enum GUIEventType
{
    EVENT_GUI_COMMON = EVENT_GUI + (1 << 16),
    EVENT_GUI_WINDOW = EVENT_GUI + (2 << 16),
};

#endif