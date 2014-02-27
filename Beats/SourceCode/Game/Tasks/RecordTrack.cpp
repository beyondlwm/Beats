#include "stdafx.h"
#include "RecordTrack.h"
#include "../../Sound/SoundManager.h"
#include "../Game.h"
#include "../../ui/UIManager.h"
#include "../UIPage/RecordTrackPage.h"
#include "../../Utility/TinyXML/tinyxml.h"
#include "../../Utility/UtilityManager.h"
#include "../../Control/InputManager.h"
#include "../../Function/Bind.h"

CRecordTrack::CRecordTrack()
: super(eTT_RecordTrack)
, m_trackSoundId(0)
, m_pUIPage(NULL)
, m_pSoundMgr(NULL)
{
}

CRecordTrack::~CRecordTrack()
{

}

void CRecordTrack::Init()
{
    m_pSoundMgr = CSoundManager::GetInstance();
    m_trackSoundId = m_pSoundMgr->PlaySound(CGame::GetInstance()->GetAudioFullPath());
    m_pUIPage = static_cast<CRecordTrackPage*>(CUIManager::GetInstance()->GetUIPage(eUIPN_RecordTrack));
    m_pUIPage->SetOwner(this);
    m_pUIPage->Open();
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CRecordTrack::Exit), true);
}

void CRecordTrack::Update()
{
    if (CSoundManager::GetInstance()->IsPlaying(m_trackSoundId))
    {
        float progress = m_pSoundMgr->GetProgress(m_trackSoundId);
        if (progress >= 1)
        {
            Stop();
        }
    }
}

void CRecordTrack::UnInit()
{
    m_pUIPage->Close();
    m_pUIPage = NULL;
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CRecordTrack::Exit));
}

void CRecordTrack::Stop()
{
    m_pSoundMgr->StopSound(m_trackSoundId);
    m_pUIPage->SwitchToStop();
}

void CRecordTrack::Play()
{
    m_pSoundMgr->PlaySound(m_trackSoundId);
    m_pUIPage->SwitchToRecording();
}

void CRecordTrack::Pause()
{
    m_pSoundMgr->PauseSound(m_trackSoundId);
    m_pUIPage->SwitchToPause();
}

void CRecordTrack::AddBeats(WORD keycode)
{
    //can't convert EKeyCode to str after 0x58, because EKeyCode is not
    //consecutive after 0x58, so we won't handle the rest.
    if (keycode <= 0x58)
    {
        unsigned long playPos = m_pSoundMgr->GetPlayPos(m_trackSoundId);
        std::map<unsigned long, WORD>::iterator iter = m_beatsRecord.find(playPos);
        if (iter != m_beatsRecord.end())
        {
            m_pUIPage->UpdateData(playPos, (EKeyCode)keycode);
        }
        else
        {
            m_pUIPage->AddDataToList(playPos, (EKeyCode)keycode);
        }
        m_beatsRecord[playPos] = keycode;
    }
}


void CRecordTrack::SetPlayTimeByProgress(float progress)
{
    CSoundManager::GetInstance()->SetPlayPosByProgress(m_trackSoundId, progress);
}

void CRecordTrack::SetPlayTimeByPos( unsigned long pos )
{
    CSoundManager::GetInstance()->SetPlayPos(m_trackSoundId, pos);
    float progress = m_pSoundMgr->GetProgress(m_trackSoundId);
    m_pUIPage->UpdatePlayScroll(progress);
}

void CRecordTrack::RemoveBeats( unsigned long playPos )
{
    BEATS_ASSERT(m_beatsRecord.find(playPos) != m_beatsRecord.end(), _T("Can't find playPos %d in beats record"), playPos);
    m_beatsRecord.erase(playPos);
    m_pUIPage->RemoveDataFromList(playPos);
}

void CRecordTrack::SaveTrack( const TCHAR* path )
{
    TString pathStr;
    if (path == NULL)
    {
        CUtilityManager::GetInstance()->AcquireSingleFilePath(true,
                                                        CGame::GetInstance()->GetHwnd(), 
                                                        pathStr, 
                                                        _T("Save Record File"), 
                                                        _T("Beats Record File(*.brf)\0 *.brf\0\0"),
                                                        NULL);
    }
    else
    {
        pathStr.assign(path);
    }
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    std::map<unsigned long, WORD>::iterator iter = m_beatsRecord.begin();
    char playPosBuff[16];
    char keyCodeBuff[16];
    for (; iter != m_beatsRecord.end(); ++iter)
    {
        sprintf_s(playPosBuff, "%d", iter->first); 
        sprintf_s(keyCodeBuff, "%d", iter->second); 
        TiXmlElement* pElement = new TiXmlElement("BeatsNode");
        pElement->SetAttribute("SampleIndex", playPosBuff);
        pElement->SetAttribute("KeyCode", keyCodeBuff);
        document.LinkEndChild(pElement);
    }

#ifdef _UNICODE
    char buffer[64];
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pathStr.append(_T(".brf")).c_str(), -1,(char *)buffer, 64, NULL, NULL);
    document.SaveFile(buffer);
#else
    document.SaveFile(pathStr.append(_T(".brf")).c_str());
#endif
}

float CRecordTrack::GetProgress()
{
    return m_pSoundMgr->GetProgress(m_trackSoundId);
}

void CRecordTrack::Exit()
{
    Stop();
    m_pUIPage->SwitchToExit();
}