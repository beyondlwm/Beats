#ifndef __MYCANVAS_H
#define __MYCANVAS_H

#include "wx/wx.h"
#include "timebaritem.h"

class TimeBarDataViewCtrl;
class TimeBarFrame;
class TimeBarScale;
class TimeBarItem;

struct SSelectCtrl
{
    int m_iSelectRowBegin;
    int m_iSelectRowEnd;
    int m_iSelectColumnBegin;
    int m_iSelectColumnEnd;
};

class TimeBarItemContainer: public wxScrolledWindow
{
    typedef wxScrolledWindow super;
public:
    TimeBarItemContainer(wxWindow* parent);
    virtual ~TimeBarItemContainer();

    virtual void        ScrollWindow(int x, int y, const wxRect *rect = NULL);
    void                SyncWith(TimeBarDataViewCtrl* pDataViewCtrl, TimeBarScale* pScaleBarCtrl);
    void                AddItem(TimeBarItem* pitem);
    void                SetItemPosition();
    void                SelectItems();
    void                DeleteItem(int index);
    void                SetCellWidth(int width);
    void                SetItemCellsCount(int iCount);
    void                SetSplitterwnd(TimeBarFrame* pSplitterWindow);
    int                 GetCellWidth() const;
    SSelectCtrl&        GetCurrentSelect();
    ArrayOfTimeBarItem* GetLineBarItems();
    TimeBarFrame*       GetSplitterwnd();

    void                OnSize(wxSizeEvent & event);

private:
    int                         m_iCellWidth;
    int                         m_iCursorPositionX;
    int                         m_iItemCellsCount;
    TimeBarDataViewCtrl*        m_pSyncWnd;
    TimeBarScale*               m_pSyncScale;
    TimeBarFrame*               m_pSplitterwnd;
    ArrayOfTimeBarItem          m_items;
    SSelectCtrl                 m_currentSelect;
    SSelectCtrl                 m_lastSelect;

    wxDECLARE_NO_COPY_CLASS(TimeBarItemContainer);
};


#endif