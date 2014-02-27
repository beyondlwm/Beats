#include "stdafx.h"
#include "PlayTrackPage.h"
#include "../Game.h"
#include "../Tasks/PlayTrack.h"
#include "../../ui/UIManager.h"

CPlayTrackPage::CPlayTrackPage()
: super(eUIPN_PlayTrack)
, m_pOwner(NULL)
{

}

CPlayTrackPage::~CPlayTrackPage()
{

}

void CPlayTrackPage::Init()
{
    char data[64];
    sprintf_s(data, "score : %d", 0);
    m_pComponents[ePTPC_ScoreText]->setText(data);

    sprintf_s(data, "counter : %d", 0);
    m_pComponents[ePTPC_ComboCounterText]->setText(data);
}

void CPlayTrackPage::InitUIEvent()
{
    m_pComponents[ePTPC_ContinueBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CPlayTrackPage::OnClickContinueBtn, this));
    m_pComponents[ePTPC_RestartBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CPlayTrackPage::OnClickRestartBtn, this));
    m_pComponents[ePTPC_ExitBtn]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CPlayTrackPage::OnClickExitBtn, this));
}

bool CPlayTrackPage::OpenImpl()
{
    BEATS_ASSERT(m_pOwner != NULL, _T("Can't Open Record Track Page without setting owner!"));
    SetAllComponentsVisible(true);
    SetVisible(ePTPC_SystemWindow, false);
    return true;
}

bool CPlayTrackPage::CloseImpl()
{
    SetAllComponentsVisible(false);
    return true;
}

void CPlayTrackPage::Update()
{

}

void CPlayTrackPage::SetOwner( CPlayTrack* pOwner )
{
    m_pOwner = pOwner;
}

bool CPlayTrackPage::OnClickContinueBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->SwitchToPlaying();
    return true;
}

bool CPlayTrackPage::OnClickRestartBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Restart();
    return true;
}

bool CPlayTrackPage::OnClickExitBtn( const CEGUI::EventArgs& /*e*/ )
{
    m_pOwner->Exit();
    return true;
}

void CPlayTrackPage::UpdateScore( unsigned long score )
{
    char data[64];
    sprintf_s(data, "score : %d", score);
    m_pComponents[ePTPC_ScoreText]->setText(data);
}

void CPlayTrackPage::UpdateComboCounter( unsigned long counter )
{
    char data[64];
    sprintf_s(data, "counter : %d", counter);
    m_pComponents[ePTPC_ComboCounterText]->setText(data);
}

void CPlayTrackPage::SetVisible( EPlayTrackPageComp type, bool visibleFlag )
{
    m_pComponents[type]->setVisible(visibleFlag);
}