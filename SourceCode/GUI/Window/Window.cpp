#include "stdafx.h"
#include "Window.h"
#include "GUI/System.h"
#include "GUI/Event/WindowEvent.h"
#include "GUI/Behavior/BehaviorBase.h"
#include "GUI/Renderer/RendererBase.h"

using namespace FCGUI;

Window::Window()
{
    Init();
}

Window::Window(const TString &name)
{
    Init();
    _name = name;
}

Window::Window(const TString &name, kmScalar x, kmScalar y, kmScalar xPercent, kmScalar yPercent, 
               kmScalar width, kmScalar height, kmScalar widthPercent, kmScalar heightPercent)
{
    Init();
    _name = name;
    SetPos(x, y);
    SetPosPercent(xPercent, yPercent);
    SetSize(width, height);
    SetSizePercent(widthPercent, heightPercent);
}

Window::~Window()
{
    delete _behavior;
    delete _renderer;
}

void Window::Init()
{
    _parent = nullptr;
    _behavior = nullptr;
    _renderer = nullptr;

    kmVec2Fill(&_pos, 0.f, 0.f);
    kmVec2Fill(&_posPercent, 0.f, 0.f);
    kmVec2Fill(&_realPos, 0.f, 0.f);
    kmVec2Fill(&_size, 0.f, 0.f);
    kmVec2Fill(&_sizePercent, 0.f, 0.f);
    kmVec2Fill(&_realSize, 0.f, 0.f);
    kmVec2Fill(&_anchor, 0.f, 0.f);
    _rotate = 0.f;
    kmVec2Fill(&_scale, 1.f, 1.f);
    _visible = true;
}

void Window::SetBehavior(BehaviorBase *behavior)
{
    _behavior = behavior;
}

BehaviorBase *Window::Behavior() const
{
    return _behavior;
}

void Window::SetRenderer(RendererBase *renderer)
{
    _renderer = renderer;
    renderer->SetWindow(this);
}

RendererBase *Window::Renderer() const
{
    return _renderer;
}

void Window::SetName(const TString &name)
{
    _name = name;
}

TString Window::Name() const
{
    return _name;
}

void Window::SetPos( kmScalar x, kmScalar y )
{
    _pos.x = x;
    _pos.y = y;
    calcRealPos();
}

void Window::SetPos( kmVec2 pos )
{
    SetPos(pos.x, pos.y);
}

kmVec2 Window::Pos() const
{
    return _pos;
}

void Window::SetPosPercent( kmScalar x, kmScalar y )
{
    _posPercent.x = x;
    _posPercent.y = y;
    calcRealPos();
}

void Window::SetPosPercent( kmVec2 posPercent )
{
    SetPosPercent(posPercent.x, posPercent.y);
}

kmVec2 Window::PosPercent() const
{
    return _posPercent;
}

void Window::calcRealPos()
{
    if(!_parent)
    {
        _realPos = _pos;
    }
    else
    {
        kmVec2 parentSize = _parent->RealSize();
        kmVec2 parentAnchor;
        parentAnchor.x = parentSize.x * _parent->Anchor().x;
        parentAnchor.y = parentSize.y * _parent->Anchor().y;
        _realPos.x = parentSize.x * _posPercent.x + _pos.x - parentAnchor.x;
        _realPos.y = parentSize.y * _posPercent.y + _pos.y - parentAnchor.y;
    }

    WindowEvent event(EVENT_GUI_WINDOW_MOVED, this);
    DispatchEvent(&event);
}

kmVec2 Window::RealPos() const
{
    return _realPos;
}

void Window::SetSize( kmScalar width, kmScalar height )
{
    _size.x = width;
    _size.y = height;
    calcRealSize();
}

void Window::SetSize( kmVec2 size )
{
    SetSize(size.x, size.y);
}

kmVec2 Window::Size() const
{
    return _size;
}

void Window::SetSizePercent( kmScalar width, kmScalar height )
{
    _sizePercent.x = width;
    _sizePercent.y = height;
    calcRealSize();
}

