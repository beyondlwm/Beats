#ifndef GUI_WINDOW_WINDOW_FACTORY_H__INCLUDE
#define GUI_WINDOW_WINDOW_FACTORY_H__INCLUDE

namespace FCGUI
{
    class Window;

    class WindowFactory
    {
    public:
        virtual ~WindowFactory(){}
        virtual Window *Create(const TString &name) = 0;
    };

    template <typename T>
    class TplWindowFactory : public WindowFactory
    {
    public:
        virtual Window *Create(const TString &name) override
        {
            return new T(name);
        }
    };
}

#endif