#include "stdafx.h"
#include "HScrollLayout.h"
#include "GUI/Window/Window.h"

using namespace FCGUI;

HScrollLayout::HScrollLayout( size_t colCount /*= 1*/ )
    : ScrollLayout(0, colCount)
{

}

bool HScrollLayout::AddChild( Window *window, kmScalar x, kmScalar y )
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());

    if(Invalidated())
        PerformLayout();
    
    kmVec2 realAnchor = _window->RealAnchor();
    x += realAnchor.x;
    y += realAnchor.y;

    int row = -1;
    for(size_t i = 0; i < _rowCount; ++i)
    {
        kmScalar left = _vSplitPos[2*i].second * _window->RealSize().x + _vSplitPos[2*i].first;
        kmScalar right = _vSplitPos[2*i+1].second * _window->RealSize().x + _vSplitPos[2*i+1].first;
        if(left <= x && x <= right)
        {
            row = i;
            break;
        }
    }

    int col = -1;
    for(size_t i = 0; i < _colCount; ++i)
    {
        kmScalar top = _hSplitPos[2*i].second * _window->RealSize().y + _hSplitPos[2*i].first;
        kmScalar bottom = _hSplitPos[2*i+1].second * _window->RealSize().y + _hSplitPos[2*i+1].first;
        if(top <= y && y <= bottom)
        {
            col = i;
            break;
        }
    }

	return row >= 0 && col >= 0 ? AddChildToCell(window, row, col) : ScrollLayout::AddChild(window);
}

void HScrollLayout::PerformLayout()
{
    calcSplitPos();

    //set position and size of child window
    for(size_t row = 0; row < _rowCount; ++row)
    {
        std::pair<kmScalar, kmScalar> left = _vSplitPos[2*row];
        std::pair<kmScalar, kmScalar> right = _vSplitPos[2*row+1];
        for(size_t col = 0; col < _colCount; ++col)
        {
            size_t index = row * _colCount + col;
            Window *child = _children[index];
            if(child)
            {
                std::pair<kmScalar, kmScalar> top = _hSplitPos[2*col];
                std::pair<kmScalar, kmScalar> bottom = _hSplitPos[2*col+1];
                child->SetArea(left.first, top.first, left.second, top.second,
                    right.first, bottom.first, right.second, bottom.second);
            }
        }
    }

    BaseLayout::PerformLayout();
}

