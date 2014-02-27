#ifndef BEATS_GAME_TASKS_TITLETASK_H__INCLUDE
#define BEATS_GAME_TASKS_TITLETASK_H__INCLUDE

#include "TaskBase.h"

class CTitlePage;
class CTitleTask : public CTaskBase
{
    typedef CTaskBase super;
public:
    CTitleTask();
    virtual ~CTitleTask();

    virtual void Init();
    virtual void Update();
    virtual void UnInit();
    bool AcquireBeatsRecordPath();
    bool AcquireAudioFilePath();
private:
    CTitlePage* m_pUIPage;

};
#endif