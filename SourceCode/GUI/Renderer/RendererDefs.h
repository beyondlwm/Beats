#ifndef GUI_RENDERER_RENDERERDEFS_H__INCLUDE
#define GUI_RENDERER_RENDERERDEFS_H__INCLUDE

namespace FCGUI
{
    enum RendererType
    {
        RENDERER_BASE,
        RENDERER_BUTTON,
        RENDERER_CHECKBOX,
        RENDERER_SLIDER,
        RENDERER_PROGRESS,
        RENDERER_TEXTEDIT,
        RENDERER_TEXTLABEL,
        RENDERER_LISTBOX,
        RENDERER_COMBOBOX,

        RENDERER_COUNT,
    };
}

#define DEFINE_RENDERER_TYPE(type) \
    public: \
        static const RendererType TYPE = type;   \
        virtual RendererType Type() const \
        {   \
            return TYPE;    \
        }   \
    private:

#endif