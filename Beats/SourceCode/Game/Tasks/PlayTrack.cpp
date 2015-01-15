#include "stdafx.h"
#include "PlayTrack.h"
#include "../../Sound/SoundManager.h"
#include "../../ui/UIManager.h"
#include "../UIPage/PlayTrackPage.h"
#include "../../Utility/UtilityManager.h"
#include "../../Game/Game.h"
#include "../../Utility/TinyXML/tinyxml.h"
#include "../../Game/Object/GameObject/Beat.h"
#include "../../Game/Object/GameObjectDef.h"
#include "../../Physics/PhysicsElement.h"
#include "../../Render/RenderManager.h"
#include "../../Control/InputManager.h"
#include "../../Function/Bind.h"
#include "../../Physics/PhysicsManager.h"
#include "../../Control/Keyboard.h"
#include "../../Control/KeyboardDef.h"
#include "../../Render/Particle/ParticleManager.h"
#include "../../Render/RenderUnit.h"
#include "../../Render/Particle/Emitter.h"
#include "../Object/ObjectManager.h"


CPlayTrack::CPlayTrack()
: super(eTT_PlayTrack)
, m_beatSpeed(0.1f)
, m_distance(0)
, m_trackSoundId(0)
, m_curBeatIndex(0)
, m_totalScore(0)
, m_comboCounter(0)
, m_pUIPage(NULL)
, m_pSoundMgr(CSoundManager::GetInstance())
, m_pResultEmitter(NULL)
, m_targetPos(0, 0, 5.0f)
, m_spawnPos(- (BEATS_WINDOW_WIDTH * 0.5f) - 10, 0, 5.0f)
, m_injectDirection(0,0,0)
{
    IDirect3DDevice9* pRenderDevice = CRenderManager::GetInstance()->GetDevice();
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    CParticle templateParticle(INVALID_ID);
    const TString& strWorkRootPath = CGame::GetInstance()->GetWorkRootPath();
    SetCurrentDirectory(strWorkRootPath.c_str());
    D3DXCreateTextureFromFile(pRenderDevice, _T("Texture\\bad.JPG"), &pTexture);
    BEATS_ASSERT(pTexture != NULL, _T("Create Texture Failed!"));
    templateParticle.AddTexture(pTexture);
    D3DXCreateTextureFromFile(pRenderDevice, _T("Texture\\ok.JPG"), &pTexture);
    BEATS_ASSERT(pTexture != NULL, _T("Create Texture Failed!"));

    templateParticle.AddTexture(pTexture);
    D3DXCreateTextureFromFile(pRenderDevice, _T("Texture\\good.JPG"), &pTexture);
    BEATS_ASSERT(pTexture != NULL, _T("Create Texture Failed!"));

    templateParticle.AddTexture(pTexture);
    D3DXCreateTextureFromFile(pRenderDevice, _T("Texture\\bingo.JPG"), &pTexture);
    BEATS_ASSERT(pTexture != NULL, _T("Create Texture Failed!"));

    templateParticle.AddTexture(pTexture);
    D3DXCreateTextureFromFile(pRenderDevice, _T("Texture\\perfect.JPG"), &pTexture);
    BEATS_ASSERT(pTexture != NULL, _T("Create Texture Failed!"));
    templateParticle.AddTexture(pTexture);

    templateParticle.SetTotalLifeTime(3000);
    templateParticle.SetRestLifeTime(3000);
    templateParticle.GetPhysics()->SetVelocity(CVector3(0,0.1f,0));
    templateParticle.SetRadius(45);
    templateParticle.SetColor(0xffffffff);
    m_pResultEmitter = new CEmitter(templateParticle, 1, 0, false);
}

CPlayTrack::~CPlayTrack()
{
    ClearRecord();
    BEATS_SAFE_DELETE(m_pResultEmitter);
}

void CPlayTrack::Init()
{
    m_pUIPage = static_cast<CPlayTrackPage*>(CUIManager::GetInstance()->GetUIPage(eUIPN_PlayTrack));
    m_pUIPage->SetOwner(this);
    m_pUIPage->Open();
    InitScore();
    ResetComboCounter();
    BuildRecord();
    m_trackSoundId = m_pSoundMgr->PlaySound(CGame::GetInstance()->GetAudioFullPath());
    m_curBeatIndex = 0;
    INIT_FSM(CPlayTrack, Playing);
}

void CPlayTrack::Update()
{
    UPDATE_FSM();
    RenderSpawnAndTargetPos();
}

void CPlayTrack::UnInit()
{
    Reset();
    EXIT_FSM();
    m_pSoundMgr->StopSound(m_trackSoundId);
    m_pUIPage->Close();
    m_pUIPage = NULL;
}

