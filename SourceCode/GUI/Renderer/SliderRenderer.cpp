#include "stdafx.h"
#include "SliderRenderer.h"
#include "BaseRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
#include "Render/TextureFrag.h"
using namespace FCGUI;

SliderRenderer::SliderRenderer()
{
   
}

void SliderRenderer::AddLayer(const TString &thumbFlagName, const TString &channelFlagName, Slider::State state)
{
    Slider *slider = static_cast<Slider *>(_window);
    BaseRenderer::AddLayer(channelFlagName,state);

    Button *subThumb = slider->GetSubThumb();
    BaseRenderer* sliderrender = static_cast<BaseRenderer*>(subThumb->Renderer());
    sliderrender->AddLayer(thumbFlagName,state);
}

void SliderRenderer::AddLayer(CTextureFrag *thumbLayer, CTextureFrag *channelLayer, Slider::State state)
{
    Slider *slider = static_cast<Slider *>(_window);
    BaseRenderer::AddLayer(channelLayer,state);

    Button *subThumb = slider->GetSubThumb();
    BaseRenderer* sliderrender = static_cast<BaseRenderer*>(subThumb->Renderer());
    sliderrender->AddLayer(thumbLayer,state);
}




