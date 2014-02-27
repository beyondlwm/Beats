#include "stdafx.h"
#include "Beat.h"
#include "../../../Physics/PhysicsElement.h"
#include "../../../Render/RenderUnit.h"
#include "../ObjectManager.h"
#include "../../../Game/Game.h"

CMusicBeat::CMusicBeat(unsigned long id)
: super (eGOT_Beat, id)
, m_enterWaitingTime(0)
, m_hitCode(eKC_ESCAPE)
{
    m_pPhysics = new CPhysicsElement(this);
    m_pGraphics = new CRenderUnit(this);
}

CMusicBeat::~CMusicBeat()
{
    BEATS_SAFE_DELETE(m_pPhysics);
    BEATS_SAFE_DELETE(m_pGraphics);
}

void CMusicBeat::Update(float timeDeltaMs)
{
    super::Update(timeDeltaMs);
    UPDATE_FSM();
}

void CMusicBeat::SetDestnation(const CVector3& dest)
{
    m_destnation = dest;
}

void CMusicBeat::SetHitCode( EKeyCode keycode )
{
    m_hitCode = keycode;
}

EKeyCode CMusicBeat::GetHitCode() const
{
    return m_hitCode;
}

void CMusicBeat::Init()
{
    INIT_FSM(CMusicBeat, Valid);
}

void CMusicBeat::Unint()
{
    EXIT_FSM();
}

bool CMusicBeat::IsInvalid()
{
    return IS_IN_STATE(CMusicBeat, Invalid);
}

unsigned long CMusicBeat::GetEnterWaitingTime()
{
    return m_enterWaitingTime;
}

void CMusicBeat::SetEnterWaitingTime( unsigned long time )
{
    m_enterWaitingTime = time;
}

IMPL_FSM_BEGIN(CMusicBeat, Valid)
IMPL_FSM_UPDATE
{
    SWITCH_FSM_STATE(m_enterWaitingTime > 0, CMusicBeat, Waiting);
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CMusicBeat, Waiting)
IMPL_FSM_ENTER
{
    m_pPhysics->SetActive(false);
    m_pGraphics->SetActive(false);
    m_enterWaitingTime = GetTickCount();
}
IMPL_FSM_UPDATE
{
    SWITCH_FSM_STATE((GetTickCount() - m_enterWaitingTime) > 2000, CMusicBeat, Invalid);
}
IMPL_FSM_EXIT
{
    m_enterWaitingTime = 0;
    m_pPhysics->SetActive(true);
    m_pGraphics->SetActive(true);
};
IMPL_FSM_END

IMPL_FSM_BEGIN(CMusicBeat, Invalid)
IMPL_FSM_ENTER
{
    GetGraphics()->SetColor(0xff0000ff);
}
IMPL_FSM_UPDATE
{
    bool outOfScope = GetPhysics()->GetWorldPos().x > (m_destnation.x + 0.5f * BEATS_WINDOW_WIDTH);
    SWITCH_FSM_STATE(outOfScope, CMusicBeat, Dead);
}
IMPL_FSM_END

IMPL_FSM_BEGIN(CMusicBeat, Dead)
IMPL_FSM_ENTER
{
    CObjectManager::GetInstance()->Unregister(this);
}
IMPL_FSM_END