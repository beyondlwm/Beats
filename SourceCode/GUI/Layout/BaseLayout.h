#ifndef GUI_LAYOUT_BASE_LAYOUT_H__INCLUDE
#define GUI_LAYOUT_BASE_LAYOUT_H__INCLUDE

namespace FCGUI
{
    //forward declaration
    class Window;

    class BaseLayout
    {
    public:
        BaseLayout();

        virtual void SetWindow(Window *window);

        bool Invalidated() const;

        virtual void PerformLayout() = 0;

    protected:
        void invalidate();

    protected:
        Window *_window;
        bool _invalidated;
    };
}

#endif