
/**
*	Copyright (C) 2014, All right reserved
*	created:	2014/03/26
*	created:	26:3:2014   16:43
*	filename: 	PARTICLESYSTEM.H
*	file base:	ParticleSystem
*	file ext:	h
*	author:		GJ
*	
*	history:	
**/

#ifndef PARTICLE_PARTICLESYSYTEM_H_INCLUDE
#define PARTICLE_PARTICLESYSYTEM_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "Emitter.h"

namespace FCEngine
{
    class ParticleSystem : public CComponentBase
    {
        friend class ParticleSystemManager;
        friend class ParticleSystemScript;
        
        typedef std::vector< ParticleEmitter* > TEmitterVector;
         DECLARE_REFLECT_GUID( ParticleEmitter, 0x15440159, CComponentBase )
    public:
        ParticleSystem();

        ParticleSystem( CSerializer& serializer );

        ~ParticleSystem();

        void InitParticleSysTem( SEmitter* pEmitterProperty );

        void Update( float dtt );

        void Render( );

        void SetMvp( const kmMat4& mvp );

    private:
        TEmitterVector m_EmitterVecor;
    };

};

#endif // ParticleSystem_H_INCLUDE
