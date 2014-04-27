#ifndef GUI_RENDERER_TEXTLABEL_RENDERER_H__INCLUDE
#define GUI_RENDERER_TEXTLABEL_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
class CTextureFrag;

namespace FCGUI
{
    class  TextLabelRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_TEXTLABEL)
    public:
        TextLabelRenderer();

    protected:
        virtual void renderLayers( const kmMat4 &parentTransform ) const override;
    };
}

#endif