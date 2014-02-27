#ifndef BEATS_GAME_TASKS_PLAYTRACK_H__INCLUDE
#define BEATS_GAME_TASKS_PLAYTRACK_H__INCLUDE

#include "TaskBase.h"
#include "../../Utility/Math/Vector3.h"
#include "../../Render/Particle/Emitter.h"
#include "../../ai/FSM.h"
#include <queue>

class CPlayTrackPage;
class CSoundManager;
class CBaseObject;

#define COMBO_COUNTER_FOR_SHOW 8

enum EScoreType
{
    eST_Bad,
    eST_Ok,
    eST_Good,
    eST_Bingo,
    eST_Perfect,

    eST_Count,
    eST_ForceTo32Bit = 0xffffffff
};

class CPlayTrack : public CTaskBase
{
    typedef CTaskBase super;
public:
    CPlayTrack();
    virtual ~CPlayTrack();

public:
    virtual void Init();
    virtual void Update();
    virtual void UnInit();

    void HandleInput();

    void Restart();
    void Exit();

    DECLARE_FSM(CPlayTrack);
    DECLARE_FSM_STATE(CPlayTrack, Playing);
    DECLARE_FSM_STATE(CPlayTrack, Pause);


private:
    void BuildRecord();
    void ClearRecord();
    void RenderSpawnAndTargetPos();
    void PreRenderForPos();
    void InitScore();
    void ResetComboCounter();
    void Reset();

private:
    
    float m_beatSpeed;
    float m_distance;
    unsigned long m_totalScore;
    unsigned long m_trackSoundId;
    unsigned long m_curBeatIndex;
    unsigned long m_comboCounter;
    CPlayTrackPage* m_pUIPage;
    CSoundManager* m_pSoundMgr;
    CEmitter* m_pResultEmitter;
    CVector3 m_targetPos;
    CVector3 m_spawnPos;
    CVector3 m_injectDirection;

    std::vector<unsigned long> m_scores;
    std::vector<unsigned long> m_waitingTime;//in Ms
    std::vector<float> m_radius;
    std::vector<SBeatRecord*> m_beatsRecord;
    std::queue<CBaseObject*> m_beatList;
};


#endif