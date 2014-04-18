#include "stdafx.h"
#include "BaseRenderer.h"
#include "GUI/Window/Window.h"
#include "Render/TextureFrag.h"
#include "Render/Texture.h"
#include "Render/SpriteFrameBatchManager.h"
#include "GUI/Event/WindowEvent.h"
#include "Render/TextureFragManager.h"

using namespace FCGUI;

BaseRenderer::BaseRenderer()
	: _window(nullptr)
{
}

BaseRenderer::~BaseRenderer()
{
    for(auto layers : _stateLayers)
    {
        for(auto layer : layers.second)
        {
            BEATS_SAFE_DELETE(layer);
        }
    }
}

void BaseRenderer::SetWindow(Window *window)
{
    _window = window;

	auto windowEventHandler = std::bind(&BaseRenderer::onWindowEvent, this, std::placeholders::_1); 
	_window->SubscribeEvent(Window::EVENT_SIZED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_MOVED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_ROTATED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_SCALED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_ANCHOR_CHANGED, windowEventHandler);

    setVertices(_window);
}

void BaseRenderer::AddLayer(CTextureFrag *layer, unsigned int state)
{
    _stateLayers[state].push_back(new RenderLayer(layer));
}

void BaseRenderer::AddLayer(const TString &textureFragName, unsigned int state)
{
    CTextureFrag *frag = CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName);
    BEATS_ASSERT(frag);
    AddLayer(frag, state);
}

void BaseRenderer::AddLayer(const RenderLayer::FrameList &frames, unsigned int state)
{
    _stateLayers[state].push_back(new RenderLayer(frames));
}

RenderLayer *BaseRenderer::GetLayer(size_t index, unsigned int state)
{
    RenderLayer *layer = nullptr;
    auto itr = _stateLayers.find(state);
    if(itr != _stateLayers.end())
    {
        if(index < itr->second.size())
        {
            layer = itr->second[index];
        }
    }
    return layer;
}

void BaseRenderer::Render(const kmMat4 &parentTransform) const
{
	if(!_window || !_window->IsVisible()) return;

	kmMat4 worldTransform;
	kmMat4Multiply(&worldTransform, &parentTransform, &_window->Transform());

    renderLayers(worldTransform);
	renderChildren(worldTransform);
}

void BaseRenderer::renderLayers( const kmMat4 &parentTransform ) const
{
    const LayerList *layers = nullptr;
    auto itr = _stateLayers.find(_window->CurrState());
    if(itr != _stateLayers.end() && !itr->second.empty())
    {
        layers = &itr->second;
    }
    else
    {
        auto itrNormal = _stateLayers.find(Window::STATE_NORMAL);
        if(itrNormal != _stateLayers.end())
        {
            layers = &itrNormal->second;
        }
    }
    if(layers)
    {
        for(auto layer : *layers)
        {
            CSpriteFrameBatchManager::GetInstance()->AddQuad(_quad, 
                layer->GetTextureFrag(), parentTransform);
        }
    }
}

void BaseRenderer::renderChildren(const kmMat4 &parentTransform) const
{
    BEATS_ASSERT(_window);

	_window->Traverse([&parentTransform](Window *window){
		BaseRenderer *renderer = window->Renderer();
		if(renderer)
		{
			renderer->Render(parentTransform);
		}
        return false;
	});	
}

void BaseRenderer::onWindowEvent(BaseEvent *event)
{
	WindowEvent *eventWindow = static_cast<WindowEvent *>(event);
	switch(event->Type())
	{
	case Window::EVENT_SIZED:
		setVertices(eventWindow->SourceWindow());
		break;
	case Window::EVENT_ANCHOR_CHANGED:
		setVertices(eventWindow->SourceWindow());
		break;
	}
}

void BaseRenderer::setVertices(const Window *window)
{
    BEATS_ASSERT(window);

    kmScalar anchorX = window->RealSize().x * window->Anchor().x;
    kmScalar anchorY = window->RealSize().y * window->Anchor().y;

    //set vertex
    _quad.tl.x = 0 - anchorX;
    _quad.tl.y = 0 - anchorY;
    _quad.tr.x = window->RealSize().x - anchorX;
    _quad.tr.y = _quad.tl.y;
    _quad.bl.x = _quad.tl.x;
    _quad.bl.y = window->RealSize().y - anchorY;
    _quad.br.x = _quad.tr.x;
    _quad.br.y = _quad.bl.y;
}
