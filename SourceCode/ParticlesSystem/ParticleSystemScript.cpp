/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/27
*    created:    27:3:2014   9:43
*    filename:  PARTICLESYSTEMSCRIPT.CPP
*    file base: ParticleSystemScript
*    file ext:   cpp
*    author:        GJ
*    
*    history:    
**/
#include "stdafx.h"
#include "ParticleSystemScript.h"
#include "Utility\BeatsUtility\Serializer.h"

namespace FCEngine
{
    ParticleSystemScript::ParticleSystemScript()
    {
        m_pEmitter = 0;
    }

    ParticleSystemScript::~ParticleSystemScript()
    {
        
    }

    bool ParticleSystemScript::Load()
    {
        BEATS_ASSERT(!IsLoaded(), _T("Can't Load a skin which is already loaded!"));

        // Load From File
        ParamScript( GetFilePath().c_str() );

        return true;
     
    }

    bool ParticleSystemScript::Unload()
    {
        BEATS_SAFE_DELETE_ARRAY( m_pEmitter );
        return true;
    }

    void ParticleSystemScript::ParamScript( const TCHAR* pScriptPath )
    {
         BEATS_SAFE_DELETE_ARRAY( m_pEmitter );

         CSerializer serializer( pScriptPath );
         serializer >> m_Count;
         m_pEmitter = new SEmitter[ m_Count ];
         memset( m_pEmitter, 0, sizeof( SEmitter ) * m_Count );
         for ( unsigned int i = 0; i < m_Count; ++i )
         {
             serializer >> m_pEmitter[ i ];
         }
    }

    void ParticleSystemScript::InitParticleSystem( ParticleSystem** pSystem )
    {
        for ( unsigned int i = 0; i < m_Count; i++)
        {
            ( *pSystem )->InitParticleSysTem( &m_pEmitter[ i ] );
        }
    }
};

