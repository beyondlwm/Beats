#ifndef BEATS_GAME_TASKS_TASKBASE_H__INCLUDE
#define BEATS_GAME_TASKS_TASKBASE_H__INCLUDE

#include "TaskDef.h"

class CTaskBase
{
public:
    CTaskBase(ETaskType type);
    virtual ~CTaskBase();

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void UnInit() = 0;

    ETaskType GetType();

protected:
    ETaskType m_type;
};

#endif