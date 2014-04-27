/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/31
*    created:    31:3:2014   19:18
*    filename: PARTICLEEMITTERENTITY.H
*    file base: ParticleEmitterEntity
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLEEMITTERENTITY_H_INCLUDE
#define PARTICLE_PARTICLEEMITTERENTITY_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

namespace FCEngine
{
    enum EEmitterType
    {
        eET_POINT = 0,
        eET_BOX,
        eET_BALL,
        eET_CONE
    };

    class ParticleEmitterEntityBase : public CComponentBase
    {
        friend class ParticleEmitter;
        DECLARE_REFLECT_GUID_ABSTRACT( ParticleEmitterEntityBase, 0x1544C155, CComponentBase )
    protected:
        ParticleEmitterEntityBase( );
        ParticleEmitterEntityBase( CSerializer& serializer );
        virtual ~ParticleEmitterEntityBase( );
        
        /*
        * Method:    GetBirthPositon
        
        * Access:    virtual public 
        * Returns:   kmVec3&
        * Remark:    Get the particle birth position from any type 
        //*/
        virtual const kmVec3& GetBirthPositon( ) = 0;

        virtual const EEmitterType& GetEmitterType( ) const = 0;

        virtual void SetEmitterPropty( const void* pValue ) = 0;

     protected:
        kmVec3 m_BirthPosition;
        EEmitterType m_EmitterType;
    };
};

#endif // PARTICLE_PARTICLEEMITTERENTITY_H_INCLUDE
