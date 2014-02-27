#ifndef BEATS_RENDER_PARTICLE_PARTICLEMANAGER_H__INCLUDE
#define BEATS_RENDER_PARTICLE_PARTICLEMANAGER_H__INCLUDE

#include "ParticleDef.h"
class CParticle;
class CParticleManager
{
    BEATS_DECLARE_SINGLETON(CParticleManager);
public:

    void Init();
    void Update(float deltaTime);
    void SetActive(bool bActive);

    CParticle* CreateParticle();
    void Register(CParticle* pParticle);
    void UnRegister(CParticle* pParticle);

private:
    bool m_bActive;
    std::vector<CParticle*> m_particle;
    std::vector<CParticle*> m_recycleParticle;
};

#endif