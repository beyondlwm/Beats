/**
*    Copyright (C) 2014, All right reserved
*    created:   2014/04/02
*    created:   2:4:2014   12:02
*    filename:  CONEPARTICLEEMITTERENTITY.CPP
*    file base: ConeParticleEmitterEntity
*    file ext:  cpp
*    author:        GJ
*    
*    history:    
**/

#include "stdafx.h"
#include "ConeParticleEmitterEntity.h"

namespace FCEngine
{
    ConeParticleEmitterEntity::ConeParticleEmitterEntity()
    {
        m_Radius = 1;
        m_EmitterType = eET_CONE;
    }

    ConeParticleEmitterEntity::ConeParticleEmitterEntity( CSerializer& serializer ) :
        ParticleEmitterEntityBase( serializer )
    {
        m_Radius = 1;
        m_EmitterType = eET_CONE;
        DECLARE_PROPERTY( serializer, m_Radius, true, 0xFFFFFFFF, _T("×¶·¢ÉäÆ÷µ×Ãæ°ë¾¶"), NULL, NULL, NULL );
    }

    ConeParticleEmitterEntity::~ConeParticleEmitterEntity()
    {

    }

    const kmVec3& ConeParticleEmitterEntity::GetBirthPositon()
    {
        //calc cone bottom face position ==> y = 0
        float x = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );
        float y = 0.0f;
        float z = RANGR_RANDOM_FLOAT( -1.0f, 1.0f );

        kmVec3 pPosition;
        kmVec3Fill( &pPosition, x, y, z );
        kmVec3Normalize( &pPosition, &pPosition );
        kmVec3Scale( &m_BirthPosition, &pPosition, m_Radius );

        return m_BirthPosition;
    }

    void ConeParticleEmitterEntity::SetEmitterPropty( const void* pValue )
    {
        float* value = (float*)pValue;
        m_Radius = *value;
        m_Radius *= 0.5f;
    }

    const EEmitterType& ConeParticleEmitterEntity::GetEmitterType() const
    {
        return m_EmitterType;
    }

};
