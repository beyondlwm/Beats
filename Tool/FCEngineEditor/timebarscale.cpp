#include "stdafx.h"
#include "timebarscale.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"
#include "wx/dcmirror.h"
#include "timebarframe.h"

#define SCALELINELENGTH 8
#define SCALELINEOFFSETX -1
#define SCALETEXTOFFSETX -1
#define SCALETEXTOFFSETY -23


TimeBarScale::TimeBarScale( wxWindow *parent ):
wxScrolledWindow (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                  wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE | wxSHOW_SB_ALWAYS),
                  m_iScaleWidth(SCALECELLWIDTH), m_iScaleCount(INITCELLNUM), m_iCursorPositionX(0)
{
    ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
    SetMinSize(wxSize(0, SCALEBARHIGHT));
    SetScrollbars(SCALECELLWIDTH, 0, INITCELLNUM, 0);
}

BEGIN_EVENT_TABLE(TimeBarScale, wxPanel)
EVT_MOUSE_EVENTS(TimeBarScale::OnMouse)
END_EVENT_TABLE()

void TimeBarScale::DrawScale(wxDC* dc)
{
    wxDC* WDC = new wxMirrorDC(*dc,false);
    int iWidth = m_iScaleCount * m_iScaleWidth;
    int iPositionX = m_pSplitterwnd->GetCursorPositionX();
    wxRect ScaleRect = wxRect(0,0,iWidth,SCALEBARHIGHT);

    wxBitmap test_bitmap(ScaleRect.GetSize());
    wxMemoryDC ScaleDC(test_bitmap); 
    ScaleDC.DrawRectangle(ScaleRect);
    ScaleDC.SetFont(*wxSMALL_FONT);

    for(int i = 0; i < m_iScaleCount; i++)
    {
        if(i % DIFFERENTDRAWLENGTH == 0)
        {
            ScaleDC.DrawText(wxString::Format(_T("%d"),i), i*m_iScaleWidth, ScaleRect.height + SCALETEXTOFFSETY);
        }
        ScaleDC.DrawLine(i * m_iScaleWidth + SCALELINEOFFSETX, ScaleRect.height, i * m_iScaleWidth + SCALELINEOFFSETX, ScaleRect.height - SCALELINELENGTH);
    }
    wxPen pen = *wxRED_PEN;
    pen.SetWidth(CURSORWIDTH);
    ScaleDC.SetPen(pen);
    ScaleDC.DrawLine(iPositionX + SCALELINEOFFSETX, ScaleRect.height, iPositionX + SCALELINEOFFSETX, 0);
    WDC->Blit(ScaleRect.GetLeft(), ScaleRect.GetTop(), ScaleRect.width, ScaleRect.height, &ScaleDC, 0, 0);
    delete WDC;
}

void TimeBarScale::OnDraw(wxDC& dcOrig)
{
    DrawScale(&dcOrig);
}

void TimeBarScale::OnMouse( wxMouseEvent& event )
{

    if (event.ButtonDown(wxMOUSE_BTN_LEFT))
    {
        ::SetCapture(GetHWND());
        m_pSplitterwnd->SetClickOnScalebar(true);
        wxPoint pstn = event.GetPosition();
        int iScale = PointToScale(pstn);
        int iPositionX = iScale + GetViewStart().x;
        iPositionX *= m_iScaleWidth;
        iPositionX += m_iScaleWidth/2;
        m_pSplitterwnd->SetCursorPositionX(iPositionX);
        m_pSplitterwnd->ClickOnScaleBar();
    }
    else if (event.ButtonUp(wxMOUSE_BTN_LEFT))
    {
        ::ReleaseCapture();
    }
    else if (event.Dragging())
    {   
        wxPoint pstn= event.GetPosition();
        if (pstn.x % FREQUENCYREDUCTIONFACTOR == 0)
        {
            int iScale = PointToScale(pstn);
            int iPositionX = iScale + GetViewStart().x;
            iPositionX *= m_iScaleWidth;
            iPositionX += m_iScaleWidth/2;
            m_pSplitterwnd->SetCursorPositionX(iPositionX);
            m_pSplitterwnd->ClickOnScaleBar();
        } 
    }
    event.Skip();
}

void TimeBarScale::SetCursorPositionX(int iPositionX)
{
    m_iCursorPositionX = iPositionX;
}

int TimeBarScale::GetCursorPositionX()
{
    return m_iCursorPositionX;
}

int TimeBarScale::GetScaleWidth()
{
    return m_iScaleWidth;
}

void TimeBarScale::SetScaleCount(int iScaleCount)
{
    m_iScaleCount = iScaleCount;
}

int TimeBarScale::GetScaleCount()
{
    return m_iScaleCount;
}

void TimeBarScale::SetSplitterwnd( TimeBarFrame* pSplitterWindow )
{
    m_pSplitterwnd = pSplitterWindow;
}

TimeBarFrame* TimeBarScale::GetSplitterwnd()
{
    return m_pSplitterwnd;
}

int TimeBarScale::PointToScale( wxPoint point )
{
    return point.x / m_iScaleWidth;
}

wxPoint TimeBarScale::ScaleToPoint( int iScale )
{
    return wxPoint(iScale * m_iScaleWidth, 0);
}
