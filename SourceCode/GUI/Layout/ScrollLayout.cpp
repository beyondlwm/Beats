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
    PerformLayout();
    return true;
}

bool ScrollLayout::AddChild( Window *window, kmScalar x, kmScalar y )
{
    return false;
}

bool ScrollLayout::AddChildToCell( Window *window, size_t row, size_t col )
{
    BEATS_ASSERT(col < _colCount);

    size_t index = row * _colCount + col;
    if(_children[index])
    {
        return false;
    }
    else
    {
        _children[index] = window;
        PerformLayout();
        return true;
    }
}

bool ScrollLayout::RemoveChild( size_t row, size_t col )
{
    return GridLayout::RemoveChild(row, col);
}
