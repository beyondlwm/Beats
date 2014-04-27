/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/03
*    created:   3:4:2014   19:06
*    filename:  PARTICLEPROPERTY.CPP
*    file base: ParticleProperty
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "ParticleProperty.h"

namespace FCEngine
{
    ParticleProperty::ParticleProperty()
    {
        m_Width = 1;
        m_Height = 1;
        m_ParticalType = ePT_BILLBORD;
    }

    ParticleProperty::ParticleProperty( CSerializer& serializer ) :
        CComponentBase( serializer )
    {
        m_Width = 1;
        m_Height = 1;
        m_ParticalType = ePT_BILLBORD;

        DECLARE_PROPERTY_ENUM(serializer, m_ParticalType, 0, NULL, EParticleType, true, 0xFFFFFFFF, _T("粒子类型"), NULL, NULL, NULL);
        DECLARE_PROPERTY( serializer, m_Width, true, 0xFFFFFFFF, _T("粒子宽度"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_Height, true, 0xFFFFFFFF, _T("粒子高度"), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_ParticleAnimations, true, 0xFFFFFFFF, _T("粒子动画"), NULL, NULL, NULL );
    }

    ParticleProperty::~ParticleProperty()
    {

    }

    float ParticleProperty::GetWidth()
    {
        return m_Width;
    }

    float ParticleProperty::GetHeight()
    {
        return m_Height;
    }

    const EParticleType& ParticleProperty::GetParticleType()
    {
        return m_ParticalType;
    }

    TParticleAnimation& ParticleProperty::GetAnimations()
    {
        return m_ParticleAnimations;
    }

};