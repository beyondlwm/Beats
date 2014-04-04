/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:08
*    filename:  COLORPARTICLEANIMATION.CPP
*    file base: ColorParticleAnimation
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "ColorParticleAnimation.h"

namespace FCEngine
{
    ColorParticleAnimation::ColorParticleAnimation()
    {
        m_CurrentColor = CColor( 0.0f, 0.0f, 0.0f, 0.0f );
        m_AnimationType = eAT_COLOR;
    }

    ColorParticleAnimation::ColorParticleAnimation( CSerializer& serializer ) :
        ParticleAnimationBase( serializer )
    {
        m_CurrentColor = CColor( 0.0f, 0.0f, 0.0f, 0.0f );
        m_AnimationType = eAT_COLOR;
        DECLARE_PROPERTY( serializer, m_BeginColor, true, 0xFFFFFFFF, _T("开始颜色"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_EndColor, true, 0xFFFFFFFF, _T("结束颜色"), NULL, NULL, NULL );
    }

    ColorParticleAnimation::~ColorParticleAnimation()
    {

    }

    void ColorParticleAnimation::Update( float dtt )
    {
        __super::Update( dtt );
        //Linear discrete the color
        BEATS_ASSERT( m_TotalTime != 0, _T( "the totalTime can't be zero" ) );
        float delta = m_CurrentTime / m_TotalTime;
        float r = CalcCurrentValue( m_BeginColor.r, m_EndColor.r, delta );
        float g = CalcCurrentValue( m_BeginColor.g, m_EndColor.g, delta );
        float b = CalcCurrentValue( m_BeginColor.b, m_EndColor.b, delta );
        float a = CalcCurrentValue( m_BeginColor.a, m_EndColor.a, delta );
        //Temporarily divided by 255， change it after editor complate
        m_CurrentColor = CColor( r / 255.0f , g / 255.0f , b / 255.0f , a / 255.0f );
    }

    const void* ColorParticleAnimation::GetCurrentAnimationValue()
    {
        return &m_CurrentColor;
    }

    void ColorParticleAnimation::SetInitAnimationValue( const void* pValue, unsigned int count )
    {
        BEATS_ASSERT( count == 2, _T( "pValue must not be two CColor type" ) );
        CColor* pColor = (CColor*)pValue;
        m_BeginColor = pColor[ 0 ];
        m_EndColor = pColor[ 1 ];
    }

    float ColorParticleAnimation::CalcCurrentValue( float begin, float end, float delta )
    {
        float f = ( end - begin ) * delta + begin;
        if ( f > end )
        {
            f = end;
        }
        else if( f < begin )
        {
            f = begin;
        }
        return f;
    }
};
