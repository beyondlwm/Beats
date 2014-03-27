#include "stdafx.h"
#include "EngineSplash.h"

EngineSplash::EngineSplash(wxBitmap* bitmap, long splashStyle, int milliseconds,
                           wxWindow* parent, wxWindowID id,
                           const wxPoint& pos ,
                           const wxSize& size ,
                           long style) :
                    m_SplashStyle(splashStyle), m_iMilliseconds(milliseconds), m_pParent(parent), m_Id(id), m_Pos(pos), m_Size(size), m_Style(style)
{
    if (bitmap->IsOk())
    {
        m_pBitmap = bitmap;
    }
    else
    {
        m_pBitmap = NULL;
    }
}

EngineSplash::~EngineSplash()
{
}

wxBitmap* EngineSplash::GetBitmap() const
{
    return m_pBitmap;
}

void EngineSplash::Instance()
{
    wxBitmap bmp;
    if (m_pBitmap != NULL)
    {
        bmp = *m_pBitmap;
    }
    
    wxSplashScreen* Splash = new wxSplashScreen(bmp, m_SplashStyle, m_iMilliseconds, m_pParent, m_Id, m_Pos, m_Size, m_Style);
    wxWindow *win = Splash->GetSplashWindow();
    wxStaticText *text = new wxStaticText( win,
        wxID_EXIT,
        wxT("FCEngineEditor V_0_1"),
        wxPoint(13, 11)
        );

    text->SetBackgroundColour(*wxWHITE);
    text->SetForegroundColour(*wxBLACK);
    wxFont font = text->GetFont();
    font.SetPointSize(2*font.GetPointSize()/3);
    text->SetFont(font);
}