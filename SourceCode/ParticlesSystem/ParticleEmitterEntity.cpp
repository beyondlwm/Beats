/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/31
*    created:    31:3:2014   19:33
*    filename:  PARTICLEEMITTERENTITY.CPP
*    file base: ParticleEmitterEntity
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/
#include "stdafx.h"
#include "ParticleEmitterEntity.h"

namespace FCEngine
{
    ParticleEmitterEntityBase::ParticleEmitterEntityBase()
    {
        kmVec3Fill( &m_BirthPosition, 0, 0, 0 );
    }

    ParticleEmitterEntityBase::ParticleEmitterEntityBase( CSerializer& serializer ) :
        CComponentBase( serializer )
    {

    }

    ParticleEmitterEntityBase::~ParticleEmitterEntityBase()
    {

    }

};