void Window::SetSizePercent( kmVec2 sizePercent )
{
    SetSizePercent(sizePercent.x, sizePercent.y);
}

kmVec2 Window::SizePercent() const
{
    return _sizePercent;
}

void Window::calcRealSize()
{
    if(!_parent)
    {
        _realSize = _size;
    }
    else
    {
        kmVec2 parentSize = _parent->RealSize();
        _realSize.x = parentSize.x * _sizePercent.x + _size.x;
        _realSize.y = parentSize.y * _sizePercent.y + _size.y;
    }

    WindowEvent event(EVENT_GUI_WINDOW_SIZED, this);
    DispatchEvent(&event);

    for(auto child : _children)
    {
        child.second->OnParentSized();
    }
}
kmVec2 Window::RealSize() const
{
    return _realSize;
}

void Window::SetArea(kmScalar left, kmScalar top, kmScalar leftPercent, kmScalar topPercent, 
                     kmScalar right, kmScalar rightPercent, kmScalar bottom, kmScalar bottomPercent)
{
    SetPos(left, top);
    SetPosPercent(leftPercent, topPercent);
    SetSize(right - left, bottom - top);
    SetSizePercent(rightPercent - leftPercent, bottomPercent - topPercent);
}

void Window::SetAnchor( kmScalar x, kmScalar y )
{
    _anchor.x = x;
    _anchor.y = y;

    WindowEvent event(EVENT_GUI_WINDOW_ANCHOR_CHANGED, this);
    DispatchEvent(&event);
}

void Window::SetAnchor( kmVec2 anchor )
{
    SetAnchor(anchor.x, anchor.y);
}

kmVec2 Window::Anchor() const
{
    return _anchor;
}

void Window::SetRotate( kmScalar rotate )
{
    _rotate = rotate;

    WindowEvent event(EVENT_GUI_WINDOW_ROTATED, this);
    DispatchEvent(&event);
}

kmScalar Window::Rotate() const
{
    return _rotate;
}

void Window::SetScale( kmScalar scaleX, kmScalar scaleY )
{
    _scale.x = scaleX;
    _scale.y = scaleY;

    WindowEvent event(EVENT_GUI_WINDOW_SCALED, this);
    DispatchEvent(&event);
}

void Window::SetScale( kmVec2 scale )
{
    SetScale(scale.x, scale.y);
}

kmVec2 Window::Scale() const
{
    return _scale;
}

void Window::Show()
{
    _visible = true;
}

void Window::Hide()
{
    _visible = false;
}

void Window::ToggleVisible()
{
    _visible = !_visible;
}

bool Window::Visible()
{
    return _visible;
}

void Window::OnParentSized()
{
    calcRealPos();
    calcRealSize();
}

void Window::SetParent( Window *parent )
{
    _parent = parent;
    calcRealPos();
    calcRealSize();
}

void Window::AddChild( Window *window )
{
    BEATS_ASSERT(window);
    BEATS_ASSERT(!GetChild(window->Name()), _T("You can't add a window twice."));

    _children[window->Name()] = window;
    window->SetParent(this);
}

void Window::RemoveChild( Window *window )
{
    BEATS_ASSERT(window);
    RemoveChild(window->Name());
}

void Window::RemoveChild( const TString &name )
{
    auto itr = _children.find(name);
    if(itr != _children.end())
    {
        itr->second->SetParent(nullptr);
        _children.erase(itr);
    }
}

void Window::DestroyChild(Window *window)
{
    if(window)
    {
        DestroyChild(window->Name());
    }
}

void Window::DestroyChild(const TString &name)
{
    auto itr = _children.find(name);    
    if(itr != _children.end())
    {
        delete itr->second;
        _children.erase(itr);
    }
}

Window *Window::GetChild( const TString &name, bool recursively ) const
{
    Window* pRet = nullptr;
    auto itr = _children.find(name);    
    if(itr != _children.end())
    {
        pRet = itr->second;
    }
    else if(recursively)
    {
        for(auto child : _children)
        {
            Window *window = child.second->GetChild(name);
            if(window)
            {
                pRet = window;
                break;
            }
        }
    }
    return pRet;
}
