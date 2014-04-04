/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   11:53
*    filename:  POINTPARTICLEEMITTERENTITY.CPP
*    file base: PointParticleEmitterEntity
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "PointParticleEmitterEntity.h"

namespace FCEngine
{
    PointParticleEmitterEntity::PointParticleEmitterEntity()
    {
        kmVec3Fill( &m_Position, 0, 0, 0 );
        m_EmitterType = eET_POINT;
    }

    PointParticleEmitterEntity::PointParticleEmitterEntity( CSerializer& serializer ) :
        ParticleEmitterEntityBase( serializer )
    {
        kmVec3Fill( &m_Position, 0, 0, 0 );
        m_EmitterType = eET_POINT;
    }

    PointParticleEmitterEntity::~PointParticleEmitterEntity()
    {

    }

    const kmVec3& PointParticleEmitterEntity::GetBirthPositon()
    {
        kmVec3Fill( &m_BirthPosition, m_Position.x, m_Position.y, m_Position.z );

        return m_BirthPosition;
    }


    void PointParticleEmitterEntity::SetEmitterPropty( const void* pValue )
    {
        kmVec3* value = (kmVec3*)pValue;
        kmVec3Fill( &m_Position, value->x, value->y, value->z );
    }

    const EEmitterType& PointParticleEmitterEntity::GetEmitterType() const
    {
        return m_EmitterType;
    }

};
