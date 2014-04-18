#include "stdafx.h"
#include "WindowManager.h"
#include "Window/WindowFactory.h"
#include "Window/Window.h"
#include "Window/Button.h"
#include "Window/CheckBox.h"
#include "Window/Slider.h"
#include "Window/Progress.h"
#include "Window/TextEdit.h"
#include "Window/TextLabel.h"
#include "Window/ListBox.h"
#include "Window/ComboBox.h"
#include "Window/ListControl.h"
#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "Renderer/RendererFactory.h"
#include "Renderer/BaseRenderer.h"
#include "Renderer/CheckBoxRenderer.h"
#include "Renderer/SliderRenderer.h"
#include "Renderer/ProgressRenderer.h"
#include "Renderer/TextEditRenderer.h"
#include "Renderer/TextLabelRenderer.h"
#include "Renderer/ListBoxRenderer.h"
#include "Renderer/ComboBoxRenderer.h"
using namespace FCGUI;

WindowManager *WindowManager::m_pInstance = nullptr;

WindowManager::WindowManager()
    : _focusedWindow(nullptr)
    , _rootWindow(nullptr)
{
    registerDefaultWindowFactories();
    registerDefaultRendererFactories();
    _rootWindow = Create(_T("Root"), WINDOW_BASE, nullptr);
}

WindowManager::~WindowManager()
{
    BEATS_SAFE_DELETE(_rootWindow);
    for(auto windowFactory : _windowFactories)
    {
        BEATS_SAFE_DELETE(windowFactory.second);
    }
    for(auto rendererFactory : _rendererFactories)
    {
        BEATS_SAFE_DELETE(rendererFactory.second);
    }
}

Window *WindowManager::Create(const TString &name, WindowType type, Window *parent)
{
    Window *window = nullptr;
    auto itr = _windowFactories.find(type);
    BEATS_ASSERT(itr != _windowFactories.end());
    if(itr != _windowFactories.end())
    {
        window = itr->second->Create(name);

        auto itr = _rendererFactories.find(window->Type());
        if(itr != _rendererFactories.end())
        {
            window->SetRenderer(itr->second->Create());
        }

        if(!parent && _rootWindow)
        {
            parent = _rootWindow;
        }
        if(parent)
            parent->AddChild(window);
    }

    return window;
}

void WindowManager::DestroyWindow(Window *window)
{
    BEATS_ASSERT(window->Parent());
    
    window->Parent()->DestroyChild(window);
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

Window *WindowManager::GetWindow(kmScalar x, kmScalar y) const
{
    BEATS_ASSERT(_rootWindow);
    Window *result = _rootWindow->GetChild(x, y);
    return result;
}

size_t WindowManager::GetWindowCount() const
{
    size_t windowCount = 1;
    std::function<bool(Window *)> counter = [&windowCount, &counter](Window *child){
        ++windowCount;
        child->Traverse(counter);
        return false;
    };
    _rootWindow->Traverse(counter);
    return windowCount;
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

bool WindowManager::OnKeyboardEvent(KeyboardEvent *event)
{
    if(_focusedWindow)
        _focusedWindow->OnKeyboardEvent(event);
    return true;
}

void WindowManager::registerWindowFactory( WindowType windowType, WindowFactory *windowFactory )
{
    auto itr = _windowFactories.find(windowType);
    if(itr != _windowFactories.end())
    {
        BEATS_SAFE_DELETE(itr->second);
        itr->second = windowFactory;
    }
    _windowFactories[windowType] = windowFactory;
}

void WindowManager::registerRendererFactory( WindowType windowType, RendererFactory *renderFactory )
{
    auto itr = _rendererFactories.find(windowType);
    if(itr != _rendererFactories.end())
    {
        BEATS_SAFE_DELETE(itr->second);
        itr->second = renderFactory;
    }
    _rendererFactories[windowType] = renderFactory;
}

void WindowManager::registerDefaultWindowFactories()
{
    registerWindowFactory(WINDOW_BASE, new TplWindowFactory<Window>());
    registerWindowFactory(WINDOW_BUTTON, new TplWindowFactory<Button>());
    registerWindowFactory(WINDOW_CHECKBOX, new TplWindowFactory<CheckBox>());
    registerWindowFactory(WINDOW_SLIDER, new TplWindowFactory<Slider>());
    registerWindowFactory(WINDOW_PROGRESS, new TplWindowFactory<Progress>());
    registerWindowFactory(WINDOW_TEXTEDIT, new TplWindowFactory<TextEdit>());
    registerWindowFactory(WINDOW_TEXTLABEL, new TplWindowFactory<TextLabel>());
    registerWindowFactory(WINDOW_LISTBOX, new TplWindowFactory<ListBox>());
    registerWindowFactory(WINDOW_COMBOBOX, new TplWindowFactory<ComboBox>());
    registerWindowFactory(WINDOW_LISTCONTROL, new TplWindowFactory<ListControl>());
}

void WindowManager::registerDefaultRendererFactories()
{
    registerRendererFactory(WINDOW_BASE, new TplRendererFactory<BaseRenderer>());
    registerRendererFactory(WINDOW_BUTTON, new TplRendererFactory<BaseRenderer>());
    registerRendererFactory(WINDOW_CHECKBOX, new TplRendererFactory<CheckBoxRenderer>());
    registerRendererFactory(WINDOW_SLIDER, new TplRendererFactory<SliderRenderer>());
    registerRendererFactory(WINDOW_PROGRESS, new TplRendererFactory<ProgressRenderer>());
    registerRendererFactory(WINDOW_TEXTEDIT, new TplRendererFactory<TextEditRenderer>());
    registerRendererFactory(WINDOW_TEXTLABEL, new TplRendererFactory<TextLabelRenderer>());
    registerRendererFactory(WINDOW_LISTBOX, new TplRendererFactory<ListBoxRenderer>());
    registerRendererFactory(WINDOW_COMBOBOX, new TplRendererFactory<ComboBoxRenderer>());
    registerRendererFactory(WINDOW_LISTCONTROL, new TplRendererFactory<BaseRenderer>());
}

