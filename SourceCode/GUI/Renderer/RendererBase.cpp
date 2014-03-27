#include "stdafx.h"
#include "RendererBase.h"
#include "GUI/Window/Window.h"
#include "Render/SpriteFrame.h"
#include "Render/SpriteFrameBatchManager.h"
#include "GUI/Event/WindowEvent.h"

using namespace FCGUI;

RendererBase::RendererBase()
    : _window(nullptr)
{
}

RendererBase::~RendererBase()
{
    for(auto layer : _layers)
    {
        delete layer;
    }
}

void RendererBase::SetWindow(Window *window)
{
    _window = window;

    auto windowEventHandler = std::bind1st(std::mem_fn(&RendererBase::onWindowEvent), this); 
    _window->SubscribeEvent(Window::EVENT_GUI_WINDOW_SIZED, windowEventHandler);
    _window->SubscribeEvent(Window::EVENT_GUI_WINDOW_MOVED, windowEventHandler);
    _window->SubscribeEvent(Window::EVENT_GUI_WINDOW_ROTATED, windowEventHandler);
    _window->SubscribeEvent(Window::EVENT_GUI_WINDOW_SCALED, windowEventHandler);
    _window->SubscribeEvent(Window::EVENT_GUI_WINDOW_ANCHOR_CHANGED, windowEventHandler);

    calcTransform(_window);
    setVertices(_window);
}

RendererType RendererBase::Type() const
{
    return RENDERER_BASE;
}

void RendererBase::Render(const kmMat4 &parentTransform) const
{
    if(!_window || !_window->Visible()) return;

    kmMat4 worldTransform;
    kmMat4Multiply(&worldTransform, &parentTransform, &_transform);

    for(auto layer : _layers)
    {
        CSpriteFrameBatchManager::GetInstance()->AddSpriteFrame(layer, worldTransform);
    }

    renderChildren(worldTransform);
}

void RendererBase::renderChildren(const kmMat4 &parentTransform) const
{
    BEATS_ASSERT(_window);

    _window->Traverse([&](Window *window){
        RendererBase *renderer = window->Renderer();
        if(renderer)
        {
            renderer->Render(parentTransform);
        }
    });    
}

void RendererBase::AddLayer(CSpriteFrame *layer)
{
    _layers.push_back(layer);
    if(_window)
    {
        layer->SetOriginSize(CPoint(_window->Anchor().x, _window->Anchor().y), 
            CSize(_window->RealSize().x, _window->RealSize().y));
    }
}

void RendererBase::onWindowEvent(EventBase *event)
{
    WindowEvent *eventWindow = static_cast<WindowEvent *>(event);
    switch(event->Type())
    {
    case Window::EVENT_GUI_WINDOW_MOVED:
        calcTransform(eventWindow->SourceWindow());
        break;
    case Window::EVENT_GUI_WINDOW_SIZED:
        setVertices(eventWindow->SourceWindow());
        break;
    case Window::EVENT_GUI_WINDOW_ROTATED:
        calcTransform(eventWindow->SourceWindow());
        break;
    case Window::EVENT_GUI_WINDOW_SCALED:
        calcTransform(eventWindow->SourceWindow());
        break;
    case Window::EVENT_GUI_WINDOW_ANCHOR_CHANGED:
        setVertices(eventWindow->SourceWindow());
        break;
    }
}

void RendererBase::calcTransform(const Window *window)
{
    BEATS_ASSERT(window);

    kmMat4 scale;
    kmMat4Identity(&scale);
    kmMat4Scaling(&scale, window->Scale().x, window->Scale().y, 0.f);
    kmMat4 rot;
    kmMat4Identity(&rot);
    kmMat4RotationZ(&rot, kmDegreesToRadians(window->Rotate()));
    kmMat4 trans;
    kmMat4Identity(&trans);
    kmMat4Translation(&trans, window->RealPos().x, window->RealPos().y, 0.f);
    kmMat4Multiply(&_transform, &rot, &scale);
    kmMat4Multiply(&_transform, &trans, &_transform);
}

void RendererBase::setVertices(const Window *window)
{
    BEATS_ASSERT(window);

    kmScalar anchorX = window->RealSize().x * window->Anchor().x;
    kmScalar anchorY = window->RealSize().y * window->Anchor().y;

    for(auto layer : _layers)
    {
        layer->SetOriginSize(CPoint(anchorX, anchorY), CSize(window->RealSize().x, window->RealSize().y));
    }
}
