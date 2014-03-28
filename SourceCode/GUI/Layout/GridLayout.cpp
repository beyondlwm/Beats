#include "stdafx.h"
#include "GridLayout.h"
#include "GUI/Window/Window.h"

using namespace FCGUI;

GridLayout::GridLayout( size_t rowCount, size_t colCount )
    : _rowCount(rowCount)
    , _colCount(colCount)
    , _topMargin(0.f)
    , _bottomMargin(0.f)
    , _leftMargin(0.f)
    , _rightMargin(0.f)
    , _hGap(0.f)
    , _vGap(0.f)
    , _colWidth(0.f)
    , _rowHeight(0.f)
{
    _children.resize(rowCount*colCount);
    _everyRowHeight.resize(rowCount);
    _everyColWidth.resize(colCount);
}

void GridLayout::SetRowCount( size_t rowCount )
{
    _children.resize(rowCount*_colCount);
    _everyRowHeight.resize(rowCount);
    _rowCount = rowCount;
    invalidate();
}

size_t GridLayout::RowCount() const
{
    return _rowCount;
}

void GridLayout::SetColCount( size_t colCount )
{
    if(colCount > _colCount)
    {
        _children.resize(_rowCount * colCount);
        //column count changed, move the existed item to corresponding position
        for(int i = _rowCount*_colCount-1; i >= 0; --i)
        {
            if(_children[i])
            {
                size_t row = i / _colCount;
                size_t col = i % _colCount;
                int newidx = row*colCount + col;
                if(newidx != i)
                {
                    _children[newidx] = _children[i];
                    _children[i] = nullptr;
                }
            }
        }
        _colCount = colCount;
        _everyColWidth.resize(colCount);
        invalidate();
    }
    else if(colCount < _colCount)
    {
        for(size_t i = 0; i < _children.size(); ++i)
        {
            if(_children[i])
            {
                size_t row = i / _colCount;
                size_t col = i % _colCount;
                size_t newidx = row*colCount + col;
                if(newidx != i)
                {
                    _children[newidx] = _children[i];
                    _children[i] = nullptr;
                }
            }
        }
        _children.resize(_rowCount * colCount);
        _everyColWidth.resize(colCount);
        _colCount = colCount;
        invalidate();
    }
}

size_t GridLayout::ColCount() const
{
    return _colCount;
}

void GridLayout::SetMargin( kmScalar topMargin, kmScalar bottomMargin, 
    kmScalar leftMargin, kmScalar rightMargin )
{
    _topMargin = topMargin;
    _bottomMargin = bottomMargin;
    _leftMargin = leftMargin;
    _rightMargin = rightMargin;
    invalidate();
}

kmScalar GridLayout::TopMargin() const
{
    return _topMargin;
}

kmScalar GridLayout::BottomMargin() const
{
    return _bottomMargin;
}

kmScalar GridLayout::LeftMargin() const
{
    return _leftMargin;
}

kmScalar GridLayout::RightMargin() const
{
    return _rightMargin;
}

void GridLayout::SetGap( kmScalar hGap, kmScalar vGap )
{
    _hGap = hGap;
    _vGap = vGap;
    invalidate();
}

kmScalar GridLayout::HGap() const
{
    return _hGap;
}

kmScalar GridLayout::VGap() const
{
    return _vGap;
}

void GridLayout::SetRowHeight( kmScalar rowHeight )
{
    _rowHeight = rowHeight;
    invalidate();
}

void GridLayout::SetColWidth( kmScalar colWidth )
{
    _colWidth = colWidth;
    invalidate();
}

void GridLayout::SetRowHeightSingleFixed( size_t row, kmScalar height)
{
    BEATS_ASSERT(row < _rowCount);
    _everyRowHeight.resize(_rowCount);
    _everyRowHeight[row] = std::make_pair(height, 0.f);
    invalidate();
}

void GridLayout::SetRowHeightSingleFactored( size_t row, kmScalar heightFactor)
{
    BEATS_ASSERT(row < _rowCount);
    _everyRowHeight.resize(_rowCount);
    _everyRowHeight[row] = std::make_pair(0.f, heightFactor);
    invalidate();
}

void GridLayout::SetColWidthSingleFixed( size_t col, kmScalar width)
{
    BEATS_ASSERT(col < _colCount);
    _everyColWidth.resize(_colCount);
    _everyColWidth[col] = std::make_pair(width, 0.f);
    invalidate();
}

void GridLayout::SetColWidthSingleFactored( size_t col, kmScalar widthFactor)
{
    BEATS_ASSERT(col < _colCount);
    _everyColWidth.resize(_colCount);
    _everyColWidth[col] = std::make_pair(0.f, widthFactor);
    invalidate();
}

bool GridLayout::AddChild(Window *window)
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());

    for(size_t i = 0; i < _children.size(); ++i)
    {
        if(!_children[i])
        {
            _children[i] = window;
            invalidate();
            return true;
        }
    }

    return false;
}

bool GridLayout::AddChild( Window *window, kmScalar x, kmScalar y )
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());
    
    kmVec3 pos;
    kmVec3Fill(&pos, x, y, 0.f);
    _window->Localize(pos);
    x = pos.x;
    y = pos.y;
    
    kmVec2 realAnchor = _window->RealAnchor();
    x += realAnchor.x;
    y += realAnchor.y;

    int col = -1;
    for(size_t i = 0; i < _colCount; ++i)
    {
        kmScalar left = _hSplitPos[2*i].second * _window->RealSize().x + _hSplitPos[2*i].first;
        kmScalar right = _hSplitPos[2*i+1].second * _window->RealSize().x + _hSplitPos[2*i+1].first;
        if(left <= x && x <= right)
        {
            col = i;
            break;
        }
    }

    int row = -1;
    for(size_t i = 0; i < _rowCount; ++i)
    {
        kmScalar top = _vSplitPos[2*i].second * _window->RealSize().y + _vSplitPos[2*i].first;
        kmScalar bottom = _vSplitPos[2*i+1].second * _window->RealSize().y + _vSplitPos[2*i+1].first;
        if(top <= y && y <= bottom)
        {
            row = i;
            break;
        }
    }

	return row >= 0 && col >= 0 && AddChildToCell(window, row, col);
}

