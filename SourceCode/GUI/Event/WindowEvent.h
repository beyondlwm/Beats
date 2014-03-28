#ifndef GUI_EVENT_WINDOW_EVENT_H__INCLUDE
#define GUI_EVENT_WINDOW_EVENT_H__INCLUDE

#include "Event/BaseEvent.h"

namespace FCGUI
{
    class Window;

	class WindowEvent : public BaseEvent
	{
	public:
		WindowEvent(int type)
			: BaseEvent(type)
		{}

        Window *SourceWindow() const
        {
            return static_cast<Window *>(_source);
        }
	};
}

#endif