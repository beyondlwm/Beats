#include "stdafx.h"
#include "Button.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"

using namespace FCGUI;

Button::Button( const TString &name )
    : Window(name)
    , _state(STATE_NORMAL)
{

}

void Button::Disable()
{
    _state = STATE_DISABLED;
}

void Button::Enable()
{
    _state = STATE_NORMAL;
}

Button::State Button::CurrState() const
{
    return _state;
}

bool Button::OnMouseEvent( MouseEvent *event )
{
    if(_state == STATE_DISABLED)
    {
        return false;
    }

    if(Window::OnMouseEvent(event))
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_PRESSED)
            {
                _state = STATE_PRESSED;
            }
            else if(event->Type() == EVENT_MOUSE_RELEASED)
            {
                if(_state == STATE_PRESSED)
                {
                    _state = STATE_NORMAL;

                    WindowEvent event(EVENT_CLICKED);
                    DispatchEvent(&event);
                }
            }
        }

        return true;
    }
    return false;
}
