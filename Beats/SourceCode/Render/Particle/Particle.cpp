#include "stdafx.h"
#include "Particle.h"
#include "../RenderManager.h"
#include "Emitter.h"

CParticle::CParticle(size_t uID)
: m_id(uID)
, m_radius(1.0f)
, m_color(0)
, m_totalLifeTimeMs(INFINITE_VALUE)
, m_restLifeTimeMs(3000)
, m_type(ePT_Visual)
, m_curTextureIndex(0)
, m_recycleFlag(false)
{
}

CParticle::CParticle( const CParticle& rVal )
: m_id(rVal.m_id)
, m_radius(rVal.m_radius)
, m_color(rVal.m_color)
, m_totalLifeTimeMs(rVal.m_totalLifeTimeMs)
, m_restLifeTimeMs(rVal.m_restLifeTimeMs)
, m_curTextureIndex(rVal.m_curTextureIndex)
, m_recycleFlag(rVal.m_recycleFlag)
{
    GetPhysics()->SetVelocity(rVal.GetPhysics()->GetVelocity());
    for (size_t i = 0; i < rVal.m_pTextures.size(); ++i)
    {   
        rVal.m_pTextures[i]->AddRef();
        m_pTextures.push_back(rVal.m_pTextures[i]);
    }
}

CParticle::~CParticle()
{
    for (size_t i = 0; i < m_pTextures.size(); ++i)
    {
        m_pTextures[i]->Release();
    }
    m_pTextures.clear();
}

void CParticle::Update( float deltaTimeMs )
{
    if (m_totalLifeTimeMs != INFINITE_VALUE)
    {
        float result = m_restLifeTimeMs - deltaTimeMs;
        BEATS_CLIP_VALUE(result, 0, result);
        m_restLifeTimeMs = (unsigned long)(result) ;
    }
}

void CParticle::PreRender()
{
    CRenderManager::GetInstance()->GetDevice()->SetTexture(0, m_pTextures.size() > 0 ? m_pTextures[m_curTextureIndex] : NULL);
}

void CParticle::Render()
{
    
}

void CParticle::PostRender()
{
 
}

bool CParticle::IsAlive()
{
    return m_totalLifeTimeMs == INFINITE_VALUE || m_restLifeTimeMs > 0;
}

EParticleType CParticle::GetType() const
{
    return m_type;
}

void CParticle::Reset()
{
    m_restLifeTimeMs = m_totalLifeTimeMs;
}

void CParticle::SetRestLifeTime( long lifeTime )
{
    m_restLifeTimeMs = lifeTime;
}

CPhysicsElement* CParticle::GetPhysics()
{
    return &m_physicElement;
}

const CPhysicsElement* CParticle::GetPhysics() const
{
    return &m_physicElement;
}

float CParticle::GetRadius() const
{
    return m_radius;
}

void CParticle::SetRadius(float radius)
{
    BEATS_ASSERT(radius > 0, _T("Radius should be greater than 0!"));
    m_radius = radius;
}

void CParticle::SetColor( DWORD color )
{
    m_color = color;
}

DWORD CParticle::GetColor() const
{
    return m_color;
}

void CParticle::AddTexture( LPDIRECT3DTEXTURE9 pTexture )
{
    m_pTextures.push_back(pTexture);
}

LPDIRECT3DTEXTURE9 CParticle::GetTexture() const
{
    return m_pTextures[m_curTextureIndex];
}

void CParticle::SetTextureIndex( size_t index )
{
    BEATS_ASSERT(index < m_pTextures.size(), _T("Texture Index is out of bound!"));
    m_curTextureIndex = index;
}

void CParticle::SetTotalLifeTime( long lifeTime )
{
    m_totalLifeTimeMs = lifeTime;
}

void CParticle::SetRecycleFlag( bool flag )
{
    m_recycleFlag = flag;
}

bool CParticle::GetRecycleFlag()
{
    return m_recycleFlag;
}

void CParticle::SetPhysicsElement(const CPhysicsElement& physicsElement)
{
    m_physicElement = physicsElement;
}

void CParticle::OnRecycle()
{
    m_pEmitter->OnParticleRecycled(this);
}

void CParticle::SetEmitter(CEmitter* pEmitter)
{
    m_pEmitter = pEmitter;
}

CEmitter* CParticle::GetEmitter()
{
    return m_pEmitter;
}

void CParticle::OnReCreate()
{

}

long CParticle::GetID()
{
    return m_id;
}

void CParticle::SetID( long id )
{
    m_id = id;
}