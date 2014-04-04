/**
*	Copyright (C) 2014, All right reserved
*	created:	2014/03/24
*	created:	24:3:2014   11:40
*	filename: 	PARTICLESYSTEMMANAGER.CPP
*	file base:	ParticleSystemManager
*	file ext:	cpp
*	author:		GJ
*	
*	history:	
**/
#include "stdafx.h"
#include "ParticleSystemManager.h"

namespace FCEngine
{
    ParticleSystemManager* ParticleSystemManager::m_pInstance = NULL;

    ParticleSystemManager::ParticleSystemManager( )
    {

    }

    ParticleSystemManager::~ParticleSystemManager( )
    {
        Release( );
    }

    void ParticleSystemManager::CreateParticleSystem()
    {

    }

    void ParticleSystemManager::CreateParticleSystem( SharePtr< ParticleSystemScript > pScript )
    {
        ParticleSystem* pSystem = new ParticleSystem( );
        pScript->InitParticleSystem( &pSystem );
        m_ParticleSystemVector.push_back( pSystem );
    }

    void ParticleSystemManager::Update( const kmMat4& mvp ,float dtt )
    {

        for ( auto i : m_ParticleSystemVector )

        {
            i->SetMvp( mvp );
            i->Update( dtt );
        }
    }

    void ParticleSystemManager::Render()
    {
        for ( auto i : m_ParticleSystemVector )
        {
            i->Render( );
        }
    }

    void ParticleSystemManager::Release()
    {
        for ( auto i : m_ParticleSystemVector )
        {
            BEATS_SAFE_DELETE( i );
        }
        m_ParticleSystemVector.clear( );
    }

};


