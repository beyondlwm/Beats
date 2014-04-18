/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   11:54
*    filename:  BOXPARTICLEEMITTERENTITY.H
*    file base: BoxParticleEmitterEntity
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_BOXPARTICLEEMITTERENTITY_H_INCLUDE
#define PARTICLE_BOXPARTICLEEMITTERENTITY_H_INCLUDE

#include "ParticleEmitterEntity.h"

namespace FCEngine
{
    class BoxParticleEmitterEntity : public ParticleEmitterEntityBase
    {
        DECLARE_REFLECT_GUID( BoxParticleEmitterEntity, 0x1544C157, ParticleEmitterEntityBase )
    public:
        BoxParticleEmitterEntity( );
        BoxParticleEmitterEntity( CSerializer& serializer );
        ~BoxParticleEmitterEntity( );
    private:
        virtual const kmVec3& GetBirthPositon( );

        virtual void SetEmitterPropty( const void* pValue );

        virtual const EEmitterType& GetEmitterType( ) const;
    private:
        kmVec3 m_BoxSize;//the size of the box
    };
};

#endif // PARTICLE_BOXPARTICLEEMITTERENTITY_H_INCLUDE
