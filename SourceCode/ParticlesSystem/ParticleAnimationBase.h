/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   11:44
*    filename:  PARTICLEANIMATIONBASE.H
*    file base: ParticleAnimationBase
*    file ext:  h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLEANIMATIONBASE_H_INCLUDE
#define PARTICLE_PARTICLEANIMATIONBASE_H_INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

namespace FCEngine
{
    enum EAnimationType
    {
        eAT_COLOR = 0,
        eAT_SCALE,
        eAT_ROTATE
    };

    class ParticleAnimationBase : public CComponentBase
    {
        friend class ParticleEntity;
        DECLARE_REFLECT_GUID_ABSTRACT( ParticleAnimationBase, 0x1544B159, CComponentBase )
    public:
        ParticleAnimationBase( CSerializer& serializer );
        virtual ~ParticleAnimationBase();

    protected:
        ParticleAnimationBase();

        virtual void Update( float dtt );

        /*
        * Method:    GetCurrentAnimationValue
        
        * Access:    virtual public 
        * Returns:   void*
        * Remark:    this method return void* ,the type decision by the ParticleAnimation,
                     when use this method must be care the type
        //*/
        virtual const void* GetCurrentAnimationValue( ) = 0;

        /*
        * Method:    SetInitAnimationValue
        
        * Access:    virtual public 
        * Returns:   void
        * Param:	 void * pValue
        * Remark:    the input type must be fit the animation's type
        //*/
        virtual void SetInitAnimationValue( const void* pValue, unsigned int count = 1 ) = 0;

        /*
        * Method:    SetTotalTime
        
        * Access:    protected 
        * Returns:   void
        * Param:	 float time
        * Remark:    set the total time of the animation
        //*/
        void SetTotalTime( float time );

        void Reset( );

        const EAnimationType& GetAnimationType( );

    protected:
        float m_CurrentTime;
        float m_TotalTime;
        EAnimationType m_AnimationType;
    };
};

#endif // ParticleAnimationBase_H_INCLUDE