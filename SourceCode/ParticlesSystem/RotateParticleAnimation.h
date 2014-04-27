/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:40
*    filename:  ROTATEPARTICLEANIMATION.H
*    file base: RotateParticleAnimation
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_ROTATEPARTICLEANIMATION_H_INCLUDE
#define PARTICLE_ROTATEPARTICLEANIMATION_H_INCLUDE

#include "ParticleAnimationBase.h"

namespace FCEngine
{
    class RotateParticleAnimation : public ParticleAnimationBase
    {
        friend class ParticleEntity;
        DECLARE_REFLECT_GUID( RotateParticleAnimation, 0x1544B157, ParticleAnimationBase )
    public:
        RotateParticleAnimation();
        RotateParticleAnimation( CSerializer& serializer );
        ~RotateParticleAnimation();

    private:

        virtual void Update( float dtt );

        virtual const void* GetCurrentAnimationValue( );

        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );

        RotateParticleAnimation& operator=( const RotateParticleAnimation& other );
    private:
        float m_CurrentRotate;
        float m_BaseRotate;
    };
};

#endif // PARTICLE_ROTATEPARTICLEANIMATION_H_INCLUDE
