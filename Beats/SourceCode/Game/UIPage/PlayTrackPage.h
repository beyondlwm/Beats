#ifndef BEATS_GAME_UIPAGE_PLAYTRACKPAGE_H__INCLUDE
#define BEATS_GAME_UIPAGE_PLAYTRACKPAGE_H__INCLUDE

#include "../../ui/UIPageBase.h"
#include "../../ai/FSM.h"

class CPlayTrack;

enum EPlayTrackPageComp
{
    ePTPC_ScoreText,
    ePTPC_ComboCounterText,

    ePTPC_SystemWindow,
    ePTPC_ContinueBtn,
    ePTPC_RestartBtn,
    ePTPC_ExitBtn,

    ePTPC_Count,
    ePTPC_Force32Bit = 0xffffffff
};

static const TCHAR* PlayTrackPageCompName[] = 
{
    _T("PlayTrackPage_ScoreText"),
    _T("PlayTrackPage_ComboCounterText"),

    _T("PlayTrackPage_SystemWindow"),
    _T("PlayTrackPage_ContinueBtn"),
    _T("PlayTrackPage_RestartBtn"),
    _T("PlayTrackPage_ExitBtn")
};

class CPlayTrackPage : public CUIPageBase
{
    typedef CUIPageBase super;
public:
    CPlayTrackPage();
    ~CPlayTrackPage();

public:
    virtual void Init();
    virtual void InitUIEvent();
    virtual bool OpenImpl();
    virtual bool CloseImpl();
    virtual void Update();

    void SetOwner(CPlayTrack* pOwner);

    void UpdateScore(unsigned long score);
    void UpdateComboCounter(unsigned long counter);

    void SetVisible(EPlayTrackPageComp type, bool visibleFlag);

private:

    bool OnClickContinueBtn(const CEGUI::EventArgs& e);
    bool OnClickRestartBtn(const CEGUI::EventArgs& e);
    bool OnClickExitBtn(const CEGUI::EventArgs& e);    

private:
    CPlayTrack* m_pOwner;
};


#endif