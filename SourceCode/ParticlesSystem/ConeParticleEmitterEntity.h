/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:01
*    filename:  CONEPARTICLEEMITTERENTITY.H
*    file base: ConeParticleEmitterEntity
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_BCONEPARTICLEEMITTERENTITY_H_INCLUDE
#define PARTICLE_BCONEPARTICLEEMITTERENTITY_H_INCLUDE

#include "ParticleEmitterEntity.h"

namespace FCEngine
{
    class ConeParticleEmitterEntity : public ParticleEmitterEntityBase
    {
        friend class ParticleEmitter;
        DECLARE_REFLECT_GUID(ConeParticleEmitterEntity, 0x1544C158, ParticleEmitterEntityBase)
    public:
        ConeParticleEmitterEntity();
        ConeParticleEmitterEntity( CSerializer& serializer );
        ~ConeParticleEmitterEntity();
    private:

        virtual const kmVec3& GetBirthPositon( );

        virtual void SetEmitterPropty( const void* pValue );

        virtual const EEmitterType& GetEmitterType( ) const;
    private:
        float m_Radius;// the radius of the cone's bottom face
    };
};

#endif // PARTICLE_BCONEPARTICLEEMITTERENTITY_H_INCLUDE
