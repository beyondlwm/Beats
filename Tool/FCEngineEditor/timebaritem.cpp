#include "stdafx.h"
#include <wx/dcbuffer.h>
#include "timebaritem.h"
#include "timebaritemcontainer.h"
#include "timebarframe.h"
#include "EditAnimationDialog.h"
#include "Render/AnimationController.h"

ArrayOfTimeBarItem::~ArrayOfTimeBarItem()
{

}

TimeBarItem::TimeBarItem( wxWindow *parent, wxWindowID id, 
                             const wxPoint &pos, const wxSize &size, 
                             long style, const wxString &name )
                             : wxPanel(parent, id, pos, size, style, name),
                             m_CellWidth(CELLWIDTH),
                             m_begin(0),
                             m_end(0)

{
    SetMinSize(wxSize(0, CELLHIGHT));
}

TimeBarItem::~TimeBarItem()
{
    TimeBarItemCell* p = NULL;
    while( m_Cells.GetCount() > 0 )
    {
        p = m_Cells.Detach(0);
        delete p;
    }
}

BEGIN_EVENT_TABLE(TimeBarItem, wxPanel)
    EVT_PAINT(TimeBarItem::OnPaint)
    EVT_MOUSE_EVENTS(TimeBarItem::OnMouse)
    EVT_MOUSE_CAPTURE_LOST(TimeBarItem::OnMouseCaptureLost)
END_EVENT_TABLE()

void TimeBarItem::DrawItem()
{
    TimeBarItemContainer* pContainer = (TimeBarItemContainer*)GetParent();
    int cursorpositionx = pContainer->GetSplitterwnd()->GetCursorPositionX() - 1;
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxAutoBufferedPaintDC ScaleDC(this); 
    wxRect ItemRect = GetRect();
    ScaleDC.SetPen(*wxBLACK_PEN);

    AddCells();
    wxPoint ViewStart = pContainer->GetViewStart();
    wxSize size = pContainer->GetSize();
    size_t iCount  = size.x * 0.1F;
    iCount += ViewStart.x;
    for(size_t i = ViewStart.x; i < iCount; i++)
    {
        if(i < m_Cells.GetCount())
        {
            m_Cells[i].OnDraw(ScaleDC, pContainer->GetCellWidth());
        }
    }
    wxPen pen = *wxRED_PEN;
    pen.SetWidth(CURSORWIDTH);
    ScaleDC.SetPen(pen);
    ScaleDC.DrawLine(cursorpositionx, 0, cursorpositionx, ItemRect.height);
}

void TimeBarItem::OnMouseCaptureLost(wxMouseCaptureLostEvent& /*event*/)
{
    UnSelecteAll();
}

void TimeBarItem::AddCells()
{
    int num = m_CellsCount;
    int count = m_Cells.GetCount();
    int addnum = num - count;
    int index = 0;
    TimeBarItemContainer* pContainer = (TimeBarItemContainer*)GetParent();
    if (addnum>0)
    {
        for (int i = 0; i < addnum; i++)
        {
            index = count+i;
            m_Cells.Add(new TimeBarItemCell);
            m_Cells[index].SetPositionX(pContainer->GetCellWidth()*(count+i));
            if (index % DIFFERENTDRAWLENGTH == 0)
            {
                m_Cells[index].SetBgColour(TimeBarItemCell::BACKGROUNDCOLOUR2);
            }
            else
            {
                m_Cells[index].SetBgColour(TimeBarItemCell::BACKGROUNDCOLOUR1);
            }
        }
    }
}

void TimeBarItem::OnPaint(wxPaintEvent& /*event*/)
{
    TimeBarItemContainer* pContainer = (TimeBarItemContainer*)GetParent();
    SetSize(pContainer->GetCellWidth() * m_CellsCount,CELLHIGHT);
    DrawItem();
}

int TimeBarItem::GetColumnID()
{
    int iRet = -1;
    TimeBarItemContainer* pParent = (TimeBarItemContainer*)GetParent();
    int num = pParent->GetLineBarItems()->GetCount();
    for (int i = 0; i < num; i++)
    {
        if (&(pParent->GetLineBarItems()->Item(i)) == this)
        {
            iRet = i;
            break;
        }
    }
    return iRet;
}

void TimeBarItem::SetDataCells(int iDataCellsNum)
{
    int cellnum = m_Cells.GetCount();
    m_end = iDataCellsNum - 1;
    for (int i = 0; i < cellnum; i++)
    {
        if (i >= m_begin && i <= m_end)
        {
            m_Cells[i].SetData(1);
        }
        else
        {
            m_Cells[i].SetData(0);
        }
    }
}

void TimeBarItem::OnMouse(wxMouseEvent& event)
{
    TimeBarItemContainer* pParent = (TimeBarItemContainer*)GetParent();
    wxPoint pstn= event.GetPosition(); 
    int index = PointToCell(pstn);
    int iCellWidth = pParent->GetCellWidth();
    int CursorPosition = pstn.x / iCellWidth;
    CursorPosition *= iCellWidth;
    CursorPosition += iCellWidth * 0.5F;
    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        pParent->GetSplitterwnd()->SetClickOnScalebar(false);
        pParent->GetSplitterwnd()->SetCursorPositionX(CursorPosition);

        pParent->GetCurrentSelect().m_iSelectRowBegin = index;
        pParent->GetCurrentSelect().m_iSelectColumnBegin = GetColumnID();
        pParent->GetCurrentSelect().m_iSelectRowEnd = index;
        pParent->GetCurrentSelect().m_iSelectColumnEnd = GetColumnID();
        pParent->GetSplitterwnd()->ClickOnScaleBar();
    }
    else if (event.Dragging())
    {   
        if (pstn.x % FREQUENCYREDUCTIONFACTOR == 0)
        {
            if (!pParent->GetSplitterwnd()->IsClickOnScalebar())
            {
                pParent->GetCurrentSelect().m_iSelectRowEnd = index;
                pParent->GetCurrentSelect().m_iSelectColumnEnd = GetColumnID();
            }
            pParent->GetSplitterwnd()->SetCursorPositionX(CursorPosition);
            pParent->GetSplitterwnd()->ClickOnScaleBar();
        }
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        int id = GetColumnID();
        pParent->GetSplitterwnd()->SetCurrentSelect(id);
    }
    event.Skip();
};

int TimeBarItem::PointToCell(wxPoint point)
{
    TimeBarItemContainer* pParent = (TimeBarItemContainer*)GetParent();
    return point.x / pParent->GetCellWidth();
}

wxPoint TimeBarItem::CellToPoint(int index)
{
    TimeBarItemContainer* pParent = (TimeBarItemContainer*)GetParent();
    return wxPoint(index * pParent->GetCellWidth(), 0);
}

void TimeBarItem::UnSelecteAll()
{
    int num = m_Cells.GetCount();
    for (int i = 0; i < num; i++)
    {
        m_Cells[i].SetIsSelected(false);
    }
}

void TimeBarItem::SelectCell( int index, bool bselect )
{
    m_Cells[index].SetIsSelected(bselect);
}

void TimeBarItem::SelectManyCells(int start, int end)
{
    int i = start > end ? end : start;
    int j = start < end ? end : start;
    if(i >= 0)
    {
        for (i; i <= j; i++)
        {
            m_Cells[i].SetIsSelected(true);
        }
    }
}

void TimeBarItem::SetCellsCount( int iCount )
{
    m_CellsCount = iCount;
}
