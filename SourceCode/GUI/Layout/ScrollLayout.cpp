#include "stdafx.h"
#include "ScrollLayout.h"
#include "GUI/Window/Window.h"

using namespace FCGUI;

ScrollLayout::ScrollLayout( size_t rowCount, size_t colCount )
    : GridLayout(rowCount, colCount)
{

}

bool ScrollLayout::AddChild( Window *window )
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());

    for(size_t i = 0; i < _children.size(); ++i)
    {
        if(!_children[i])
        {
            _children[i] = window;
            PerformLayout();
            return true;
        }
    }

    SetRowCount(_rowCount+1);
    _children[_children.size() - _colCount] = window;
    invalidate();
    return true;
}

bool ScrollLayout::AddChildToCell( Window *window, size_t row, size_t col )
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());

    size_t index = row * _colCount + col;
    if(index >= _children.size())
    {
        return AddChild(window);
    }

    if(_children[index])
    {
        if(_children.back())
        {
            SetRowCount(_rowCount+1);
        }
        for(size_t i = _children.size()-1; i > index; --i)
        {
            _children[i] = _children[i-1];
        }
    }
    _children[index] = window;
    invalidate();
    return true;
}

bool ScrollLayout::RemoveChild( Window *window )
{
    auto itr = std::find(_children.begin(), _children.end(), window);
    if(itr != _children.end())
    {
        for(auto itrNext = itr + 1; itrNext != _children.end(); ++itrNext, ++itr)
        {
            *itr = *itrNext;
        }
        *itr = nullptr;

        if(!_children[_children.size()-_colCount])
            SetRowCount(_rowCount-1);
        invalidate();
        return true;
    }
    return false;
}
