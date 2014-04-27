#ifndef GUI_RENDERER_SLIDER_RENDERER_H__INCLUDE
#define GUI_RENDERER_SLIDER_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/Slider.h"
#include "GUI/Window/Button.h"
class CTextureFrag;

namespace FCGUI
{
    class  SliderRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_SLIDER)
    public:
        SliderRenderer();

        void AddLayer(CTextureFrag *thumbLayer, CTextureFrag *channelLayer, Slider::State state);
        void AddLayer(const TString &thumbFlagName, const TString &channelFlagName, Slider::State state);
    };
}

#endif