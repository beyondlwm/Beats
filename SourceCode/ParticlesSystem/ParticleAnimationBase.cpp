
/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   11:45
*    filename:  PARTICLEANIMATIONBASE.CPP
*    file base: ParticleAnimationBase
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/
#include "stdafx.h"
#include "ParticleAnimationBase.h"

namespace FCEngine
{

    ParticleAnimationBase::ParticleAnimationBase()
    {
        m_CurrentTime = 0;
        m_TotalTime = 0;
    }

    ParticleAnimationBase::ParticleAnimationBase( CSerializer& serializer ) :
        CComponentBase( serializer )
    {

    }

    ParticleAnimationBase::~ParticleAnimationBase()
    {

    }

    void ParticleAnimationBase::Update( float dtt )
    {
        m_CurrentTime += dtt;
    }

    void ParticleAnimationBase::SetTotalTime( float time )
    {
        m_TotalTime = time;
    }

    void ParticleAnimationBase::Reset()
    {
        m_CurrentTime = 0;
    }

    const EAnimationType& ParticleAnimationBase::GetAnimationType()
    {
        return m_AnimationType;
    }

}


