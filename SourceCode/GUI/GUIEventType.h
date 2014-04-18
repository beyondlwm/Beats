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
        EVENT_CHECKBOX = EVENT_GUI + (4 << 16),
        EVENT_SLIDER = EVENT_GUI + (5 << 16),
        EVENT_TEXTEDIT = EVENT_GUI + (6 << 16),
        EVENT_TEXTLABEL = EVENT_GUI + (7 << 16),
        EVENT_LISTBOX = EVENT_GUI + (8 << 16),
        EVENT_COMBOBOX = EVENT_GUI + (9 << 16),
        EVENT_LISTCONTROL = EVENT_GUI + (10 << 16),
    };
}

#endif