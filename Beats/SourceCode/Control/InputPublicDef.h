#ifndef BEATS_CONTROL_INPUTPUBLICDEF_H__INCLUDE
#define BEATS_CONTROL_INPUTPUBLICDEF_H__INCLUDE

#include "KeyboardDef.h"
#include "MousePublicDef.h"

struct SKeyTriggerType
{
    EKeyCode m_code;
    EKeyState m_state;

    SKeyTriggerType(EKeyCode code, EKeyState state)
    : m_code(code)
    , m_state(state)
    {

    }

    SKeyTriggerType(WORD id)
    {
        m_code = (EKeyCode)(id >> 8);
        m_state = (EKeyState)(id & 0x00ff);
    }

    WORD GetUniqueId()
    {
        return (WORD)((m_code << 8) + m_state);
    }
};
#endif