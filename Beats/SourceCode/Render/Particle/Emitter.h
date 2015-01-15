#ifndef BEATS_RENDER_PARTICLE_EMITTER_EMITTER_H__INCLUDE
#define BEATS_RENDER_PARTICLE_EMITTER_EMITTER_H__INCLUDE
#include "Particle.h"

class CEmitter : public CParticle
{
    typedef CParticle super;
public:
    CEmitter(const CParticle& templateParticle, uint32_t emitCount, uint32_t interval, bool autoEmit);
    virtual ~CEmitter();

    virtual void Update(float deltaTimeMS);
    void Emit(long textureIndex = INVALID_TEXTURE_INDEX);
    void ClearAll();
    void OnParticleRecycled(CParticle* pParticle);

private:
    uint32_t m_emitCount;
    uint32_t m_emitIntervalMs;
    float m_startTime;
    float m_elapsedTime;
    bool m_autoEmit;
    std::set<CParticle*> m_emittedParticle;
};


#endif