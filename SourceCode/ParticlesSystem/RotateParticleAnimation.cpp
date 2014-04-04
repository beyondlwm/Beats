/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:42
*    filename:  ROTATEPARTICLEANIMATION.CPP
*    file base: RotateParticleAnimation
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "RotateParticleAnimation.h"

namespace FCEngine
{
    RotateParticleAnimation::RotateParticleAnimation()
    {
        m_CurrentRotate = 0.0f;
        m_BaseRotate = 0.0f;
        m_AnimationType = eAT_ROTATE;
    }

    RotateParticleAnimation::RotateParticleAnimation( CSerializer& serializer ) :
        ParticleAnimationBase( serializer )
    {
        m_CurrentRotate = 0.0f;
        m_BaseRotate = 0.0f;
        m_AnimationType = eAT_ROTATE;
        DECLARE_PROPERTY( serializer, m_BaseRotate, true, 0xFFFFFFFF, _T("Ðý×ªËÙ¶È"), NULL, NULL, NULL );
    }

    RotateParticleAnimation::~RotateParticleAnimation()
    {

    }

    void RotateParticleAnimation::Update( float dtt )
    {
        __super::Update( dtt );
        //Linear discrete the rotate
        BEATS_ASSERT( m_TotalTime != 0, _T( "the totalTime can't be zero" ) );
        m_CurrentRotate += dtt * m_BaseRotate;
        if ( m_CurrentRotate > MATH_PI_DOUBLE )
        {
            m_CurrentRotate = dtt * m_BaseRotate;
        }
    }

    const void* RotateParticleAnimation::GetCurrentAnimationValue()
    {
        return &m_CurrentRotate;
    }

    void RotateParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count == 1, _T( "pValue must not be one float type" ) );
        m_BaseRotate = *(float*)pValue;
        //convert degree to radian
        m_BaseRotate = m_BaseRotate / MATH_PI_DEGREE * MATH_PI;
    }
};