void CPlayTrack::BuildRecord()
{
    ClearRecord();
    const TString& brfFilePath = CGame::GetInstance()->GetBRFFilePath();
    BEATS_ASSERT(brfFilePath.empty() == false, _T("Can't phase BRF file which is not existing!"));
    char szBuffer[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToCHAR(brfFilePath.c_str(), szBuffer, MAX_PATH);
    TiXmlDocument document(szBuffer);
    bool loadSuccess = document.LoadFile();
    BEATS_ASSERT(loadSuccess, _T("Bad BRF file: %s!"), brfFilePath.c_str());
    m_beatsRecord.clear();
    TiXmlElement* pRootElement = document.RootElement();
    TiXmlElement* pCurElement = pRootElement;
    while (pCurElement != NULL)
    {
        SBeatRecord* record = new SBeatRecord();
        pCurElement->Attribute("SampleIndex", (int*)(&(record->m_sampleIndex)));
        pCurElement->Attribute("KeyCode", (int*)(&(record->m_keyCode)));
        m_beatsRecord.push_back(record);
        pCurElement = pCurElement->NextSiblingElement();
    }
}

void CPlayTrack::ClearRecord()
{
    BEATS_SAFE_DELETE_VECTOR(m_beatsRecord);
}

void CPlayTrack::RenderSpawnAndTargetPos()
{
    CRenderManager::GetInstance()->SendQuadToCache(m_targetPos, 4, 4, 0xffffffff, NULL);

    //draw the virtual line.
    Function<void(void)> function = Bind(*this, &CPlayTrack::PreRenderForPos);
    for (uint32_t i = eST_Ok; i < eST_Count; ++i)
    {
        unsigned long color = (0xffffffff<<(i*5));
        CRenderManager::GetInstance()->SendQuadToCache(m_targetPos, m_radius[i] * 2, m_radius[i] * 2, color, &function);
    }
}

void CPlayTrack::PreRenderForPos()
{
    IDirect3DDevice9* pDevice = CRenderManager::GetInstance()->GetDevice();
    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    pDevice->SetTexture(0, NULL);
}

void CPlayTrack::Restart()
{
    Reset();
    SWITCH_FSM_STATE(true, CPlayTrack, Playing);
}

void CPlayTrack::Exit()
{
    CObjectManager::GetInstance()->RemoveAllObjects(true);
    CGame::GetInstance()->SetTask(eTT_Title);
}

void CPlayTrack::HandleInput()
{
    CKeyboard* pKeyboard = CKeyboard::GetInstance();
    bool spaceUp = pKeyboard->GetKeyState(eKC_SPACE) == eKS_JustRelease;
    bool RControlUp = pKeyboard->GetKeyState(eKC_RCONTROL) == eKS_JustRelease;

    if (spaceUp || RControlUp)
    {
        if (m_beatList.size() > 0)
        {
            CMusicBeat* pNearestBeat = static_cast<CMusicBeat*>(m_beatList.front());
            float distance = fabs(pNearestBeat->GetPhysics()->GetWorldPos().x - m_targetPos.x);
            BEATS_CLIP_VALUE(distance, 0, distance);
            m_pResultEmitter->GetPhysics()->SetWorldPos(pNearestBeat->GetPhysics()->GetWorldPos());
            bool hitRightKey = pNearestBeat->GetHitCode() == eKC_SPACE ? spaceUp : RControlUp;
            if (!hitRightKey || distance > m_radius[eST_Ok])
            {
                m_pResultEmitter->Emit(eST_Bad);
                ResetComboCounter();
            }
            else
            {
                unsigned long beatWaitingTime = pNearestBeat->GetEnterWaitingTime();
                bool inWaiting = beatWaitingTime > 0;
                if (inWaiting)
                {
                    unsigned long deltaTime = GetTickCount() - beatWaitingTime;
                    for (uint32_t i = eST_Count - 1; i > 0; --i)
                    {
                        if (deltaTime < m_waitingTime[i])
                        {
                            m_pResultEmitter->GetPhysics()->SetWorldPos(pNearestBeat->GetPhysics()->GetWorldPos());
                            m_pResultEmitter->Emit((long)i);

                            m_totalScore += m_scores[i];
                            ++m_comboCounter;
                            m_pUIPage->UpdateScore(m_totalScore);
                            if (m_comboCounter > COMBO_COUNTER_FOR_SHOW)
                            {
                                m_pUIPage->UpdateComboCounter(m_comboCounter);
                                m_pUIPage->SetVisible(ePTPC_ComboCounterText, true);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    for (uint32_t i = eST_Count - 1; i > 0; --i)
                    {
                        if (distance <= m_radius[i])
                        {
                            m_pResultEmitter->GetPhysics()->SetWorldPos(pNearestBeat->GetPhysics()->GetWorldPos());
                            m_pResultEmitter->Emit((long)i);

                            m_totalScore += m_scores[i];
                            ++m_comboCounter;
                            m_pUIPage->UpdateScore(m_totalScore);
                            if (m_comboCounter > COMBO_COUNTER_FOR_SHOW)
                            {
                                m_pUIPage->UpdateComboCounter(m_comboCounter);
                                m_pUIPage->SetVisible(ePTPC_ComboCounterText, true);
                            }
                            break;
                        }
                    }
                }
                pNearestBeat->SwitchToDead();
                m_beatList.pop();
            }
        }
    }
}

void CPlayTrack::ResetComboCounter()
{
    m_comboCounter = 0;
    m_pUIPage->SetVisible(ePTPC_ComboCounterText, false);
}

void CPlayTrack::InitScore()
{
    for (uint32_t i = 0; i < eST_Count; ++i)
    {
        m_scores.push_back(0);
        m_waitingTime.push_back(0);
        m_radius.push_back(0);
    }
    m_scores[eST_Bad] = 0;
    m_scores[eST_Ok] = 50;
    m_scores[eST_Good] = 100;
    m_scores[eST_Bingo] = 180;
    m_scores[eST_Perfect] = 300;

    m_radius[eST_Bad] = 1000000.f;
    m_radius[eST_Ok] = 75.f;
    m_radius[eST_Good] = 50.f;
    m_radius[eST_Bingo] = 27.f;
    m_radius[eST_Perfect] = 12.f;

    m_waitingTime[eST_Bad] = 2000;
    m_waitingTime[eST_Ok] = 1200;
    m_waitingTime[eST_Good] = 950;
    m_waitingTime[eST_Bingo] = 300;
    m_waitingTime[eST_Perfect] = 150;
}

void CPlayTrack::Reset()
{
    m_curBeatIndex = 0;
    m_pSoundMgr->StopSound(m_trackSoundId);
    CObjectManager::GetInstance()->RemoveAllObjects(true);
    m_totalScore = 0;
    m_curBeatIndex = 0;
    m_beatList = std::queue<CBaseObject*>(); // clear
    m_pResultEmitter->ClearAll();
}

IMPL_FSM_BEGIN(CPlayTrack, Playing)
IMPL_FSM_ENTER
{
    m_pSoundMgr->PlaySound(m_trackSoundId);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CPlayTrack::SwitchToPause), true);
    m_distance = (m_spawnPos - m_targetPos).Length();
    m_injectDirection = (m_targetPos - m_spawnPos).Normalize();
}
IMPL_FSM_UPDATE
{    
    if (m_curBeatIndex < m_beatsRecord.size())
    {
        SBeatRecord* pCurRecord = m_beatsRecord[m_curBeatIndex];
        unsigned long arriveTime = m_pSoundMgr->GetTimeMsByPos(m_trackSoundId, pCurRecord->m_sampleIndex);
        unsigned long currentTime = m_pSoundMgr->GetPlayTimeMs(m_trackSoundId);
        long timeMsDelta = arriveTime - currentTime;
        if ((timeMsDelta > 0) && (timeMsDelta * m_beatSpeed <= m_distance))
        {
            CMusicBeat* pBeat = static_cast<CMusicBeat*>(CObjectManager::GetInstance()->CreateObject(eGOT_Beat));
            if (pBeat)
            {
                CObjectManager::GetInstance()->Register(pBeat);
                m_beatList.push(pBeat);
                pBeat->GetPhysics()->SetVelocity(m_injectDirection * m_beatSpeed);
                pBeat->SetHitCode((EKeyCode)(pCurRecord->m_keyCode));
                pBeat->GetGraphics()->SetColor((pCurRecord->m_keyCode == eKC_SPACE) ? 0xffff0000 : 0xffffff00);
                CVector3 realPos = m_spawnPos;
                realPos.x += m_distance - timeMsDelta * m_beatSpeed;
                pBeat->GetPhysics()->SetWorldPos(realPos);
                pBeat->SetDestnation(m_targetPos);
                ++m_curBeatIndex;
            }
        }
        if (timeMsDelta < 0 )
        {
            ++m_curBeatIndex;
        }
        while ((m_beatList.size() > 0))
        {
            CMusicBeat* pFrontBeat = static_cast<CMusicBeat*>(m_beatList.front());
            if (!pFrontBeat->IsInvalid())
            {
                pFrontBeat->GetGraphics()->SetActive(true);
                break;
            }
            m_beatList.pop();
        }
    }
    HandleInput();
}
IMPL_FSM_EXIT
{
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CPlayTrack::SwitchToPause));
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CPlayTrack, Pause)
IMPL_FSM_ENTER
{
    m_pSoundMgr->PauseSound(m_trackSoundId);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CPlayTrack::SwitchToPlaying), true);
    CPhysicsManager::GetInstance()->SetActive(false);
    CParticleManager::GetInstance()->SetActive(false);
    m_pUIPage->SetVisible(ePTPC_SystemWindow, true);
}
IMPL_FSM_EXIT
{
    CInputManager::GetInstance()->UnRegisterKeyBoardCallback(SKeyTriggerType(eKC_ESCAPE, eKS_JustRelease), Bind(*this, &CPlayTrack::SwitchToPlaying));
    CPhysicsManager::GetInstance()->SetActive(true);
    CParticleManager::GetInstance()->SetActive(true);
    m_pUIPage->SetVisible(ePTPC_SystemWindow, false);
}
IMPL_FSM_END
