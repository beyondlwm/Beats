#ifndef GUI_WINDOW_BUTTON_H__INCLUDE
#define GUI_WINDOW_BUTTON_H__INCLUDE

#include "Window.h"

namespace FCGUI
{
    class Button : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_BUTTON);
    public:
        enum EventType
        {
            EVENT_BUTTON = FCGUI::EVENT_BUTTON,
            EVENT_CLICKED,
        };

        enum State
        {
            STATE_NORMAL,
            STATE_PRESSED,
            STATE_DISABLED,

            STATE_COUNT,
        };

    public:
        Button(const TString &name);

        virtual int CurrState() const override;

        virtual bool OnMouseEvent( MouseEvent *event ) override;

    protected:
        State _state;
    };
}

#endif