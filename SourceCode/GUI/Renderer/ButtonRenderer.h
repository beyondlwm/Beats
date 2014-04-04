#ifndef GUI_RENDERER_BUTTON_RENDERER_H__INCLUDE
#define GUI_RENDERER_BUTTON_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/Button.h"

class CTextureFrag;

namespace FCGUI
{
    class ButtonRenderer : public BaseRenderer
    {
    public:
        ButtonRenderer();

        virtual RendererType Type() const override;

        void AddLayer(CTextureFrag *layer, std::set<Button::State> states);
        void AddLayer(CTextureFrag *layer, Button::State state);
		void AddLayer(const TString &textureFragName, Button::State state);

    protected:
        virtual void renderLayers( const kmMat4 &parentTransform ) const override;

    protected:
        std::vector<std::set<CTextureFrag *> > _stateLayers;
    };
}

#endif