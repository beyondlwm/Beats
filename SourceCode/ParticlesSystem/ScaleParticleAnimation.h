/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:10
*    filename:  SCALEPARTICLEANIMATION.H
*    file base: ScaleParticleAnimation
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_SCALEPARTICLEANIMATION_H_INCLUDE
#define PARTICLE_SCALEPARTICLEANIMATION_H_INCLUDE

#include "ParticleAnimationBase.h"

namespace FCEngine
{
    class ScaleParticleAnimation : public ParticleAnimationBase
    {
        friend class ParticleEntity;
        DECLARE_REFLECT_GUID( ScaleParticleAnimation, 0x1544B156, ParticleAnimationBase )
    public:
        ScaleParticleAnimation( CSerializer& serializer );
        ~ScaleParticleAnimation();

    private:
        ScaleParticleAnimation();
 
        virtual void Update( float dtt );

        virtual const void* GetCurrentAnimationValue( );

        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );
    private:
        float m_CurrentScale;
        float m_BeginScale;
        float m_EndScale;
    };
};

#endif // PARTICLE_SCALEPARTICLEANIMATION_H_INCLUDE