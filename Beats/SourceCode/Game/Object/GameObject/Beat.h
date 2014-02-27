#ifndef BEATS_GAME_OBJECT_GAMEOBJECT_BEAT_H__INCLUDE
#define BEATS_GAME_OBJECT_GAMEOBJECT_BEAT_H__INCLUDE

#include "../BaseObject.h"
#include "../../../../Utility/Math/Vector3.h"
#include "../../../Control/KeyboardDef.h"
#include "../../../ai/FSM.h"

class CMusicBeat : public CBaseObject
{
    typedef CBaseObject super;
public:
    CMusicBeat(unsigned long id = INVALID_ID);
    virtual ~CMusicBeat();

    void SetDestnation(const CVector3& dest);

    void SetHitCode(EKeyCode keycode);
    EKeyCode GetHitCode() const;

    bool IsInvalid();

    unsigned long GetEnterWaitingTime();
    void SetEnterWaitingTime(unsigned long time);

    virtual void Init();
    virtual void Unint();
    virtual void Update(float timeDeltaMs);

private:
    DECLARE_FSM(CMusicBeat);
    DECLARE_FSM_STATE(CMusicBeat, Valid);
    DECLARE_FSM_STATE(CMusicBeat, Waiting);
    DECLARE_FSM_STATE(CMusicBeat, Invalid);
    DECLARE_FSM_STATE(CMusicBeat, Dead);
    long m_enterWaitingTime;
    EKeyCode m_hitCode;
    CVector3 m_destnation;
};


#endif