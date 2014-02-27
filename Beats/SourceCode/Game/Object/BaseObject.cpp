#include "stdafx.h"
#include "BaseObject.h"
#include "../../Render/RenderUnit.h"
CBaseObject::CBaseObject(EGameObjectType type, long id)
: m_id(id)
, m_type(type)
, m_active(true)
, m_recycleFlag(false)
, m_pParent(NULL)
, m_pPhysics(NULL)
, m_pGraphics(NULL)
{

}
CBaseObject::~CBaseObject()
{
}

void CBaseObject::Init()
{
}

void CBaseObject::UnInit()
{
}

EGameObjectType CBaseObject::GetType()
{
    return m_type;
}

void CBaseObject::SetActive( bool active )
{
    m_active = active;
}

bool CBaseObject::IsActive()
{
    return m_active;
}

void CBaseObject::Update(float timeDeltaMs)
{
    if (m_pGraphics!= NULL)
    {
        m_pGraphics->Render(timeDeltaMs);
    }
}

void CBaseObject::SetRecycleFlag( bool recycle )
{
    m_recycleFlag = recycle;
}

bool CBaseObject::GetRecycleFlag()
{
    return m_recycleFlag;
}

unsigned long CBaseObject::GetID()
{
    return m_id;
}

void CBaseObject::SetID( unsigned long id )
{
    m_id = id;
}

CPhysicsElement* CBaseObject::GetPhysics() const
{
    return m_pPhysics;
}

void CBaseObject::OnRecycle()
{
    SetActive(false);
    UnInit();
}

void CBaseObject::OnReCreate()
{
    m_recycleFlag = false;
    m_active = true;
    Init();
}

CRenderUnit* CBaseObject::GetGraphics() const
{
    return m_pGraphics;
}