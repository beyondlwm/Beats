
/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   10:47
*    filename:     PARTICLE.H
*    file base:    Particle
*    file ext:    h
*    author:        GJ
*    
*    history:    
**/

#ifndef PARTICLE_PARTICLE_H_INCLUDE
#define PARTICLE_PARTICLE_H_INCLUDE

#include "ColorParticleAnimation.h"
#include "RotateParticleAnimation.h"
#include "ScaleParticleAnimation.h"
#include "ParticleProperty.h"

namespace FCEngine
{

    struct SParticle
    {
        float m_Velocity;//运动速度(s)
        float m_VelocityDecay;//速度衰减系数
        float m_ForcePower;//所受外力大小
        float m_LiveTime;//粒子生存时间，毫秒为单位
        kmVec3 m_ForceDirection;//所受外力方向
        kmVec3 m_BirthPosition;//发射位置
        kmVec3 m_BirthDirection;//发射方向

        SParticle( )
        {
            kmVec3Fill( &m_BirthPosition, 0.0, 0.0, 0.0 );
            kmVec3Fill( &m_BirthDirection, 0.0, 1.0, 0.0 );
            m_Velocity = 1;
            m_VelocityDecay = 0;
            m_ForcePower = 0;
            kmVec3Fill( &m_ForceDirection, 0.0, -1.0, 0.0 );
            m_LiveTime = 1000;
        }
        ~SParticle( )
        {

        }
    };

    class ParticleEntity
    {
        friend class ParticleEmitter;
    private:
        ParticleEntity( );

        /*
        * Method:    update
        
        * Access:    public 
        * Returns:   void
        * Param:     float dtt当前帧渲染时间
        * Remark:    根据时间点更新当前粒子的位置、朝向等信息
        //*/
        void Update( float dtt );

        /*
        * Method:    computeVertices
        
        * Access:    public 
        * Returns:   void
        * Param:     CVertexPTC * vertices Emitter的顶点信息初始指针
        * Param:     OUT unsigned int& idx 当前已经存的点的个数,返回给下一个顶点
        * Param:     unsigned short* indexBuffer 索引数组初始指针
        * Remark:    每一个粒子在emitter的顶点buffer中占有一段空间，
        该函数就是指定更新自己所属的粒子空间段
        //*/
        void ComputeVertices(CVertexPTC** vertices, unsigned short** indexBuffer, OUT unsigned int& idx );

        void SetBirthPosition( const kmVec3& birthPosition );

        void SetBirthDirection( const kmVec3& direction );

        void SetVelocity( float velocity );

        void SetVelocityDecay( float velocityDecay );

        void SetForcePower( float power );

        void SetForceDirection( const kmVec3& direction );

        void SetLiveTime( float time );


        /*
        * Method:    isLive
        
        * Access:    private 
        * Returns:   bool true = 存活， false = 死亡
        * Remark:	 返回当前粒子的存活状态
        //*/
        bool IsLive( );

        /*
        * Method:    activate
        
        * Access:    private 
        * Returns:   void
        * Remark:    重新激活死亡粒子
        //*/
        void Activate( );

        void SetProperty( ParticleProperty* pProperty );

    public:
        ~ParticleEntity( );

    private:
        bool m_IsAlive;//存活状态
        kmVec3 m_CurrentPosition;//当前的粒子位置
        float m_CurrentLiveTime;//当前已经存活时间
        SParticle* m_pParticle;
        float m_CurrentVelocity;//当前粒子运动速度
        kmMat4 m_RotateMat4;//旋转矩阵
        ParticleProperty* m_pParticleProperty;
    };
};

#endif // Particle_H_INCLUDE
