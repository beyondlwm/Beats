/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   11:51
*    filename:  POINTPARTICLEEMITTERENTITY.H
*    file base: PointParticleEmitterEntity
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_POINTPARTICLEEMITTERENTITY_H_INCLUDE
#define PARTICLE_POINTPARTICLEEMITTERENTITY_H_INCLUDE

#include "ParticleEmitterEntity.h"

namespace FCEngine
{
    class PointParticleEmitterEntity : public ParticleEmitterEntityBase
    {
        friend class ParticleEmitter;
        DECLARE_REFLECT_GUID(PointParticleEmitterEntity, 0x1544C159, ParticleEmitterEntityBase )
    public:
        PointParticleEmitterEntity( );
        PointParticleEmitterEntity( CSerializer& serializer );
        ~PointParticleEmitterEntity( );

    private:
        virtual const kmVec3& GetBirthPositon( );

        virtual void SetEmitterPropty( const void* pValue );

        virtual const EEmitterType& GetEmitterType( ) const;
    private:
        kmVec3 m_Position;
    };
};

#endif // PARTICLE_POINTPARTICLEEMITTERENTITY_H_INCLUDE
