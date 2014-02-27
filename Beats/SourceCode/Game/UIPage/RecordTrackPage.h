#ifndef BEATS_GAME_UIPAGE_RECORDTRACKPAGE_H__INCLUDE
#define BEATS_GAME_UIPAGE_RECORDTRACKPAGE_H__INCLUDE

#include "../../ui/UIPageBase.h"
#include "../../ai/FSM.h"
#include "../../Control/KeyboardDef.h"

enum ERecordTrackPageComp
{
    eRTPC_PlayBtn,
    eRTPC_PauseBtn,
    eRTPC_StopBtn,
    eRTPC_ExitBtn,
    eRTPC_AddBeatBtn,
    eRTPC_PlayScroll,
    eRTPC_BeatsList,

    eRTPC_Count,
    eRTPC_ForceTo32Bit = 0xffffffff
};

static const TCHAR* RecordTrackPageCompName[] = 
{
    _T("RecordTrackPage_PlayBtn"),
    _T("RecordTrackPage_PauseBtn"),
    _T("RecordTrackPage_StopBtn"),
    _T("RecordTrackPage_ExitBtn"),
    _T("RecordTrackPage_AddBeatBtn"),
    _T("RecordTrackPage_AudioPlayBar"),
    _T("RecordTrackPage_BeatsList")
};

class CRecordTrack;

class CRecordTrackPage : public CUIPageBase
{
    typedef CUIPageBase super;
public:
    CRecordTrackPage();
    virtual ~CRecordTrackPage();

public:
    virtual void Init();
    virtual void InitUIEvent();
    virtual bool OpenImpl();
    virtual bool CloseImpl();
    virtual void Update();

    void UpdatePlayScroll(float progress); // scope [0, 1]
    void SetOwner(CRecordTrack* pOwner);
    void AddDataToList(unsigned long playPos, EKeyCode code);
    void UpdateData(unsigned long playPos, EKeyCode code);
    void RemoveDataFromList(CEGUI::ListboxItem* pSelectItem );
    void RemoveDataFromList(unsigned long playPos);

private:
    bool OnClickPlayBtn(const CEGUI::EventArgs& e);
    bool OnClickPauseBtn(const CEGUI::EventArgs& e);
    bool OnClickStopBtn(const CEGUI::EventArgs& e);
    bool OnClickExitBtn(const CEGUI::EventArgs& e);
    bool OnClickAddBeatBtn(const CEGUI::EventArgs& e);
    bool OnDragScrollBarEnd(const CEGUI::EventArgs& e);
    bool OnDragScrollBarBegin(const CEGUI::EventArgs& e);
    bool OnPressKeyBoard(const CEGUI::EventArgs& e);
    bool OnListClicked(const CEGUI::EventArgs& e);
    
    size_t CalculateItemIndex(unsigned long pos);

    DECLARE_FSM(CRecordTrackPage);
    DECLARE_FSM_STATE(CRecordTrackPage, Recording);
    DECLARE_FSM_STATE(CRecordTrackPage, Pause);
    DECLARE_FSM_STATE(CRecordTrackPage, Exit);
    DECLARE_FSM_STATE(CRecordTrackPage, Stop);


private:
    bool m_scrollUpdateSwitch;
    CRecordTrack* m_pOwner;
};
#endif