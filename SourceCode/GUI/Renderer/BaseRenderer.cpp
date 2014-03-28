#include "stdafx.h"
#include "BaseRenderer.h"
#include "GUI/Window/Window.h"
#include "Render/SpriteFrame.h"
#include "Render/SpriteFrameBatchManager.h"
#include "GUI/Event/WindowEvent.h"

using namespace FCGUI;

BaseRenderer::BaseRenderer()
	: _window(nullptr)
{
}

BaseRenderer::~BaseRenderer()
{
	for(auto layer : _layers)
	{
		delete layer;
	}
}

void BaseRenderer::SetWindow(Window *window)
{
    _window = window;

	auto windowEventHandler = std::bind1st(std::mem_fn(&BaseRenderer::onWindowEvent), this); 
	_window->SubscribeEvent(Window::EVENT_SIZED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_MOVED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_ROTATED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_SCALED, windowEventHandler);
	_window->SubscribeEvent(Window::EVENT_ANCHOR_CHANGED, windowEventHandler);

    setVertices(_window);
}

RendererType BaseRenderer::Type() const
{
	return RENDERER_BASE;
}

void BaseRenderer::Render(const kmMat4 &parentTransform) const
{
	if(!_window || !_window->Visible()) return;

	kmMat4 worldTransform;
	kmMat4Multiply(&worldTransform, &parentTransform, &_window->Transform());

    renderLayers(worldTransform);
	renderChildren(worldTransform);
}

void BaseRenderer::renderLayers(const kmMat4 &worldTransform) const
{
	for(auto layer : _layers)
	{
		CSpriteFrameBatchManager::GetInstance()->AddSpriteFrame(layer, worldTransform);
	}
}

void BaseRenderer::renderChildren(const kmMat4 &parentTransform) const
{
    BEATS_ASSERT(_window);

	_window->Traverse([&](Window *window){
		BaseRenderer *renderer = window->Renderer();
		if(renderer)
		{
			renderer->Render(parentTransform);
		}
	});	
}

void BaseRenderer::AddLayer(CSpriteFrame *layer)
{
	_layers.push_back(layer);

    if(_window)
    {
        kmScalar anchorX = _window->RealSize().x * _window->Anchor().x;
        kmScalar anchorY = _window->RealSize().y * _window->Anchor().y;

        layer->SetOriginSize(CPoint(anchorX, anchorY), 
            CSize(_window->RealSize().x, _window->RealSize().y));
    }
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

	for(auto layer : _layers)
	{
		layer->SetOriginSize(CPoint(anchorX, anchorY), CSize(window->RealSize().x, window->RealSize().y));
	}
}
