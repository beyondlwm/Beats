/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   11:40
*    filename:     PARTICLE.CPP
*    file base:    Particle
*    file ext:    cpp
*    author:        GJ
*    
*    history:    
**/
#include "stdafx.h"
#include "Particle.h"

#define PARTICLE_MASS 10

namespace FCEngine
{
    ParticleEntity::ParticleEntity()
    {
        m_pParticle = new SParticle( );
        Activate( );
    }

    ParticleEntity::~ParticleEntity()
    {
        BEATS_SAFE_DELETE( m_pParticle );
    }

    void ParticleEntity::Update( float dtt )
    {
        m_CurrentLiveTime += dtt;
        float updateTime = dtt;//需要更新粒子的时间
        if ( m_CurrentLiveTime > m_pParticle->m_LiveTime )
        {
            updateTime = dtt - ( m_CurrentLiveTime - m_pParticle->m_LiveTime );
            m_IsAlive = false;//标志粒子死亡
        }
        //计算运动方向
        //计算位移
        kmVec3 pMove;
        m_CurrentVelocity -= m_pParticle->m_VelocityDecay * dtt;
        if ( m_CurrentVelocity <= 0 )
        {
            m_CurrentVelocity = 0;
        }
        float distance = m_CurrentVelocity * dtt;
        //计算位置
        kmVec3Scale( &pMove, &m_pParticle->m_BirthDirection, distance ); 
        kmVec3Add( &m_CurrentPosition, &m_CurrentPosition, &pMove );

        //考虑外力情况 s = v0 * t + 0.5 * a * t * t, a = f / m(m为粒子质量)
        if ( m_pParticle->m_ForcePower != 0 )
        {
            float length = 0.5f * m_pParticle->m_ForcePower / PARTICLE_MASS * m_CurrentLiveTime * m_CurrentLiveTime;
            kmVec3 direction;
            kmVec3Scale( &direction, &m_pParticle->m_ForceDirection, length );
            kmVec3Add( &m_CurrentPosition, &m_CurrentPosition, &direction );
        }

        //update the animation
        for ( auto iter : m_pParticleProperty->GetAnimations() )
        {
            iter->Update( dtt );
        }
    }

    void ParticleEntity::ComputeVertices( CVertexPTC** vertices, unsigned short** indexBuffer, OUT unsigned int& idx )
    {
        BEATS_ASSERT( NULL == m_pParticleProperty, _T( "The particle property not be set" ));
        //在发射器局部坐标系计算当前位置
        CVertexPTC* pVertices = *vertices;
        unsigned short* pIndexBuffer = *indexBuffer;
        const TParticleAnimation& aniamtions = m_pParticleProperty->GetAnimations( );
        kmVec3 leftTop, leftDown, rightTop, rightDown;
        kmVec3Fill( &leftTop, m_pParticleProperty->GetWidth() * -0.5f, m_pParticleProperty->GetHeight() * 0.5f, 0 );
        kmVec3Fill( &leftDown, m_pParticleProperty->GetWidth() * -0.5f, m_pParticleProperty->GetHeight() * -0.5f, 0 );
        kmVec3Fill( &rightTop, m_pParticleProperty->GetWidth() * 0.5f , m_pParticleProperty->GetHeight() * -0.5f, 0 );
        kmVec3Fill( &rightDown, m_pParticleProperty->GetWidth() * 0.5f, m_pParticleProperty->GetHeight() * 0.5f, 0 );
        for ( auto iter : aniamtions )
        {
            if ( eAT_SCALE == iter->GetAnimationType( ) )//scale
            {
                const float* scale = (const float*)iter->GetCurrentAnimationValue();
                kmVec3Scale( &leftTop, &leftTop, *scale );
                kmVec3Scale( &leftDown, &leftDown, *scale );
                kmVec3Scale( &rightTop, &rightTop, *scale );
                kmVec3Scale( &rightDown, &rightDown, *scale );
            }
            else if( eAT_ROTATE == iter->GetAnimationType( ) )//rotate
            {
                const float* rotate = (const float*)iter->GetCurrentAnimationValue();
                kmMat4RotationPitchYawRoll( &m_RotateMat4, 0, 0, *rotate );
                kmVec3Transform( &leftTop, &leftTop, &m_RotateMat4 );
                kmVec3Transform( &leftDown, &leftDown, &m_RotateMat4 );
                kmVec3Transform( &rightTop, &rightTop, &m_RotateMat4 );
                kmVec3Transform( &rightDown, &rightDown, &m_RotateMat4 );
            }
            else if( eAT_COLOR == iter->GetAnimationType( ) )
            {
                //颜色
                const CColor* color = (const CColor*)iter->GetCurrentAnimationValue();
                pVertices[ idx ].color = *color;
                pVertices[ idx + 1 ].color = *color;
                pVertices[ idx + 2 ].color = *color;
                pVertices[ idx + 3 ].color = *color;
            }
        }

        //平移
        kmVec3Add( &leftTop, &leftTop, &m_CurrentPosition );
        kmVec3Add( &leftDown, &leftDown, &m_CurrentPosition );
        kmVec3Add( &rightTop, &rightTop, &m_CurrentPosition );
        kmVec3Add( &rightDown, &rightDown, &m_CurrentPosition );

        //存顶点
        //位置
        kmVec3Fill( &pVertices[ idx ].position, leftTop.x, leftTop.y, leftTop.z );
        kmVec3Fill( &pVertices[ idx + 1 ].position, leftDown.x, leftDown.y, leftDown.z );
        kmVec3Fill( &pVertices[ idx + 2 ].position, rightDown.x, rightDown.y, rightDown.z );
        kmVec3Fill( &pVertices[ idx + 3 ].position, rightTop.x, rightTop.y, rightTop.z );
        //纹理
        pVertices[ idx ].tex.u = 0;
        pVertices[ idx ].tex.v = 0;
        pVertices[ idx + 1 ].tex.u = 0;
        pVertices[ idx + 1 ].tex.v = 1;
        pVertices[ idx + 2 ].tex.u = 1;
        pVertices[ idx + 2 ].tex.v = 1;
        pVertices[ idx + 3 ].tex.u = 1;
        pVertices[ idx + 3 ].tex.v = 0;
        
        //索引
        unsigned short indexIdx = ( unsigned short )idx / 4 * 6;
        pIndexBuffer[ indexIdx ] = ( unsigned short )idx;
        pIndexBuffer[ indexIdx + 1 ] = ( unsigned short )idx + 1;
        pIndexBuffer[ indexIdx + 2 ] = ( unsigned short )idx + 2;
        pIndexBuffer[ indexIdx + 3 ] = ( unsigned short )idx;
        pIndexBuffer[ indexIdx + 4 ] = ( unsigned short )idx + 2;
        pIndexBuffer[ indexIdx + 5 ] = ( unsigned short )idx + 3;

        idx += 4;
    }

