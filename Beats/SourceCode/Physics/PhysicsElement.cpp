#include "stdafx.h"
#include "PhysicsElement.h"
#include "PhysicsManager.h"

CPhysicsElement::CPhysicsElement(CBaseObject* pOwner)
: m_uId(INVALID_ID)
, m_active(true)
, m_pParent(NULL)
, m_pOwner(pOwner)
, m_pos(0,0,0)
, m_up(0,1.f,0)
, m_direction(0,0,1.f)
, m_velocity(0,0,0)
, m_acceleration(0,0,0)
{
    CPhysicsManager::GetInstance()->Register(this);
}

CPhysicsElement::~CPhysicsElement()
{
    CPhysicsManager::GetInstance()->Unregister(this);
}

CPhysicsElement& CPhysicsElement::operator=( const CPhysicsElement& rightValue )
{
    m_pParent = rightValue.m_pParent;
    m_pos = rightValue.m_pos;
    m_velocity = rightValue.m_velocity;
    m_acceleration = rightValue.m_acceleration;
    m_direction = rightValue.m_direction;
    m_children = rightValue.m_children;
    return *this;
}

void CPhysicsElement::Update( float deltaTimeMs )
{
    if (!m_velocity.IsZero())
    {
        m_pos += m_velocity * deltaTimeMs;
    }
    if (!m_acceleration.IsZero())
    {
        m_velocity += m_acceleration;
    }
}

void CPhysicsElement::SetLocalPos( const CVector3& value )
{
    m_pos = value;
}

void CPhysicsElement::SetLocalPos( float x, float y, float z )
{
    m_pos[eVP_X] = x;
    m_pos[eVP_Y] = y;
    m_pos[eVP_Z] = z;
}

void CPhysicsElement::SetLocalPos( float value, EVectorPos pos )
{
    m_pos[pos] = value;
}

const CVector3& CPhysicsElement::GetLocalPos() const
{
    return m_pos;
}

const CVector3 CPhysicsElement::GetWorldPos() const
{
    CVector3 result = m_pos;
    if (m_pParent != NULL)
    {
        result += m_pParent->GetWorldPos();
    }
    return result;
}

void CPhysicsElement::SetWorldPos( const CVector3& value )
{
    SetLocalPos((m_pParent != NULL) ? value - m_pParent->GetWorldPos() : value);
}

void CPhysicsElement::SetWorldPos( float x, float y, float z )
{
    CVector3 value(x,y,z);
    SetWorldPos(value);    
}

void CPhysicsElement::SetWorldPos( float value, EVectorPos pos )
{
    SetLocalPos((m_pParent != NULL) ? value - m_pos[pos] : value, pos);
}

void CPhysicsElement::SetDirection( const CVector3& value )
{
    m_direction = value;
    m_direction.Normalize();
}

void CPhysicsElement::SetDirection( float x, float y, float z )
{
    m_direction[eVP_X] = x;
    m_direction[eVP_Y] = y;
    m_direction[eVP_Z] = z;
    m_direction.Normalize();
}

void CPhysicsElement::SetDirection( float value, EVectorPos pos )
{
    m_direction[pos] = value;
    m_direction.Normalize();
}

const CVector3& CPhysicsElement::GetDirection() const
{
    return m_direction;
}

void CPhysicsElement::SetUpDirection( const CVector3& value )
{
    m_up = value;
    m_up.Normalize();
}

void CPhysicsElement::SetUpDirection( float x, float y, float z )
{
    m_up[eVP_X] = x;
    m_up[eVP_Y] = y;
    m_up[eVP_Z] = z;
    m_up.Normalize();
}

void CPhysicsElement::SetUpDirection( float value, EVectorPos pos )
{
    m_up[pos] = value;
    m_up.Normalize();
}

const CVector3& CPhysicsElement::GetUpDirection() const
{
    return m_up;
}

void CPhysicsElement::SetVelocity( const CVector3& value )
{
    m_velocity = value;
}

void CPhysicsElement::SetVelocity( float x, float y, float z )
{
    m_velocity[eVP_X] = x;
    m_velocity[eVP_Y] = y;
    m_velocity[eVP_Z] = z;
}

void CPhysicsElement::SetVelocity( float value, EVectorPos pos )
{
    m_velocity[pos] = value;
}

const CVector3& CPhysicsElement::GetVelocity() const
{
    return m_velocity;
}

void CPhysicsElement::SetID(size_t uId)
{
    m_uId = uId;
}

size_t CPhysicsElement::GetID() const
{
    return m_uId;
}

CVector3& CPhysicsElement::Translate( const CVector3& value )
{
    m_pos += value;
    return m_pos;
}

CVector3& CPhysicsElement::Translate( float x, float y, float z )
{
    m_pos.x += x;
    m_pos.y += y;
    m_pos.z += z;
    return m_pos;

}

CVector3& CPhysicsElement::Translate( float value, EVectorPos pos )
{
    m_pos[pos] += value;
    return m_pos;
}

void CPhysicsElement::Attach( CPhysicsElement* pParent )
{
    BEATS_ASSERT(pParent != NULL, _T("Can't attach to NULL!"));
    if (m_pParent != pParent)
    {
        if (m_pParent != NULL)
        {
            Detach();
        }
        m_pParent = pParent;
        m_pParent->AddChild(this);
    }
}

void CPhysicsElement::Detach()
{
    BEATS_ASSERT(m_pParent != NULL, _T("Can't detach before attach!"));
    m_pParent->RemoveChild(this);
    m_pParent = NULL;
}

void CPhysicsElement::AddChild( CPhysicsElement* pChild )
{
    BEATS_ASSERT(m_children.find(pChild) == m_children.end(), _T("Can't add the same child twice!"));
    m_children.insert(pChild);
}

void CPhysicsElement::RemoveChild( CPhysicsElement* pChild )
{
    BEATS_ASSERT(m_children.find(pChild) != m_children.end(), _T("Can't find the child!"));
    m_children.erase(pChild);
}

void CPhysicsElement::DetachAllChildren()
{
    std::set<CPhysicsElement*>::iterator iter = m_children.begin();
    for (; iter != m_children.end(); ++iter)
    {
        (*iter)->SetParent(NULL);
    }
    m_children.clear();
}

void CPhysicsElement::SetParent( CPhysicsElement* pParent )
{
    m_pParent = pParent;
}

bool CPhysicsElement::IsActive() const
{
    return m_active;
}

void CPhysicsElement::SetActive(bool active)
{
    m_active = active;
}