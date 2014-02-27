#ifndef BEATS_GAME_TASKS_RECORDTRACK_H__INCLUDE
#define BEATS_GAME_TASKS_RECORDTRACK_H__INCLUDE

#include "TaskBase.h"

class CRecordTrackPage;
class CSoundManager;

class CRecordTrack : public CTaskBase
{
    typedef CTaskBase super;
public:
    CRecordTrack();
    virtual ~CRecordTrack();

public:
    virtual void Init();
    virtual void Update();
    virtual void UnInit();

    void Stop();
    void Play();
    void Pause();
    void Exit();

    void AddBeats(WORD keycode);
    void RemoveBeats(unsigned long playPos);
    void SetPlayTimeByProgress(float progress);
    void SetPlayTimeByPos(unsigned long pos);
    void SaveTrack(const TCHAR* path = NULL);
    float GetProgress();

private:
    unsigned long m_trackSoundId;
    CRecordTrackPage* m_pUIPage;
    CSoundManager* m_pSoundMgr;
    std::map<unsigned long, WORD> m_beatsRecord;
};

#endif