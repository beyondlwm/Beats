
/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   10:48
*    filename:     EMITTER.H
*    file base:    Emitter
*    file ext:    h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_EMITTER_H_INCLUDE
#define PARTICLE_EMITTER_H_INCLUDE

#include "Particle.h"
#include "Render/ShaderProgram.h"
#include "Render/Material.h"
#include "BallParticleEmitterEntity.h"
#include "PointParticleEmitterEntity.h"
#include "BoxParticleEmitterEntity.h"
#include "ConeParticleEmitterEntity.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "ParticleProperty.h"

namespace FCEngine
{

    struct SEmitter
    {
        bool m_Constraint;//是否把粒子约束到自己的坐标系内
        bool m_IsLoop;//是否循环播放
        unsigned int m_TotalCount;//粒子总数
        unsigned int m_ShootVelocity;//发射速度，每一秒发射多少粒子
        float m_MinParticleVelocity;//粒子最小初始速度
        float m_MaxParticleVelocity;//粒子最大初始速度
        float m_MinVelocityDecay;//粒子速度的最小衰减系数
        float m_MaxVelocityDecay;//粒子速度的最大衰减系数
        float m_ForcePower;//粒子受到的外力大小
        float m_MinLiveTime;//粒子的最小生存时间(s)
        float m_MaxLiveTime;//粒子的最大生存时间(s)
        float m_DelayTime;//延迟发射时间(s)
        CMaterial* m_pMaterial;
        kmVec3 m_ShootOrigin;//发射原点
        kmVec3 m_ShootDeriction;//发射方向
        kmVec3 m_ShootDegree;//发射角度，分为XYZ三个方向
        kmVec3 m_ForceDeriction;//粒子收到的外力方向，如果外力大小为0，该属性不起作用

        SEmitter( )
        {
            m_TotalCount = 10;
            m_pMaterial = NULL;
            kmVec3Fill( &m_ShootOrigin, 0.0, 0.0, 0.0 );
            kmVec3Fill( &m_ShootDeriction, 0.0, 1.0, 0.0 );
            kmVec3Fill( &m_ShootDegree, 0.0, 0.0, 0.0 );
            m_ShootVelocity = 5;
            m_MinParticleVelocity = 1.0;
            m_MaxParticleVelocity = 3.0;
            m_MinVelocityDecay = 0.0;
            m_MaxVelocityDecay = 0.0;
            m_ForcePower = 0.0;
            kmVec3Fill( &m_ForceDeriction, 0.0, -1.0, 0.0 );
            m_MinLiveTime = 3.0;
            m_MaxLiveTime = 5.0;
            m_DelayTime = 0.0;
            m_Constraint = true;
            m_IsLoop = false;
        }

        ~SEmitter( )
        {

        }
    };

    class ParticleEmitter : public CComponentBase
    {
        friend class ParticleSystem;
        typedef std::list< ParticleEntity* > TParticleList;
        DECLARE_REFLECT_GUID( ParticleEmitter, 0x1544A159, CComponentBase )
    public:
        ParticleEmitter( CSerializer& serializer );
        ~ParticleEmitter( );

    private:
        ParticleEmitter( );

        /*
        * Method:    beginShoot
        
        * Access:    private 
        * Returns:   void
        * Remark:     激活发射器开始发射粒子,一般调用该函数时，所有的属性值已经设置完成
        //*/
        void BeginShoot( );

        /*
        * Method:    update
        
        * Access:    private 
        * Returns:   void
        * Param:     float dtt 渲染每一帧的时间
        * Remark:     渲染之前更新所有发射器
        //*/
        void Update( float dtt );

        void Render( );


        void SetTotalCount( unsigned int count );

        unsigned int GetTotleCount( );


        void SetShootDeriction( const kmVec3& deriction );

        const kmVec3& GetShootDeriction( ) const;


        void SetShootOrigin( const kmVec3& origin );

        const kmVec3& GetShootOrigin( ) const;


        void SetShootDegree( const kmVec3& degree );

        const kmVec3& GetShootDegree( ) const;

        /*
        * Method:    setShootVelocity
        
        * Access:    private 
        * Returns:   void
        * Param:     float velocity
        * Remark:     设置粒子的发射速度
        //*/
        void SetShootVelocity( unsigned int velocity );

