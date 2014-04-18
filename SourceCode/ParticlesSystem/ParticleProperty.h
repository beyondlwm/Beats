/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/03
*    created:   3:4:2014   18:55
*    filename:  PARTICLEPROPERTY.H
*    file base: ParticleProperty
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLEPROPERTY_H_INCLUDE
#define PARTICLE_PARTICLEPROPERTY_H_INCLUDE

#include "ParticleAnimationBase.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

namespace FCEngine
{
    enum EParticleType
    {
        ePT_BILLBORD = 0,//布告板
        ePT_AXISALIGNED//世界坐标系中的轴对其
    };

    typedef std::vector< ParticleAnimationBase* > TParticleAnimation;

    class ParticleProperty : public CComponentBase
    {
        friend class ParticleEmitter;
        friend class ParticleEntity;
        typedef std::vector< ParticleAnimationBase* > TParticleAnimation;
        DECLARE_REFLECT_GUID( ParticleProperty, 0x1544A158, CComponentBase )
    public:
        ParticleProperty( );
        ParticleProperty( CSerializer& serializer );
        ~ParticleProperty( );

    private:

        float GetWidth( );

        float GetHeight( );

        const EParticleType& GetParticleType( );

        TParticleAnimation& GetAnimations( );

    private:

        float m_Width;//粒子的宽度
        float m_Height;//粒子的高度
        EParticleType m_ParticalType;//粒子的类型
        TParticleAnimation m_ParticleAnimations;
    };
};

#endif // PARTICLE_PARTICLEPROPERTY_H_INCLUDE
