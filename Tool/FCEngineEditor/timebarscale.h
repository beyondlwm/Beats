#ifndef __SCALEBARCTRL_H
#define __SCALEBARCTRL_H

#include "wx/scrolwin.h"

class TimeBarFrame;
class TimeBarScale : public wxScrolledWindow
{
public:
    TimeBarScale( wxWindow *parent );
    
    void                DrawScale(wxDC* dc);
    void                SetCursorPositionX(int iPositionX);
    int                 GetCursorPositionX();
    int                 GetScaleWidth();
    int                 PointToScale(wxPoint point);
    wxPoint             ScaleToPoint(int iScale);
    void                SetScaleCount(int iScaleCount);
    int                 GetScaleCount();
    void                SetSplitterwnd(TimeBarFrame* pSplitterWindow);
    TimeBarFrame*       GetSplitterwnd();

    virtual void        OnDraw(wxDC& dc);
    void                OnMouse( wxMouseEvent& event );
private:
    int     m_iCursorPositionX;
    int     m_iScaleWidth;
    int     m_iScaleCount;
    TimeBarFrame* m_pSplitterwnd;
    DECLARE_EVENT_TABLE()
};



#endif