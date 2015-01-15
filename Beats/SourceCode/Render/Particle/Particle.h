#ifndef BEATS_RENDER_PARTICLE_PARTICLE_H__INCLUDE
#define BEATS_RENDER_PARTICLE_PARTICLE_H__INCLUDE

#include "ParticleDef.h"
#include "../../Physics/PhysicsElement.h"

class CEmitter;

class CParticle
{
public:
    CParticle(uint32_t uID);
    CParticle(const CParticle& rVal);

    virtual ~CParticle();

    bool IsAlive();
    void SetRestLifeTime(long lifeTime);
    void SetTotalLifeTime(long lifeTime);
    EParticleType GetType() const;
    float GetRadius() const;
    void SetRadius(float radius);

    DWORD GetColor() const;
    void SetColor(DWORD color);

    CPhysicsElement* GetPhysics();
    const CPhysicsElement* GetPhysics() const;

    void AddTexture(LPDIRECT3DTEXTURE9 pTexture);
    void SetTextureIndex(uint32_t index);
    LPDIRECT3DTEXTURE9 GetTexture() const;

    void SetRecycleFlag(bool flag);
    bool GetRecycleFlag();

    void SetPhysicsElement(const CPhysicsElement& physicsElement);

    virtual void PreRender();
    virtual void Render();
    virtual void PostRender();

    virtual void Update(float deltaTimeMs);
    virtual void Reset();
    virtual void Init(){}

    void OnRecycle();
    void OnReCreate();
    void SetEmitter(CEmitter* pEmitter);
    CEmitter* GetEmitter();

    long GetID();
    void SetID(long id);
protected:
    long m_id;
    float m_radius;
    DWORD m_color;
    long m_totalLifeTimeMs;
    unsigned long m_restLifeTimeMs;
    EParticleType m_type;
    uint32_t m_curTextureIndex;
    bool m_recycleFlag;
    CEmitter* m_pEmitter;
    std::vector<LPDIRECT3DTEXTURE9> m_pTextures;
    CPhysicsElement m_physicElement;
};

#endif