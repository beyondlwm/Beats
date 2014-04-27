
/**
*	Copyright (C) 2014, All right reserved
*	created:	2014/03/26
*	created:	26:3:2014   16:43
*	filename: 	PARTICLESYSTEM.CPP
*	file base:	ParticleSystem
*	file ext:	cpp
*	author:		GJ
*	
*	history:	
**/
#include "stdafx.h"
#include "ParticleSystem.h"

namespace FCEngine
{
    ParticleSystem::ParticleSystem()
    {

    }

    ParticleSystem::ParticleSystem( CSerializer& serializer ) :
        CComponentBase( serializer )
    {
        DECLARE_PROPERTY( serializer, m_EmitterVecor, true, 0xFFFFFFFF, _T("Á£×ÓÏµÍ³"), NULL, NULL, NULL );
    }

    ParticleSystem::~ParticleSystem()
    {
        for ( auto i : m_EmitterVecor )
        {
            BEATS_SAFE_DELETE( i );
        }
    }

    void ParticleSystem::InitParticleSysTem( SEmitter* pEmitterProperty  )
    {
        ParticleEmitter* pEmitter = new ParticleEmitter( );
        pEmitter->SetEmitterAttribute( *pEmitterProperty );
        m_EmitterVecor.push_back( pEmitter );
    }

    void ParticleSystem::Update( float dtt )
    {
        for ( auto i : m_EmitterVecor )
        {
            i->Update( dtt );
        }
    }

    void ParticleSystem::Render()
    {
        for ( auto i : m_EmitterVecor )
        {
            i->Render( );
        }
    }

    void ParticleSystem::SetMvp( const kmMat4& mvp )
    {

    }

};
