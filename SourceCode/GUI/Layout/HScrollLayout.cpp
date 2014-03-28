#include "stdafx.h"
#include "HScrollLayout.h"
#include "GUI/Window/Window.h"

using namespace FCGUI;

HScrollLayout::HScrollLayout( size_t colCount /*= 1*/ )
    : ScrollLayout(0, colCount)
    , _colWidth(0.f)
{

}

void HScrollLayout::SetColumnWidth( kmScalar colWidth )
{
    _colWidth = colWidth;
}

void HScrollLayout::PerformLayout()
{
    kmScalar cellWidth = _colWidth;
    if(cellWidth <= 0.f)
    {
        kmScalar spaceWidth = _leftMargin + _rightMargin + _vGap * (_rowCount - 1);
        cellWidth = (_window->RealSize().x - spaceWidth) / _rowCount;
    }
    kmScalar spaceHeight = _topMargin + _bottomMargin + _hGap * (_colCount - 1);
    kmScalar cellHeight = (_window->RealSize().y - spaceHeight) / _colCount;

    for(size_t i = 0; i < _children.size(); ++i)
    {
        size_t row = i / _colCount;
        size_t col = i % _colCount;
        Window *child = _children[i];
        if(child)
        {
            child->SetPos(_leftMargin + (_vGap + cellWidth) * row,
                _topMargin + (_hGap + cellHeight) * col);
            child->SetPosPercent(0.f, 0.f);
            child->SetSize(cellWidth, cellHeight);
            child->SetSizePercent(0.f, 0.f);
        }
    }
}

