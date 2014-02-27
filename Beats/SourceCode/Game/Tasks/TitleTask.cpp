#include "stdafx.h"
#include "TitleTask.h"
#include "../UIPage/TitlePage.h"
#include "../../ui/UIManager.h"
#include "../../Utility/UtilityManager.h"
#include "../../Game/Game.h"
#include "../../Control/Mouse.h"

CTitleTask::CTitleTask()
: super(eTT_Title)
{

}

CTitleTask::~CTitleTask()
{

}

void CTitleTask::Init()
{
    m_pUIPage = static_cast<CTitlePage*>(CUIManager::GetInstance()->GetUIPage(eUIPN_Title));
    BEATS_ASSERT(m_pUIPage != NULL, _T("Can't get title page!"));
    m_pUIPage->SetOwner(this);
    m_pUIPage->Open();
}

void CTitleTask::Update()
{

}

void CTitleTask::UnInit()
{
    m_pUIPage->Close();
}

bool CTitleTask::AcquireBeatsRecordPath()
{
    TString path;
    //avoid click the UI button again when the sub window close. 
    CMouse::GetInstance()->LockMouseBtnState(eMB_Left, eMBS_Release, eMB_Left, eMBS_Release);

    bool result = CUtilityManager::GetInstance()->AcquireSingleFilePath(false,
        CGame::GetInstance()->GetHwnd(), 
        path,
        _T("Select Beats Record file"),
        _T("BRF(*.brf)\0 *.brf;\0\0"),
        NULL);
    if (result)
    {
        m_pUIPage->SetBRFPathText(path);
        CGame::GetInstance()->SetBRFFilePath(path);
    }
    return result;
}

bool CTitleTask::AcquireAudioFilePath()
{
    TString path;
    //avoid click the UI button again when the sub window close. 
    CMouse::GetInstance()->LockMouseBtnState(eMB_Left, eMBS_Release, eMB_Left, eMBS_Release);

    bool result = CUtilityManager::GetInstance()->AcquireSingleFilePath(false,
        CGame::GetInstance()->GetHwnd(), 
        path,
        _T("Select MP3 file"),
        _T("MP3File(*.mp3)\0 *.mp3;\0\0"),
        NULL);
    if (result)
    {
        m_pUIPage->SetAudioPathText(path);
        CGame::GetInstance()->SetAudioFullPath(path);
    }
    return result;
}