bool GridLayout::AddChildToCell( Window *window, size_t row, size_t col)
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(_window->GetChild(window->Name()), 
        _T("window:%s must be the child of window:%s"), window->Name().c_str(), _window->Name().c_str());
   
    if(row >= _rowCount)
    {
        return false;
    }
    if(col >= _colCount)
    {
        return false;
    }

    size_t index = row*_colCount+col;
    if(_children[index])    //already has item
    {
        return false;
    }

    _children[index] = window;
    invalidate();
    return true;
}

Window *GridLayout::GetChild( size_t row, size_t col )
{
    BEATS_ASSERT(row < _rowCount && col < _colCount);

    return _children[row*_colCount+col];
}

bool GridLayout::RemoveChild( size_t row, size_t col )
{
    BEATS_ASSERT(_window, _T("You must set this layout to a window first"));
    BEATS_ASSERT(row < _rowCount && col < _colCount);

    _children[row*_colCount+col] = nullptr;
    return true;
}

void GridLayout::PerformLayout()
{
    kmScalar spaceWidth = _leftMargin + _rightMargin + _hGap * (_colCount - 1);
    kmScalar averageWidth = (_window->RealSize().x - spaceWidth) / _colCount;
    kmScalar spaceHeight = _topMargin + _bottomMargin + _vGap * (_rowCount - 1);
    kmScalar averageHeight = (_window->RealSize().y - spaceHeight) / _rowCount;

    kmScalar totalFixedWidth = 0.f;
    kmScalar totalFactoredWidth = 0.f;
    kmScalar totalFixedHeight = 0.f;
    kmScalar totalFactoredHeight = 0.f;
    for(auto width : _everyColWidth)
    {
        totalFixedWidth += width.first;
        totalFactoredWidth += width.second;
    }
    for(auto height : _everyRowHeight)
    {
        totalFixedHeight += height.first;
        totalFactoredHeight += height.second;
    }

    BEATS_ASSERT(totalFixedWidth <= _window->RealSize().x);
    BEATS_ASSERT(totalFixedHeight <= _window->RealSize().y);

    //vertical split position
    _vSplitPos.clear();
    _vSplitPos.resize(2*_rowCount);
    kmScalar y = _topMargin;
    kmScalar yPercent = 0.f;
    for(size_t row = 0; row < _rowCount; ++row)
    {
        kmScalar height = _rowHeight;   //all row, same height
        kmScalar heightPercent = 0.f;
        if(height <= 0.f)   //all row, diffrent height
        {
            height = _everyRowHeight[row].first;
            heightPercent = _everyRowHeight[row].second;
            if(height > 0.f)    //fixed height
            {
                heightPercent = 0.f;
            }
            else if(heightPercent > 0.f)    //factored height
            {
                heightPercent = heightPercent / totalFactoredHeight;
                height = -(totalFixedHeight + spaceHeight) * heightPercent;
            }
            else    //average height
            {
                height = averageHeight;
            }
        }
        _vSplitPos[2*row].first = y;
        _vSplitPos[2*row].second = yPercent;
        _vSplitPos[2*row+1].first = y + height;
        _vSplitPos[2*row+1].second = yPercent + heightPercent;
        y += height + _vGap;
        yPercent += heightPercent;
    }

    //horizontal split position
    _hSplitPos.clear();
    _hSplitPos.resize(2*_colCount);
    kmScalar x = _leftMargin;
    kmScalar xPercent = 0.f;
    for(size_t col = 0; col < _colCount; ++col)
    {
        kmScalar width = _colWidth;
        kmScalar widthPercent = 0.f;
        if(width <= 0.f)
        {
            width = _everyColWidth[col].first;
            widthPercent = _everyColWidth[col].second;
            if(width > 0.f) //fixed width
            {
                widthPercent = 0.f;
            }
            else if(widthPercent > 0.f) //factored width
            {
                widthPercent = widthPercent / totalFactoredWidth;
                width = -(totalFixedWidth + spaceWidth) * widthPercent;
            }
            else  //average width
            {
                width = averageWidth;
            }
        }
        _hSplitPos[2*col].first = x;
        _hSplitPos[2*col].second = xPercent;
        _hSplitPos[2*col+1].first = x + width;
        _hSplitPos[2*col+1].second = xPercent + widthPercent;
        x += width + _hGap;
        xPercent += widthPercent;
    }

    //set position and size of child window
    for(size_t row = 0; row < _rowCount; ++row)
    {
        std::pair<kmScalar, kmScalar> top = _vSplitPos[2*row];
        std::pair<kmScalar, kmScalar> bottom = _vSplitPos[2*row+1];
        for(size_t col = 0; col < _colCount; ++col)
        {
            size_t index = row * _colCount + col;
            Window *child = _children[index];
            if(child)
            {
                std::pair<kmScalar, kmScalar> left = _hSplitPos[2*col];
                std::pair<kmScalar, kmScalar> right = _hSplitPos[2*col+1];
                child->SetArea(left.first, top.first, left.second, top.second,
                    right.first, bottom.first, right.second, bottom.second);
            }
        }
    }

    BaseLayout::PerformLayout();
}

