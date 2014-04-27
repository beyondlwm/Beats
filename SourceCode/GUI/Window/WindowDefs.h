#ifndef GUI_WINDOW_WINDOWDEFS_H__INCLUDE
#define GUI_WINDOW_WINDOWDEFS_H__INCLUDE

namespace FCGUI
{
    enum WindowType
    {
        WINDOW_BASE,
        WINDOW_BUTTON,
        WINDOW_CHECKBOX,
        WINDOW_SLIDER,
        WINDOW_PROGRESS,
        WINDOW_TEXTEDIT,
        WINDOW_TEXTLABEL,
        WINDOW_LISTBOX,
        WINDOW_COMBOBOX,
        WINDOW_LISTCONTROL,
        
        WINDOW_COUNT,
    };
}

#define DEFINE_WINDOW_TYPE(type) \
    public: \
        static const WindowType TYPE = type;   \
        virtual WindowType Type() const \
        {   \
            return TYPE;    \
        }   \
    private:

#endif