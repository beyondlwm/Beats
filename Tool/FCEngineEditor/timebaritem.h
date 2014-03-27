#ifndef __TIMELINEBARITEM_H
#define __TIMELINEBARITEM_H

#define CELLWIDTH 10

#include "timebaritemcell.h"
#include "wx/panel.h"

class TimeBarItem : public wxPanel
{
    typedef wxPanel super;
public:
    enum
    {
        Cell_Type0,
        Cell_Type1,
        Cell_Type2,
        Cell_Type3
    };

    TimeBarItem( wxWindow *parent, wxWindowID id=wxID_ANY, 
        const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
        long style= wxTAB_TRAVERSAL, const wxString &name=wxPanelNameStr );
    ~TimeBarItem();
    void        DrawItem();
    void        UnSelecteAll();
    void        SelectCell( int index, bool bselect );
    void        SelectManyCells(int start, int end = -1);
    int         PointToCell(wxPoint point);
    wxPoint     CellToPoint(int index);
    void        OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
    void        OnPaint( wxPaintEvent& event );
    void        OnMouse(wxMouseEvent& event);
    int         GetColumnID();
    void        SetDataCells(int iDataCellsNum);
    void        AddCells();
    void        SetCellsCount(int iCount);
    

private:
    int         m_CellsCount;
    int         m_CellWidth;  
    int         m_begin;        
    int         m_end;          
    ArrayOfCell m_Cells;              
    DECLARE_EVENT_TABLE()
};
WX_DECLARE_OBJARRAY(TimeBarItem, ArrayOfTimeBarItem);

#endif