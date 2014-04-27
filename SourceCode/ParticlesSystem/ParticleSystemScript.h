/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/27
*    created:    27:3:2014   9:43
*    filename:  PARTICLESYSTEMSCRIPT.h
*    file base: ParticleSystemScript
*    file ext:   h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLESYSYTEMSCRIPT_H_INCLUDE
#define PARTICLE_PARTICLESYSYTEMSCRIPT_H_INCLUDE

#include "resource/Resource.h"
#include "ParticleSystem.h"

namespace FCEngine
{
    class ParticleSystemScript : public CResource
    {
        DECLARE_RESOURCE_TYPE(eRT_ParticleScript)
         typedef std::vector< ParticleEmitter** > TEmitterVector;
    public:
        ParticleSystemScript( );
        ~ParticleSystemScript( );

        virtual bool Load( );
        virtual bool Unload( );

        void InitParticleSystem( ParticleSystem** pSystem );

    private:

        /*
        * Method:    paramScript
        
        * Access:    private 
        * Returns:   void
        * Param:	 const TCHAR * pScriptPath
        * Remark:    解析脚本文件中的参数
        //*/
        void ParamScript( const TCHAR* pScriptPath );

    private:
        SEmitter* m_pEmitter;
        unsigned int m_Count;//发射器的个数

    };
};

#endif // ParticleSystemScript_H_INCLUDE