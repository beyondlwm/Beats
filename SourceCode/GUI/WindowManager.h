#ifndef GUI_WINDOW_MANAGER_H__INCLUDE
#define GUI_WINDOW_MANAGER_H__INCLUDE

//forward declaration
class MouseEvent;

namespace FCGUI
{
    //forward declaration
    class Window;

    class WindowManager
    {
        BEATS_DECLARE_SINGLETON(WindowManager);
    public:
        template <typename WindowType>
        WindowType *Create(const TString &name, Window *parent);

		Window *RootWindow() const;
        void SetFocusedWindow(Window *window);
        Window *FocusedWindow() const;

        bool OnMouseEvent(MouseEvent *event);

    private:
        Window *_rootWindow;
        Window *_focusedWindow;
    };

    template <typename WindowType>
    WindowType *WindowManager::Create(const TString &name, Window *parent)
    {
        WindowType *wnd = new WindowType(name);
        if(!parent)
        {
            parent = _rootWindow;
        }
        parent->AddChild(wnd);
        return wnd;
    }

}

#endif