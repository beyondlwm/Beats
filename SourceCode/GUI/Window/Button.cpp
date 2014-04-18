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

int Button::CurrState() const
{
    return IsEnabled() ? _state : STATE_DISABLED;
}

bool Button::OnMouseEvent( MouseEvent *event )
{
    bool result = false;
    if(IsEnabled())
    {
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

            result = true;
        }
    }

    return result;
}
