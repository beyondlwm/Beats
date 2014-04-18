#ifndef GUI_RENDERER_LISTBOX_RENDERER_H__INCLUDE
#define GUI_RENDERER_LISTBOX_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/CheckBox.h"

class CTextureFrag;

namespace FCGUI
{
    class  ListBoxRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_CHECKBOX)
    public:
        void  AddLayer(const TString &backTextName, const TString &selTextName);
        void  AddLayer(CTextureFrag* backLayer, CTextureFrag *selLayer);

    protected:
        virtual void renderLayers( const kmMat4 &parentTransform ) const override;

    };
}

#endif
