#include "stdafx.h"
#include "VScrollLayout.h"
#include "GUI/Window/Window.h"

using namespace FCGUI;

VScrollLayout::VScrollLayout( size_t colCount /*= 1*/ )
    : ScrollLayout(0, colCount)
    , _rowHeight(0.f)
{

}

void VScrollLayout::SetRowHeight( kmScalar rowHeight )
{
    _rowHeight = rowHeight;
}

void VScrollLayout::PerformLayout()
{
    kmScalar spaceWidth = _leftMargin + _rightMargin + _vGap * (_colCount - 1);
    kmScalar cellWidth = (_window->RealSize().x - spaceWidth) / _colCount;
    kmScalar cellHeight = _rowHeight;
    if(cellHeight <= 0.f)
    {
        kmScalar spaceHeight = _topMargin + _bottomMargin + _hGap * (_rowCount - 1);
        cellHeight = (_window->RealSize().y - spaceHeight) / _rowCount;
    }

    for(size_t i = 0; i < _children.size(); ++i)
    {
        size_t row = i / _colCount;
        size_t col = i % _colCount;
        Window *child = _children[i];
        if(child)
        {
            child->SetPos(_leftMargin + (_vGap + cellWidth) * col,
                _topMargin + (_hGap + cellHeight) * row);
            child->SetPosPercent(0.f, 0.f);
            child->SetSize(cellWidth, cellHeight);
            child->SetSizePercent(0.f, 0.f);
        }
    }
}