    void ParticleEntity::SetBirthPosition( const kmVec3& birthPosition )
    {
        kmVec3Fill( &m_pParticle->m_BirthPosition, birthPosition.x, birthPosition.y, birthPosition.z );
        kmVec3Fill( &m_CurrentPosition, birthPosition.x, birthPosition.y, birthPosition.z );
    }

    void ParticleEntity::SetBirthDirection( const kmVec3& direction )
    {
        kmVec3Fill( &m_pParticle->m_BirthDirection, direction.x, direction.y, direction.z );
        if ( kmVec3Length( &m_pParticle->m_BirthDirection ) > 0 )
        {
            kmVec3Normalize( &m_pParticle->m_BirthDirection , &m_pParticle->m_BirthDirection );
        }
    }

    void ParticleEntity::SetVelocity( float velocity )
    {
        m_pParticle->m_Velocity = velocity;
        m_CurrentVelocity = velocity;
    }

    void ParticleEntity::SetVelocityDecay( float velocityDecay )
    {
        m_pParticle->m_VelocityDecay = velocityDecay;
    }

    void ParticleEntity::SetForcePower( float power )
    {
        m_pParticle->m_ForcePower = power;
    }

    void ParticleEntity::SetForceDirection( const kmVec3& direction )
    {
        kmVec3Fill( &m_pParticle->m_ForceDirection, direction.x, direction.y, direction.z );
        kmVec3Normalize( &m_pParticle->m_ForceDirection , &m_pParticle->m_ForceDirection );
    }

    void ParticleEntity::SetLiveTime( float time )
    {
        m_pParticle->m_LiveTime = time;
        BEATS_ASSERT( NULL == m_pParticleProperty, _T( "The particle property not be set" ) )
        for ( auto iter : m_pParticleProperty->GetAnimations() )
        {
            iter->SetTotalTime( time );
        }
    }

    bool ParticleEntity::IsLive()
    {
        return m_IsAlive;
    }

    void ParticleEntity::Activate()
    {
        m_IsAlive = true;
        kmVec3Fill( &m_CurrentPosition, 0.0, 0.0, 0.0 );
        m_CurrentLiveTime = 0;
        kmMat4Identity( &m_RotateMat4 );
        
        for ( auto iter : m_pParticleProperty->GetAnimations() )
        {
            iter->Reset( );
        }
        
        m_pParticleProperty = NULL;
    }

    void ParticleEntity::SetProperty( ParticleProperty* pProperty )
    {
        m_pParticleProperty = pProperty;
    }


};