        unsigned int GetShootVelocity( );

        /*
        * Method:    setMinParticleVelocity
        
        * Access:    private 
        * Returns:   void
        * Param:     float velocity
        * Remark:     设置发射出的粒子的最小运动速度
        //*/
        void SetMinParticleVelocity( float velocity );

        float GetMinParticleVelocity( );


        void SetMaxParticleVelocity( float velocity );

        float GetMaxParticleVelocity( );

        /*
        * Method:    setMinParticleVelocityDecay
        
        * Access:    private 
        * Returns:   void
        * Param:     float decay
        * Remark:     设置粒子最小衰减率
        //*/
        void SetMinParticleVelocityDecay( float decay );

        float GetMinParticleVelocityDecay( );


        void SetMaxParticleVelocityDecay( float decay );

        float GetMaxParticleVelocityDecay();


        void SetForcePower( float power );

        float GetForcePower( );


        void SetFroceDeriction( const kmVec3& deriction );

        const kmVec3& GetFroceDeriction( ) const;


        void SetMinLiveTime( float time );

        float GetMinLiveTime( );


        void SetMaxLiveTime( float time );

        float GetMaxLiveTime( );


        void SetDelayTime( float time );

        float GetDelayTime( );


        /*
        * Method:    setParticleConstraint
        
        * Access:    private 
        * Returns:   void
        * Param:     bool bConstraint true 约束在发射器坐标系内， false 粒子在世界坐标系中
        * Remark:     设置粒子是否约束在发射器的坐标系内部
        //*/
        void SetParticleConstraint( bool bConstraint );

        bool GetParticleConstraint( );

        /*
        * Method:    setEmitterAttribute
        
        * Access:    private 
        * Returns:   void
        * Param:     const Emitter & pEmitter
        * Remark:    增加一个设置所有属性的函数，用于打开文件时全部属性赋值
        //*/
        void SetEmitterAttribute( const SEmitter& pEmitter );

        const SEmitter& GetEmitterAttribute( ) const;


        void SetPlayLoop( bool bLoop );

        bool GetPlayLoop( );

    private:
        /*
        * Method:    shootParicle
        
        * Access:    private 
        * Returns:   void
        * Param:	 unsigned int count 发射粒子数
        * Remark:	 一次发射多个粒子
        //*/
        void ShootParicle( unsigned int count );

        /*
        * Method:    setParticleInitProperty
        
        * Access:    private 
        * Returns:   void
        * Param:     ParticleEntity * pParticle
        * Remark:    设置一个新生粒子的初始属性
        //*/
        void SetParticleInitProperty( ParticleEntity** pParticleEntity );

        /*
        * Method:    GetParticleFromRecycle
        
        * Access:    private 
        * Returns:   ParticleEntity*
        * Remark:     从已经回收的粒子中重新激活一个粒子
        //*/
        ParticleEntity* GetParticleFromRecycle( ); 

        /*
        * Method:    updateActiveParticles
        
        * Access:    private 
        * Returns:   void
        * Param:     unsigned int dtt
        * Remark:    更新所有已经发射出的粒子
        //*/
        void UpdateActiveParticles( float dtt );

        void InitVAOAndVBO( int count );

    private:

        SEmitter* m_pEmitter;
        CVertexPTC* m_VertData;
        unsigned short* m_IndexBuffer;
        TParticleList m_AliveParticleList;//活动粒子列表
        TParticleList m_ParticleRecycle;//回收粒子列表

        GLuint m_VAO;
        GLuint m_VBO[ 2 ];

        unsigned int m_CurrentCount;//当前已经发射的粒子个数
        float m_BeginTime;

        float m_MinPreShootTime;//发射一颗粒子的最小满足时间
        float m_DeltaTime;//上一帧发射粒子的时间差

        bool m_bInit;

        CShaderProgram* m_pParticleProgram;
        CMaterial* m_pMaterial;//所用材质

        ParticleEmitterEntityBase* m_pEmitterBase;
        ParticleProperty* m_pParticleProperty;
    };
};

#endif // Emitter_H_INCLUDE
