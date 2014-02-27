#ifndef BEATS_GAME_UIPAGE_TITLEPAGE_H__INCLUDE
#define BEATS_GAME_UIPAGE_TITLEPAGE_H__INCLUDE

#include "../../ui/UIPageBase.h"
#include "../../ai/FSM.h"

enum ETitlePageComp
{
    eTPC_CreateBtn,
    eTPC_PlayBtn,
    eTPC_BrowseBtn,
    eTPC_SelectOkBtn,
    eTPC_SelectBackBtn,
    eTPC_TrackList,

    eTPC_SelectAudioFileBtn,
    eTPC_SelectBRFileBtn,
    eTPC_AudioFilePathTxt,
    eTPC_BeatsRecordTxt,
    eTPC_StartBtn,
    eTPC_BackBtn,

    eTPC_Count,
    eTPC_ForceTo32Bit = 0xffffffff
};

static const TCHAR* TitlePageCompName[] = 
{
    _T("TitlePage_CreateTrackBtn"),
    _T("TitlePage_PlayTrackBtn"),
    _T("TitlePage_BrowseBtn"),
    _T("TitlePage_SelectOkBtn"),
    _T("TitlePage_SelectBackBtn"),
    _T("TitlePage_TrackList"),

    _T("TitlePage_SelectAudioFileBtn"),
    _T("TitlePage_SelectBRFBtn"),
    _T("TitlePage_AudioFilePathTxt"),
    _T("TitlePage_BeatsRecordTxt"),
    _T("TitlePage_StartBtn"),
    _T("TitlePage_BackBtn")
};

namespace CEGUI
{
    class ItemEntry;
}
class CTitleTask;

class CTitlePage : public CUIPageBase
{
    typedef CUIPageBase super;
public:
    CTitlePage();
    virtual ~CTitlePage();

public:    
    virtual void Init();
    virtual bool OpenImpl();
    virtual bool CloseImpl();
    virtual void Update();

    void SetOwner(CTitleTask* pOwner);
    void SetAudioPathText(const TString& path);
    void SetBRFPathText(const TString& path);

private:
    void InitUIEvent();
    bool AcquireAudioPath(TString& path);
    void AddToTrackList(const TCHAR* name);
    void ClearTrackList();
    CEGUI::ItemEntry* GetSelectItem();
    void ReverseFSM();

    bool OnClickCreateBtn(const CEGUI::EventArgs& args);
    bool OnClickPlayBtn(const CEGUI::EventArgs& args);
    bool OnClickSelectBackBtn(const CEGUI::EventArgs& args);
    bool OnClickBrowseBtn(const CEGUI::EventArgs& args);
    bool OnClickSelectOkBtn(const CEGUI::EventArgs& args);
    bool OnSelectItem(const CEGUI::EventArgs& args);

    bool OnClickBackBtn(const CEGUI::EventArgs& e);
    bool OnClickStartBtn(const CEGUI::EventArgs& e);
    bool OnClickSelectAudioBtn(const CEGUI::EventArgs& e);
    bool OnClickSelectBRFBtn(const CEGUI::EventArgs& e);

    DECLARE_FSM(CTitlePage);
    DECLARE_FSM_STATE(CTitlePage, Welcome);
    DECLARE_FSM_STATE(CTitlePage, CreateTrack);
    DECLARE_FSM_STATE(CTitlePage, PlayTrack);

private:
    CTitleTask* m_pOwner;
    std::vector<CEGUI::ItemEntry*> m_listEntries;
};


#endif