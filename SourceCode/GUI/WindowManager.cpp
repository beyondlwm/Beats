#include "stdafx.h"
#include "WindowManager.h"
#include "Renderer\BaseRenderer.h"
#include "Window\Window.h"
#include "Event\MouseEvent.h"

using namespace FCGUI;

WindowManager *WindowManager::m_pInstance = nullptr;

WindowManager::WindowManager()
    : _rootWindow(new Window(_T("Root")))
    , _focusedWindow(nullptr)
{
    _rootWindow->SetRenderer(new BaseRenderer());
}

WindowManager::~WindowManager()
{
    BEATS_SAFE_DELETE(_rootWindow);
}

Window *WindowManager::RootWindow() const
{
    return _rootWindow;
}

void WindowManager::SetFocusedWindow( Window *window )
{
    _focusedWindow = window;
}

Window *WindowManager::FocusedWindow() const
{
    return _focusedWindow;
}

bool WindowManager::OnMouseEvent( MouseEvent *event )
{
    if(_focusedWindow)
        _focusedWindow->OnMouseEvent(event);

    if(!event->Stopped())
    {
        if(event->Type() == EVENT_MOUSE_PRESSED)
        {
            _rootWindow->OnMouseEvent(event);
        }
    }
    return true;
}
