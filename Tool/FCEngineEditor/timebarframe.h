#ifndef __SPLITER_H
#define __SPLITER_H

#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "timebaritemcontainer.h"
#include "timebarscale.h"
#include "wx/button.h"
#include "timebardataviewctrl.h"
#include <wx/spinctrl.h>

#define FREQUENCYREDUCTIONFACTOR 4
#define DIFFERENTDRAWLENGTH 5
    
class CAnimation;
class TimeBarDataViewCtrl;
class TimeBarItemContainer;
class wxSpinCtrlDouble;
class TimeBarFrame : public wxSplitterWindow
{
    typedef wxSplitterWindow super;
public:
    TimeBarFrame(wxWindow *parent);
    virtual ~TimeBarFrame();

    void            ResetLeft();
    void            AddItems();
    void            AddItems(SharePtr<CAnimation> pAnimationItem);
    void            DeleteItems(int index);
    void            ClickOnScaleBar();
    void            SetCursorPositionX(int pos);
    int             GetCursorPositionX();
    void            SetCurrentCursor(int pos);
    void            SetCurrentSelect(int iCurrentSelect);
    int             GetCurrentSelect() const;
    void            SetClickOnScalebar(bool bIsClicked);
    bool            IsClickOnScalebar() const;
    void            SetFps(size_t iFps);
    void            SetFpsToAnimation(size_t iFps);
    void            SetFrameWindow(wxWindow* pWindow);
    int             GetItemCount();
    wxWindow*       GetFrameWindow();
    TimeBarScale*   GetScalebar();
    TimeBarItemContainer*   GetItemContainer();

protected:
    void OnSize(wxSizeEvent& event);
    void OnBtnAddClick(wxCommandEvent& event);
    void OnBtnDeleteClick(wxCommandEvent& event);
    void OnPositionChanged(wxSplitterEvent& event);
    void OnPositionChanging(wxSplitterEvent& event);
    void OnSpinCtrl(wxSpinEvent& event);
    void OnSpinTextEnter(wxCommandEvent& event);
    void OnSelect(wxDataViewEvent& event);

private:
    int                     m_iItemId;
    int                     m_iCurrentSelect;
    int                     m_iCursorPositionX;
    bool                    m_bclickonscalebar;
    TimeBarDataViewCtrl*    m_pTimeLineDataViewCtrl;
    TimeBarItemContainer*   m_pContainer;
    TimeBarScale*           m_pScalebar;
    wxButton*               m_pButtonAdd;
    wxButton*               m_pButtonDelete;
    wxSpinCtrl*             m_pSpinCtrl;
    wxWindow*               m_pFrame;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(TimeBarFrame);
};

#endif