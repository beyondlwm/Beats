
/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   11:27
*    filename:  PARTICLESYSTEMMANAGER.H
*    file base: ParticleSystemManager
*    file ext:   h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLESYSYTEMMANAGER_H_INCLUDE
#define PARTICLE_PARTICLESYSYTEMMANAGER_H_INCLUDE

#include "ParticleSystemScript.h"

namespace FCEngine
{
    class ParticleSystemManager
    {
        BEATS_DECLARE_SINGLETON(ParticleSystemManager);

        typedef std::vector< ParticleSystem* > TParticleSystemVector;
    public:

        void CreateParticleSystem( );

        void CreateParticleSystem( SharePtr< ParticleSystemScript > pScript );

        void Update( const kmMat4& mvp ,float dtt );

        void Render( );

        /*
        * Method:    release
        
        * Access:    public 
        * Returns:   void
        * Remark:    退出时调用该函数，否则会内存泄露
        //*/
        void Release( );

    private:

        TParticleSystemVector m_ParticleSystemVector;
    };
};

#endif // ParticleSystemManager_H_INCLUDE
