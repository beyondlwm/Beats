/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   11:58
*    filename:  BALLPARTICLEEMITTERENTITY.H
*    file base: BallParticleEmitterEntity
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_BALLPARTICLEEMITTERENTITY_H_INCLUDE
#define PARTICLE_BALLPARTICLEEMITTERENTITY_H_INCLUDE

#include "ParticleEmitterEntity.h"

namespace FCEngine
{
    class BallParticleEmitterEntity : public ParticleEmitterEntityBase
    {
        friend class ParticleEmitter;
        DECLARE_REFLECT_GUID( BallParticleEmitterEntity, 0x1544C156, ParticleEmitterEntityBase )
    public:
        BallParticleEmitterEntity( );
        BallParticleEmitterEntity( CSerializer& serializer );
        ~BallParticleEmitterEntity( );

    private:
        virtual const kmVec3& GetBirthPositon( );

        virtual void SetEmitterPropty( const void* pValue );

        virtual const EEmitterType& GetEmitterType( ) const;
    private:
        float m_Radius;//the radius of the ball
    };
};

#endif // PARTICLE_BALLPARTICLEEMITTERENTITY_H_INCLUDE
