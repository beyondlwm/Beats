/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:00
*    filename:  BALLPARTICLEEMITTERENTITY.CPP
*    file base: BallParticleEmitterEntity
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "BallParticleEmitterEntity.h"

namespace FCEngine
{
    BallParticleEmitterEntity::BallParticleEmitterEntity()
    {
        m_Radius = 1;
        m_EmitterType = eET_BALL;
    }

    BallParticleEmitterEntity::BallParticleEmitterEntity( CSerializer& serializer ) :
        ParticleEmitterEntityBase( serializer )
    {
        m_Radius = 1;
        m_EmitterType = eET_BALL;
        DECLARE_PROPERTY( serializer, m_Radius, true, 0xFFFFFFFF, _T("球发射器半径"), NULL, NULL, NULL );
    }

    BallParticleEmitterEntity::~BallParticleEmitterEntity()
    {

    }

    const kmVec3& BallParticleEmitterEntity::GetBirthPositon()
    {
        //计算球中的位置
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        kmVec3 pPosition;
        kmVec3Fill( &pPosition, x, y, z );
        kmVec3Normalize( &pPosition, &pPosition );
        kmVec3Scale( &m_BirthPosition, &pPosition, m_Radius );

        return m_BirthPosition;
    }

    void BallParticleEmitterEntity::SetEmitterPropty( const void* pValue )
    {
        float* value = (float*)pValue;
        m_Radius = *value;
    }

    const EEmitterType& BallParticleEmitterEntity::GetEmitterType() const
    {
        return m_EmitterType;
    }

};