#include "stdafx.h"
#include "ButtonRenderer.h"
#include "Render\SpriteFrameBatchManager.h"

using namespace FCGUI;

ButtonRenderer::ButtonRenderer()
{
    _stateLayers.resize(Button::STATE_COUNT);
}

RendererType ButtonRenderer::Type() const
{
    return RENDERER_BUTTON;
}

void ButtonRenderer::AddLayer(CSpriteFrame *layer, std::set<Button::State> states)
{
    BaseRenderer::AddLayer(layer);
    for(auto state : states)
    {
        _stateLayers[state].insert(layer);
    }
}

void ButtonRenderer::AddLayer(CSpriteFrame *layer, Button::State state)
{
    BaseRenderer::AddLayer(layer);
    _stateLayers[state].insert(layer);
}

void ButtonRenderer::renderLayers( const kmMat4 &parentTransform ) const
{
    Button *btn = static_cast<Button *>(_window);
    auto *layers = &_stateLayers[btn->CurrState()];
    if(layers->empty()) //if no layers for current state, rendering as normal
        layers = &_stateLayers[Button::STATE_NORMAL];
    for(auto layer : *layers)
    {
        CSpriteFrameBatchManager::GetInstance()->AddSpriteFrame(layer, parentTransform);
    }
}
