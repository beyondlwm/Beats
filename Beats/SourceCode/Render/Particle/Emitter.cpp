#include "stdafx.h"
#include "Emitter.h"
#include "ParticleManager.h"

CEmitter::CEmitter(const CParticle& templateParticle, uint32_t emitCount, uint32_t interval, bool autoEmit )
: super(templateParticle)
, m_emitCount(emitCount)
, m_emitIntervalMs(interval)
, m_startTime(0.f)
, m_elapsedTime(0.f)
, m_autoEmit(autoEmit)
{
    m_type = ePT_Emitter;
}

CEmitter::~CEmitter()
{

}

void CEmitter::Update( float deltaTimeMS )
{
    super::Update(deltaTimeMS);
    if (m_autoEmit && IsAlive())
    {
        m_elapsedTime += deltaTimeMS;
        if (m_elapsedTime - m_startTime > m_emitIntervalMs)
        {
            m_startTime = m_elapsedTime;
            Emit();
        }
    }
}

void CEmitter::Emit(long textureIndex)
{
    for (uint32_t i = 0; i < m_emitCount; ++i)
    {
        CParticle* pNewParticle = CParticleManager::GetInstance()->CreateParticle();
        // Initialize the new particle by the emitter.
        pNewParticle->SetColor(m_color);
        pNewParticle->SetRadius(m_radius);
        pNewParticle->SetTotalLifeTime(m_totalLifeTimeMs);
        pNewParticle->SetPhysicsElement(m_physicElement);
        for (uint32_t i = 0; i < m_pTextures.size(); ++i)
        {        
            pNewParticle->AddTexture(m_pTextures[i]);
        }
        pNewParticle->SetTextureIndex(m_curTextureIndex);

        CParticleManager::GetInstance()->Register(pNewParticle);
        pNewParticle->GetPhysics()->SetLocalPos(GetPhysics()->GetWorldPos());
        pNewParticle->GetPhysics()->SetDirection(GetPhysics()->GetDirection());
        if (textureIndex != INVALID_TEXTURE_INDEX)
        {
            pNewParticle->SetTextureIndex(textureIndex);
        }
        pNewParticle->SetEmitter(this);
        BEATS_ASSERT(m_emittedParticle.find(pNewParticle) == m_emittedParticle.end(), _T("The particle already belongs to this emitter!"));
        m_emittedParticle.insert(pNewParticle);
    }
}


void CEmitter::OnParticleRecycled(CParticle* pParticle)
{
    BEATS_ASSERT(m_emittedParticle.find(pParticle) != m_emittedParticle.end(), _T("The particle on recyled doesn't belong to this emitter!"));
    m_emittedParticle.erase(pParticle);
}

void CEmitter::ClearAll()
{
    for (std::set<CParticle*>::iterator iter = m_emittedParticle.begin(); iter != m_emittedParticle.end(); ++iter)
    {
        (*iter)->SetRestLifeTime(0);
    }
}