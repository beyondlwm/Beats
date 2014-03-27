#ifndef FCENGINEEDITOR_ENGINESPLASH_H__INCLUDE
#define FCENGINEEDITOR_ENGINESPLASH_H__INCLUDE

#include "wx/image.h"
#include "wx/splash.h"
#include "wx/mediactrl.h"

class EngineSplash
{
public:
    EngineSplash(wxBitmap* bitmap, long splashStyle, int milliseconds,
        wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP);
    virtual ~EngineSplash();
    wxBitmap* GetBitmap() const;
    void Instance();
private:
    wxBitmap*   m_pBitmap;
    long        m_Style;
    long        m_SplashStyle;
    int         m_iMilliseconds;
    wxWindow*   m_pParent;
    wxWindowID  m_Id;
    wxPoint     m_Pos;
    wxSize      m_Size;
    
};

#endif