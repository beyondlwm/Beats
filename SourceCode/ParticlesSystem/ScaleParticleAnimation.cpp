/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:39
*    filename:  SCALEPARTICLEANIMATION.CPP
*    file base: ScaleParticleAnimation
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "ScaleParticleAnimation.h"

namespace FCEngine
{
    ScaleParticleAnimation::ScaleParticleAnimation()
    {
        m_BeginScale = 1.0;
        m_EndScale = 1.0;
        m_CurrentScale = 1.0f;
        m_AnimationType = eAT_SCALE;
    }

    ScaleParticleAnimation::ScaleParticleAnimation( CSerializer& serializer ) :
        ParticleAnimationBase( serializer )
    {
        m_BeginScale = 1.0;
        m_EndScale = 1.0;
        m_CurrentScale = 1.0f;
        m_AnimationType = eAT_SCALE;
        DECLARE_PROPERTY( serializer, m_BeginScale, true, 0xFFFFFFFF, _T("开始放缩倍数"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_EndScale, true, 0xFFFFFFFF, _T("结束放缩倍数"), NULL, NULL, NULL );
    }

    ScaleParticleAnimation::~ScaleParticleAnimation()
    {

    }

    void ScaleParticleAnimation::Update( float dtt )
    {
        __super::Update( dtt );
        //Linear discrete the scale
        BEATS_ASSERT( m_TotalTime != 0, _T( "the totalTime can't be zero" ) );
        float cursor = m_CurrentTime / m_TotalTime;
        m_CurrentScale = ( m_EndScale - m_BeginScale ) * cursor + m_BeginScale;
    }

    const void* ScaleParticleAnimation::GetCurrentAnimationValue()
    {
        return &m_CurrentScale;
    }

    void ScaleParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count == 2, _T( "pValue must not be two float type" ) );
        float* pRelValue = (float*)pValue;
        m_BeginScale = pRelValue[ 0 ];
        m_EndScale = pRelValue[ 1 ];
    }
};