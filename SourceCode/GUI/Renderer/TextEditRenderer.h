#ifndef GUI_RENDERER_TEXTEDIT_RENDERER_H__INCLUDE
#define GUI_RENDERER_TEXTEDIT_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
class CTextureFrag;
class FontFace;
namespace FCGUI
{
    class  TextEditRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_TEXTEDIT)
    public:
        TextEditRenderer();

    protected:
        virtual void renderLayers( const kmMat4 &parentTransform ) const override;
    };
}

#endif