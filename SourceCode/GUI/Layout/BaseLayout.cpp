#include "stdafx.h"
#include "BaseLayout.h"

using namespace FCGUI;

BaseLayout::BaseLayout()
    : _window(nullptr)
    , _invalidated(true)
{

}

void BaseLayout::SetWindow(Window *window)
{
    _window = window;
}

bool BaseLayout::Invalidated() const
{
    return _invalidated;
}

void BaseLayout::PerformLayout()
{
    _invalidated = false;
}

void BaseLayout::invalidate()
{
    _invalidated = true;
}

