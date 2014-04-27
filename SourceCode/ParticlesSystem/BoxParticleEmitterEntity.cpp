/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   11:56
*    filename:  BOXPARTICLEEMITTERENTITY.CPP
*    file base: BoxParticleEmitterEntity
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "BoxParticleEmitterEntity.h"

namespace FCEngine
{
    BoxParticleEmitterEntity::BoxParticleEmitterEntity()
    {
        kmVec3Fill( &m_BoxSize, 1, 1, 1 );
        m_EmitterType = eET_BOX;
    }

    BoxParticleEmitterEntity::BoxParticleEmitterEntity( CSerializer& serializer ) :
        ParticleEmitterEntityBase( serializer )
    {
        kmVec3Fill( &m_BoxSize, 1, 1, 1 );
        m_EmitterType = eET_BOX;
        DECLARE_PROPERTY(serializer, m_BoxSize, true, 0xFFFFFFFF, _T("盒子发射器大小"), NULL, NULL, NULL);
    }

    BoxParticleEmitterEntity::~BoxParticleEmitterEntity()
    {

    }

    const kmVec3& BoxParticleEmitterEntity::GetBirthPositon()
    {
        //计算盒子中的位置
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        x *= m_BoxSize.x;
        float y = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        y *= m_BoxSize.y;
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        z *= m_BoxSize.z;
        kmVec3Fill( &m_BirthPosition, x, y, z );

        return m_BirthPosition;
    }


    void BoxParticleEmitterEntity::SetEmitterPropty( const void* pValue )
    {
        kmVec3* value = (kmVec3*)pValue;
        kmVec3Fill( &m_BoxSize, value->x, value->y, value->z );
    }

    const EEmitterType& BoxParticleEmitterEntity::GetEmitterType() const
    {
        return m_EmitterType;
    }

};