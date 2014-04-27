#include "stdafx.h"
#include "timebaritemcontainer.h"
#include "timebarframe.h"
#include "timebarscale.h"
#include "timebaritem.h"

TimeBarItemContainer::TimeBarItemContainer(wxWindow* parent)
: wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL)
, m_iCellWidth(CELLWIDTH)
, m_pSyncWnd(NULL)
, m_pSyncScale(NULL)
, m_pSplitterwnd(NULL)
, m_iItemCellsCount(INITCELLNUM)
, m_iCursorPositionX(0)
{
    memset(&m_currentSelect, 0, sizeof(m_currentSelect));
    memset(&m_lastSelect, 0, sizeof(m_lastSelect));
    ShowScrollbars(wxSHOW_SB_ALWAYS,wxSHOW_SB_ALWAYS);
    SetScrollbars(CELLWIDTH, CELLHIGHT, SCROLLUNITSX, SCROLLUNITSY);
}

TimeBarItemContainer::~TimeBarItemContainer()
{

}

void TimeBarItemContainer::SyncWith(TimeBarDataViewCtrl *win, TimeBarScale* win2)
{
    m_pSyncWnd = win;
    m_pSyncScale = win2;
}

void TimeBarItemContainer::ScrollWindow(int dx, int dy, const wxRect *rect )
{
    super::ScrollWindow(dx, dy, rect);
    wxPoint ptViewStart = GetViewStart();
    bool bHorizontalMove = dx != 0 && dy == 0;
    if (bHorizontalMove)
    {
        wxPoint pt = ptViewStart;
        m_pSyncScale->Scroll(pt);
    }
    else
    {
        m_pSyncWnd->Scroll(ptViewStart);
    }
}

void TimeBarItemContainer::AddItem(TimeBarItem* pItem)
{
    m_items.Add(pItem);
    pItem->SetCellsCount(m_iItemCellsCount);
    pItem->AddCells();
    m_pSplitterwnd->GetScalebar()->SetScaleCount(m_iItemCellsCount);
    m_pSplitterwnd->GetScalebar()->SetScrollbars(CELLWIDTH, 0, m_iItemCellsCount, 0);
    SetItemPosition();
    SetScrollbars(CELLWIDTH, CELLHIGHT, m_iItemCellsCount, m_items.GetCount());
}

void TimeBarItemContainer::SetItemPosition()
{
    int num = m_items.GetCount();
    for (int i = 0; i < num; i++)
    {
        m_items[i].SetPosition(wxPoint(0, i * CELLHIGHT));
    }
}

void TimeBarItemContainer::SetSplitterwnd(TimeBarFrame* pSplitterWindow)
{
    m_pSplitterwnd = pSplitterWindow;
}

ArrayOfTimeBarItem* TimeBarItemContainer::GetLineBarItems()
{
    return &m_items;
}

TimeBarFrame* TimeBarItemContainer::GetSplitterwnd()
{
    return m_pSplitterwnd;
}

SSelectCtrl& TimeBarItemContainer::GetCurrentSelect()
{
    return m_currentSelect;
}

void TimeBarItemContainer::SetCellWidth(int iWidth)
{
    m_iCellWidth = iWidth;
}

void TimeBarItemContainer::DeleteItem(int index)
{
    TimeBarItem* p = m_items.Detach(index);
    delete p;
    SetItemPosition();
}

void TimeBarItemContainer::SelectItems()
{
    int columnstart = m_currentSelect.m_iSelectColumnBegin > m_currentSelect.m_iSelectColumnEnd ? m_currentSelect.m_iSelectColumnEnd : m_currentSelect.m_iSelectColumnBegin;
    int columnend = m_currentSelect.m_iSelectColumnBegin < m_currentSelect.m_iSelectColumnEnd ? m_currentSelect.m_iSelectColumnEnd : m_currentSelect.m_iSelectColumnBegin;
    int rowstart = m_currentSelect.m_iSelectRowBegin > m_currentSelect.m_iSelectRowEnd ? m_currentSelect.m_iSelectRowEnd : m_currentSelect.m_iSelectRowBegin;
    int rowend = m_currentSelect.m_iSelectRowBegin < m_currentSelect.m_iSelectRowEnd ? m_currentSelect.m_iSelectRowEnd : m_currentSelect.m_iSelectRowBegin;

    int num = m_items.GetCount();
    for (int i = 0; i < num; i++)
    {
        m_items[i].UnSelecteAll();
        if (i >= columnstart && i <= columnend)
        {
            m_items[i].SelectManyCells(rowstart,rowend);
        }
        m_items[i].Refresh(false);
    }
    m_lastSelect = m_currentSelect;
}

void TimeBarItemContainer::SetItemCellsCount( int iCount )
{
    if (m_iItemCellsCount <= iCount)
    {
        m_iItemCellsCount = iCount + 100;
    } 
}

int TimeBarItemContainer::GetCellWidth() const
{
    return m_iCellWidth;
}
