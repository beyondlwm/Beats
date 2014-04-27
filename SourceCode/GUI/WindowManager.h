#ifndef GUI_WINDOW_MANAGER_H__INCLUDE
#define GUI_WINDOW_MANAGER_H__INCLUDE

#include "Window/WindowDefs.h"

//forward declaration
class MouseEvent;
class KeyboardEvent;

namespace FCGUI
{
    //forward declaration
    class Window;
    class WindowFactory;
    class RendererFactory;

    class WindowManager
    {
        BEATS_DECLARE_SINGLETON(WindowManager);
    public:
        Window *Create(const TString &name, WindowType type, Window *parent);
        template <typename WindowType>
        WindowType *Create(const TString &name, Window *parent);
        void DestroyWindow(Window *window);

        void registerWindowFactory(WindowType windowType, WindowFactory *windowFactory);
        void registerRendererFactory(WindowType windowType, RendererFactory *renderFactory);

		Window *RootWindow() const;
        void SetFocusedWindow(Window *window);
        Window *FocusedWindow() const;
        Window *GetWindow(kmScalar x, kmScalar y) const;
        size_t GetWindowCount() const;

        bool OnMouseEvent(MouseEvent *event);
        bool OnKeyboardEvent(KeyboardEvent *event);

    private:
        void registerDefaultWindowFactories();
        void registerDefaultRendererFactories();

    private:
        Window *_rootWindow;
        Window *_focusedWindow;
        std::map<WindowType, RendererFactory *> _rendererFactories;
        std::map<WindowType, WindowFactory *> _windowFactories;
    };

    template <typename WindowType>
    WindowType *WindowManager::Create(const TString &name, Window *parent)
    {
        return static_cast<WindowType *>(Create(name, WindowType::TYPE, parent));
    }
}

#endif