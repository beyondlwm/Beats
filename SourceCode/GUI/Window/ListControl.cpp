#include "stdafx.h"
#include "ListControl.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/WindowManager.h"
#include "GUI/Layout/GridLayout.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"


using namespace FCGUI;

ListControl::ListControl(const TString &name)
    : Window(name)
{
    Init();
}

ListControl::ListControl(CSerializer serializer)
    :Window(serializer)
{
    Init();
    DECLARE_PROPERTY(serializer, _rowHeight, true, 0xFFFFFFFF, _T("行高度"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _colWidth, true, 0xFFFFFFFF, _T("列宽度"), NULL, NULL, NULL);

}

ListControl::~ ListControl()
{
}

bool ListControl::OnMouseEvent( MouseEvent *event )
{
    bool bRet = false;
    if(IsEnabled())
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_PRESSED)
            {
                _bPressed = true;
                kmVec2Fill(&_pressedPos, event->X(), event->Y());
            }
            else if(event->Type() == EVENT_MOUSE_RELEASED)
            {
                _bPressed = false;
            }
            else if(event->Type() == EVENT_MOUSE_MOVED)
            {
                if(_bPressed)
                {
                    kmVec2 currPos;
                    kmVec2Fill(&currPos, event->X(), event->Y());
                    kmVec2 offset;
                    kmVec2Fill(&offset, currPos.x - _pressedPos.x , currPos.y - _pressedPos.y);
                    CheckScrollOffset(offset);
                    if(!BEATS_FLOAT_EQUAL(offset.x, 0) || !BEATS_FLOAT_EQUAL(offset.y, 0))
                    {
                        Scroll(offset);
                        WindowEvent event(EVENT_MOVE);
                        DispatchEvent(&event);
                    }
                }
            }
        }
        bRet = true;
    }
    return bRet;
}

void ListControl::AddItem(Window *item)
{
    GridLayout *layout = static_cast<GridLayout*>(Layout());
    layout->AddChild(item);
    CalcScrollOffsetRange();
}

void ListControl::RemoveItem(Window *item)
{
    GridLayout *layout = static_cast<GridLayout*>(Layout());
    layout->RemoveChild(item);
}

Window* ListControl::GetItem(size_t row, size_t col)
{
    GridLayout *layout = static_cast<GridLayout*>(Layout());
    return layout->GetChild(row, col);
}

void ListControl::SetRowHeight(kmScalar rowHeight)
{
     GridLayout *layout = static_cast<GridLayout*>(Layout());
     _rowHeight = rowHeight;
     layout->SetRowHeight(_rowHeight);
}

void ListControl::SetColWidth(kmScalar colWidth)
{
    GridLayout *layout = static_cast<GridLayout*>(Layout());
    _colWidth = colWidth;
    layout->SetColWidth(colWidth);
}

void ListControl::Init()
{
    kmVec2Fill(&_scrollOfffsetRange, 0.f, 0.f);
    kmVec2Fill(&_scrollOffset, 0.f, 0.f);
    _rowHeight = 20.f;
    _colWidth =20.f;
    _topMargin = 10.f;
    _bottomMargin = 10.f;
    _leftMargin = 10.f;
    _rightMargin = 10.f;
    _hGap = 10.f;
    _vGap = 10.f;
    InitLayout();
}

void ListControl::InitLayout()
{
    GridLayout *pGridLayout = new GridLayout(2, 2);
    pGridLayout->SetMargin(_topMargin, _bottomMargin, _leftMargin, _rightMargin);
    pGridLayout->SetGap(_hGap, _vGap);
    pGridLayout->SetRowHeight(_rowHeight);
    pGridLayout->SetColWidth(_colWidth);
    SetLayout(pGridLayout);
    CalcScrollOffsetRange();
}

void ListControl::CheckScrollOffset(kmVec2 &offset)
{
    _scrollOffset.x += offset.x;
    _scrollOffset.y += offset.y;
    if( _scrollOffset.x < 0 )
    {
        offset.x = offset.x - _scrollOffset.x;
        _scrollOffset.x = 0;
    }
    else if(_scrollOffset.x > _scrollOfffsetRange.x)
    {
        offset.x = offset.x - (_scrollOffset.x - _scrollOfffsetRange.x);
        _scrollOffset.x = _scrollOfffsetRange.x;
    }
    if( _scrollOffset.y < 0 )
    {
        offset.y = offset.y - _scrollOffset.y;
        _scrollOffset.y = 0;
    }
    else if(_scrollOffset.y > _scrollOfffsetRange.y)
    {
        offset.y = offset.y - (_scrollOffset.y - _scrollOfffsetRange.y);
        _scrollOffset.y = _scrollOfffsetRange.y;
    }
}

void ListControl::CalcScrollOffsetRange()
{
    kmVec2 viewSize = RealSize();
    GridLayout *layout = static_cast<GridLayout*>(Layout());
    kmScalar realWidth = 0;
    kmScalar realHeight = 0;
    size_t row = layout->RowCount();
    size_t col = layout->ColCount();

    realWidth +=  (col - 1) * ((layout->LeftMargin() + layout->RightMargin()) / 2.0f );
    realWidth +=  col * layout->HGap();
    realWidth += _colWidth * col;

    realHeight += ( row - 1) *((layout->TopMargin() + layout->BottomMargin()) / 2.0f);
    realHeight += row * layout->VGap();
    realHeight += _rowHeight * row;

    _scrollOfffsetRange.x = realWidth - viewSize.x;
    _scrollOfffsetRange.y = realHeight - viewSize.y;
}
