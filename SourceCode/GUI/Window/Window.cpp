#include "stdafx.h"
#include "Window.h"
#include "GUI/System.h"
#include "GUI/Event/WindowEvent.h"
#include "GUI/Layout/BaseLayout.h"
#include "GUI/Behavior/BehaviorBase.h"
#include "GUI/Renderer/BaseRenderer.h"
#include "GUI/WindowManager.h"
#include "Event/MouseEvent.h"

using namespace FCGUI;

Window::Window(const TString &name)
    : _name(name)
{
    Init();
}

Window::~Window()
{
    BEATS_SAFE_DELETE(_behavior);
    BEATS_SAFE_DELETE(_renderer);

    for(auto child : _children)
    {
        BEATS_SAFE_DELETE(child.second);
    }
}

void Window::Init()
{
    _parent = nullptr;
    _behavior = nullptr;
    _renderer = nullptr;
    _layout = nullptr;

	kmVec2Fill(&_pos, 0.f, 0.f);
	kmVec2Fill(&_posPercent, 0.f, 0.f);
	kmVec2Fill(&_realPos, 0.f, 0.f);
	kmVec2Fill(&_size, 0.f, 0.f);
	kmVec2Fill(&_sizePercent, 0.f, 0.f);
	kmVec2Fill(&_realSize, 0.f, 0.f);
	kmVec2Fill(&_anchor, 0.f, 0.f);
	_rotation = 0.f;
	kmVec2Fill(&_scale, 1.f, 1.f);
	_visible = true;
}

void Window::SetPosSize(kmScalar x, kmScalar y, kmScalar xPercent, kmScalar yPercent, 
    kmScalar width, kmScalar height, kmScalar widthPercent, kmScalar heightPercent)
{
    SetPos(x, y);
    SetPosPercent(xPercent, yPercent);
    SetSize(width, height);
    SetSizePercent(widthPercent, heightPercent);
}

void Window::SetArea(kmScalar left, kmScalar top, kmScalar leftPercent, kmScalar topPercent, 
    kmScalar right, kmScalar bottom, kmScalar rightPercent, kmScalar bottomPercent)
{
    SetPos(left, top);
    SetPosPercent(leftPercent, topPercent);
    SetSize(right - left, bottom - top);
    SetSizePercent(rightPercent - leftPercent, bottomPercent - topPercent);
}

void Window::SetBehavior(BehaviorBase *behavior)
{
    _behavior = behavior;
}

BehaviorBase *Window::Behavior() const
{
	return _behavior;
}

void Window::SetRenderer(BaseRenderer *renderer)
{
    _renderer = renderer;
    renderer->SetWindow(this);
}

BaseRenderer *Window::Renderer() const
{
	return _renderer;
}

void Window::SetLayout(BaseLayout *layout)
{
    _layout = layout;
    layout->SetWindow(this);
}

BaseLayout *Window::Layout() const
{
    return _layout;
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

    calcTransform();

	WindowEvent event(EVENT_MOVED);
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

	WindowEvent event(EVENT_SIZED);
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

void Window::SetAnchor( kmScalar x, kmScalar y )
{
    _anchor.x = x;
    _anchor.y = y;

	WindowEvent event(EVENT_ANCHOR_CHANGED);
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

kmVec2 Window::RealAnchor() const
{
    kmVec2 realAnchor;
    kmVec2Fill(&realAnchor, _realSize.x * _anchor.x, _realSize.y * _anchor.y);
    return realAnchor;
}

void Window::SetRotation( kmScalar rotation )
{
	_rotation = rotation;

    calcTransform();

	WindowEvent event(EVENT_ROTATED);
	DispatchEvent(&event);
}

kmScalar Window::Rotation() const
{
	return _rotation;
}

void Window::SetScale( kmScalar scaleX, kmScalar scaleY )
{
    _scale.x = scaleX;
    _scale.y = scaleY;

    calcTransform();

	WindowEvent event(EVENT_SCALED);
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

void Window::calcTransform()
{
	kmMat4 scale;
	kmMat4Identity(&scale);
	kmMat4Scaling(&scale, _scale.x, _scale.y, 1.f);
	kmMat4 rot;
	kmMat4Identity(&rot);
	kmMat4RotationZ(&rot, kmDegreesToRadians(_rotation));
	kmMat4 trans;
	kmMat4Identity(&trans);
	kmMat4Translation(&trans, _realPos.x, _realPos.y, 0.f);
	kmMat4Multiply(&_transform, &rot, &scale);
	kmMat4Multiply(&_transform, &trans, &_transform);
}

const kmMat4 &Window::Transform() const
{
    return _transform;
}

kmMat4 Window::WorldTransform() const
{
    if(!_parent)
    {
        return _transform;
    }
    else
    {
        kmMat4 worldMat;
        kmMat4 parentWorldMat = _parent->WorldTransform();
        kmMat4Multiply(&worldMat, &_transform, &parentWorldMat);
        return worldMat;
    }
}

void Window::Localize(kmVec3 &pos) const
{
    if(_parent)
    {
        _parent->Localize(pos);
    }

    kmMat4 inverseMat;
    kmMat4Inverse(&inverseMat, &_transform);
    kmVec3Transform(&pos, &pos, &inverseMat);
}

bool Window::HitTest(kmScalar x, kmScalar y) const
{
    kmVec3 anchorPos;
    anchorPos.x = _realSize.x * _anchor.x;
    anchorPos.y = _realSize.y * _anchor.y;
    anchorPos.z = 0.f;

    kmVec3 hitPos;
    kmVec3Fill(&hitPos, x, y, 0.f);
    Localize(hitPos);
    
    return (hitPos.x >  -anchorPos.x)
        && (hitPos.x <  -anchorPos.x + _realSize.x)
        && (hitPos.y >  -anchorPos.y)
        && (hitPos.y <  -anchorPos.y + _realSize.y);
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

bool Window::OnMouseEvent( MouseEvent *event )
{
    if(HitTest(event->X(), event->Y()))
    {
        for(auto child : _children)
        {
            child.second->OnMouseEvent(event);
            if(event->Stopped())
            {
                return true;
            }
        }

        if(!event->Stopped())
        {
            DispatchEvent(event);
        }

        if(!event->Stopped() && event->Type() == EVENT_MOUSE_PRESSED)
        {
            WindowManager::GetInstance()->SetFocusedWindow(this);
            event->StopPropagation();
        }

        return true;
    }
    return false;
}

void Window::Update(float deltaTime)
{
    if(_layout && _layout->Invalidated())
    {
        _layout->PerformLayout();
    }

    for(auto child : _children)
    {
        child.second->Update(deltaTime);
    }
}

void Window::SetParent( Window *parent )
{
    if(_parent)
    {
        _parent->RemoveChild(this);
    }
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
