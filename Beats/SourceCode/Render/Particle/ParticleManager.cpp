#include "stdafx.h"
#include "ParticleManager.h"
#include "Emitter.h"
#include "../RenderPublicDef.h"
#include "../RenderManager.h"
#include "../../Function/Bind.h"

CParticleManager* CParticleManager::m_pInstance = NULL;

CParticleManager::CParticleManager()
{

}

CParticleManager::~CParticleManager()
{
    for (uint32_t i = 0; i < m_particle.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_particle[i]);
    }
    for (uint32_t i = 0; i < m_recycleParticle.size(); ++i)
    {
        BEATS_SAFE_DELETE(m_recycleParticle[i]);
    }
}

void CParticleManager::Init()
{

}

void CParticleManager::Update( float deltaTime )
{
    for (uint32_t i = 0; i < m_particle.size(); ++i)
    {
        CParticle* pParticle = m_particle[i];
        if (pParticle != NULL)
        {
            if (pParticle->IsAlive())
            {
                pParticle->Update(deltaTime);
                if (pParticle->GetType() == ePT_Visual)
                {
                    Function<void(void)> func = Bind(*pParticle, &CParticle::PreRender);
                    CRenderManager::GetInstance()->SendQuadToCache(pParticle->GetPhysics()->GetWorldPos(),
                        pParticle->GetRadius(), 
                        pParticle->GetRadius(), 
                        pParticle->GetColor(), 
                        &func);
                }
            }
            else
            {
                UnRegister(pParticle);
            }
        }
    }
}

void CParticleManager::SetActive(bool bActive)
{
    m_bActive = bActive;
}

CParticle* CParticleManager::CreateParticle()
{
     return new CParticle(INVALID_ID);
}

void CParticleManager::Register(CParticle* pParticle)
{
    uint32_t uId = pParticle->GetID();
    if (uId == INVALID_ID)
    {
        pParticle->SetID(m_particle.size());
        m_particle.push_back(pParticle);
    }
    else
    {
        BEATS_ASSERT(uId < m_particle.size(), _T("Invalid particle id %d"), uId);
        BEATS_ASSERT(m_particle[uId] == NULL, _T("Can't register a particle twice!"));
        m_particle[uId] = pParticle;
    }
}

void CParticleManager::UnRegister(CParticle* pParticle)
{
    uint32_t uId = pParticle->GetID();
    m_recycleParticle.push_back(pParticle);

    if (uId != INVALID_ID)
    {
        m_particle[uId] = NULL;
    }
}

