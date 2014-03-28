#ifndef GUI_RENDERER_BUTTON_RENDERER_H__INCLUDE
#define GUI_RENDERER_BUTTON_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI\Window\Button.h"

class CSpriteFrame;

namespace FCGUI
{
    class ButtonRenderer : public BaseRenderer
    {
    public:
        ButtonRenderer();

        virtual RendererType Type() const override;

        void AddLayer(CSpriteFrame *layer, std::set<Button::State> states);
        void AddLayer(CSpriteFrame *layer, Button::State state);

    protected:
        virtual void renderLayers( const kmMat4 &parentTransform ) const override;

    protected:
        std::vector<std::set<CSpriteFrame *> > _stateLayers;
    };
}

#endif