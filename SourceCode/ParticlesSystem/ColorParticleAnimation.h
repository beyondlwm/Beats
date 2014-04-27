/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:07
*    filename:  COLORPARTICLEANIMATION.H
*    file base: ColorParticleAnimation
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_COLORPARTICLEANIMATION_H_INCLUDE
#define PARTICLE_COLORPARTICLEANIMATION_H_INCLUDE

#include "ParticleAnimationBase.h"

namespace FCEngine
{
    class ColorParticleAnimation : public ParticleAnimationBase
    {
        friend class ParticleEntity;
        DECLARE_REFLECT_GUID( ColorParticleAnimation, 0x1544B158, ParticleAnimationBase )
    public:
        ColorParticleAnimation();
        ColorParticleAnimation( CSerializer& serializer );
        ~ColorParticleAnimation();

    private:
        virtual void Update( float dtt );

        virtual const void* GetCurrentAnimationValue( );

        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 );

        float CalcCurrentValue( float begin, float end, float delta );
    private:
        CColor m_CurrentColor;
        CColor m_BeginColor;
        CColor m_EndColor;
    };
};

#endif // PARTICLE_COLORPARTICLEANIMATION_H_INCLUDE
