
/**
*    Copyright (C) 2014, All right reserved
*    created:    2014/03/24
*    created:    24:3:2014   11:40
*    filename:     EMITTER.CPP
*    file base:    Em_itter
*    file ext:    cpp
*    author:        GJ
*    
*    history:    
**/
#include "stdafx.h"
#include "Emitter.h"
#include "Render/Renderer.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Render/Shader.h"


namespace FCEngine
{
    ParticleEmitter::ParticleEmitter()
    {
        m_pEmitter = new SEmitter( );
        m_VertData = 0;
        m_CurrentCount = 0;
        m_BeginTime = -1;
        m_MinPreShootTime = -1;
        m_DeltaTime = 0;
        m_IndexBuffer = 0;
        m_bInit = false;
        m_pParticleProperty = NULL;

        SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("particle.vs"), false);
        SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("particle.ps"), false);
        m_pParticleProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());

        m_pMaterial = NULL;
        m_pEmitterBase = NULL;
    }

    ParticleEmitter::ParticleEmitter( CSerializer& serializer ) :
        CComponentBase( serializer )
    {
        m_pEmitter = new SEmitter( );
        m_VertData = 0;
        m_CurrentCount = 0;
        m_BeginTime = -1;
        m_MinPreShootTime = -1;
        m_DeltaTime = 0;
        m_IndexBuffer = 0;
        m_bInit = false;
        m_pParticleProperty = NULL;

        //SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("particle.vs"), false);
        //SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("particle.ps"), false);
        //m_pParticleProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());

        m_pMaterial = NULL;
        m_pEmitterBase = NULL;

        DECLARE_PROPERTY( serializer, m_pEmitterBase, true, 0xFFFFFFFF, _T( "发射器类型" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_TotalCount, true, 0xFFFFFFFF, _T( "发射粒子总数" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_pMaterial, true, 0xFFFFFFFF, _T( "材质" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ShootOrigin, true, 0xFFFFFFFF, _T( "发射器原点坐标" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ShootDeriction, true, 0xFFFFFFFF, _T( "发射方向" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ShootDegree, true, 0xFFFFFFFF, _T( "发射角度" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ShootVelocity, true, 0xFFFFFFFF, _T( "每秒发射粒子个数" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MinParticleVelocity, true, 0xFFFFFFFF, _T( "最小运动速度" ), _T( "粒子运动速度" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MaxParticleVelocity, true, 0xFFFFFFFF, _T( "最大运动速度" ), _T( "粒子运动速度" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MinVelocityDecay, true, 0xFFFFFFFF, _T( "最小衰减速度" ), _T( "粒子运动速度衰减" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MaxVelocityDecay, true, 0xFFFFFFFF, _T( "最大衰减速度" ), _T( "粒子运动速度衰减" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ForcePower, true, 0xFFFFFFFF, _T( "外力大小" ), _T( "外力" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_ForceDeriction, true, 0xFFFFFFFF, _T( "外力方向" ), _T( "外力" ), NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MinLiveTime, true, 0xFFFFFFFF, _T( "最小生存时间" ), _T( "粒子运动生存周期" ), _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_MaxLiveTime, true, 0xFFFFFFFF, _T( "最小生存时间" ), _T( "粒子运动生存周期" ), _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_DelayTime, true, 0xFFFFFFFF, _T( "延迟发射时间" ), NULL, _T( "单位(s)" ), NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_Constraint, true, 0xFFFFFFFF, _T( "是否约束粒子" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pEmitter->m_IsLoop, true, 0xFFFFFFFF, _T( "是否循环播放" ), NULL, NULL, NULL );
        DECLARE_PROPERTY( serializer, m_pParticleProperty, true, 0xFFFFFFFF, _T( "粒子属性" ), NULL, NULL, NULL );
    }

    ParticleEmitter::~ParticleEmitter()
    {
        for ( auto iter : m_AliveParticleList )
        {
           BEATS_SAFE_DELETE( iter );
        }

        for ( auto iter : m_ParticleRecycle )
        {
           BEATS_SAFE_DELETE( iter );
        }

        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->DeleteBuffers(2, m_VBO);
        pRenderer->DeleteBuffers(1, &m_VAO);

        BEATS_SAFE_DELETE( m_pEmitter );
        BEATS_SAFE_DELETE_ARRAY( m_VertData );
        BEATS_SAFE_DELETE_ARRAY( m_IndexBuffer );

        BEATS_SAFE_DELETE( m_pEmitterBase );
        BEATS_SAFE_DELETE( m_pParticleProperty );
    }

    void ParticleEmitter::BeginShoot()
    {
        m_BeginTime = 0;
    }

    void ParticleEmitter::Update( float dtt )
    {
        UpdateActiveParticles( dtt );

        if ( m_pEmitter->m_DelayTime > 0 && m_BeginTime < m_pEmitter->m_DelayTime )//延迟发射
        {
            m_BeginTime += dtt;
            if ( m_BeginTime < m_pEmitter->m_DelayTime )
            {
                return;
            }
        }
        if ( m_CurrentCount < m_pEmitter->m_TotalCount || m_pEmitter->m_IsLoop )
        {
            //计算当前帧应该发射多少粒子
            //当前时间是否满足发射一颗粒子的最小时间
            if ( m_DeltaTime < m_MinPreShootTime )
            {
                m_DeltaTime += dtt;
            }
            float pCount = m_DeltaTime * m_pEmitter->m_ShootVelocity + 0.5f;
            ShootParicle( (int)pCount );
            if ( pCount > 1.0 )//已经至少发射一个粒子，重新计时
            {
                m_DeltaTime = 0;
            }
        }
    }

    void ParticleEmitter::Render()
    {
        unsigned int index = 0;
        TParticleList::iterator it = m_AliveParticleList.begin( );
        //clear buffer befor per frame
        memset( m_VertData, 0, sizeof( CVertexPTC ) * m_pEmitter->m_TotalCount * 4 );
        memset( m_IndexBuffer, 0, sizeof( unsigned short ) * m_pEmitter->m_TotalCount * 6 );
        for ( ; it != m_AliveParticleList.end(); )
        {
            ParticleEntity* pParticle = *it;
            if ( index < m_pEmitter->m_TotalCount * 4 )
            {
                pParticle->ComputeVertices( &m_VertData ,&m_IndexBuffer, index );
            }
            else
            {
                BEATS_ASSERT( "index > m_pEmitter->m_TotalCount * 4 !" );
            }
            if ( !pParticle->IsLive() )
            {
                m_ParticleRecycle.push_back( *it );
                it = m_AliveParticleList.erase( it );
            }
            else
            {
                ++it;
            }
        }
       
       //render
        if ( m_VertData && m_IndexBuffer && !m_AliveParticleList.empty() )
        {
            CRenderer* pRenderer = CRenderer::GetInstance();

            pRenderer->BindBuffer( GL_ARRAY_BUFFER, m_VBO[ 0 ] ); 
            pRenderer->BufferData( GL_ARRAY_BUFFER, sizeof( CVertexPTC ) * m_CurrentCount * 4 , m_VertData, GL_DYNAMIC_DRAW ); 
            FC_CHECK_GL_ERROR_DEBUG( );

            pRenderer->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_VBO[ 1 ] ); 
            pRenderer->BufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( unsigned short ) * m_CurrentCount * 6 , m_IndexBuffer , GL_DYNAMIC_DRAW );
            FC_CHECK_GL_ERROR_DEBUG( );

            if( m_pMaterial )
            {
                 m_pMaterial->Use();
            }
  
            pRenderer->UseProgram( m_pParticleProgram->ID() );
            kmMat4 viewMatrix;
            kmGLGetMatrix(KM_GL_MODELVIEW, &viewMatrix); 
            kmMat4 translateMatrix;
            kmMat4Translation( &translateMatrix, -viewMatrix.mat[ 12 ], -viewMatrix.mat[ 13 ], -viewMatrix.mat[ 14 ] );
            kmMat4Multiply( &viewMatrix, &translateMatrix, &viewMatrix );
            kmMat4Inverse( &viewMatrix, &viewMatrix );
            GLint MVPLocation = CRenderer::GetInstance()->GetUniformLocation( m_pParticleProgram->ID(), COMMON_UNIFORM_NAMES[ UNIFORM_MV_MATRIX ]);
            CRenderer::GetInstance()->SetUniformMatrix4fv(MVPLocation, (const float*)viewMatrix.mat, 1);
            FC_CHECK_GL_ERROR_DEBUG();

            pRenderer->BindVertexArray( m_VAO );
            FC_CHECK_GL_ERROR_DEBUG( );
            pRenderer->DrawElements( GL_TRIANGLES, m_CurrentCount * 6 , GL_UNSIGNED_SHORT, 0 );
            FC_CHECK_GL_ERROR_DEBUG( );
            pRenderer->BindVertexArray( 0 );
            pRenderer->UseProgram( 0 );
        }
    }

    void ParticleEmitter::ShootParicle( unsigned int count )
    {
        BEATS_ASSERT( NULL == m_pEmitterBase , _T( "The emitter type can't be sure!" ) );
        m_CurrentCount += count;
        unsigned int pCreateCount = count;
        unsigned int pReUseCount = 0;
        if ( m_CurrentCount >= m_pEmitter->m_TotalCount )//是否已经达到最大粒子数
        {
            pReUseCount = m_CurrentCount - m_pEmitter->m_TotalCount;
            pCreateCount -= pReUseCount;
            m_CurrentCount = m_pEmitter->m_TotalCount;
            if ( !m_pEmitter->m_IsLoop )
            {
                pReUseCount = 0;
            }
            else
            {
                m_BeginTime = 0;
            }
        }

        //创建新粒子
        for ( unsigned int i = 0; i < pCreateCount; ++i )
        {
           ParticleEntity* pParticle = new ParticleEntity( );
           kmVec3 birthPosition;
           kmVec3Fill( &birthPosition, m_pEmitterBase->GetBirthPositon( ).x,
               m_pEmitterBase->GetBirthPositon( ).y, m_pEmitterBase->GetBirthPositon( ).z );
           kmVec3Add( &birthPosition, &m_pEmitter->m_ShootOrigin, &birthPosition );
           pParticle->SetBirthPosition( birthPosition );
           SetParticleInitProperty( &pParticle );
           m_AliveParticleList.push_back( pParticle );
        }

        //从回收站中拿粒子
        for ( unsigned int i = 0; i < pReUseCount; ++i )
        {
            ParticleEntity* pParticle = GetParticleFromRecycle( );
            if ( pParticle )
            { 
                pParticle->Activate();//重新激活粒子
                kmVec3 birthPosition;
                kmVec3Fill( &birthPosition, m_pEmitterBase->GetBirthPositon( ).x,
                    m_pEmitterBase->GetBirthPositon( ).y, m_pEmitterBase->GetBirthPositon( ).z );
                kmVec3Add( &birthPosition, &m_pEmitter->m_ShootOrigin, &birthPosition );
                pParticle->SetBirthPosition( birthPosition );
                SetParticleInitProperty( &pParticle );
                m_AliveParticleList.push_back( pParticle );
            }
        }
    }


    void ParticleEmitter::SetTotalCount( unsigned int count )
    {
        m_pEmitter->m_TotalCount = count;
        InitVAOAndVBO( count );
    }

    unsigned int ParticleEmitter::GetTotleCount()
    {
        return m_pEmitter->m_TotalCount;
    }


    void ParticleEmitter::SetShootDeriction( const kmVec3& deriction )
    {
        kmVec3Fill( &m_pEmitter->m_ShootDeriction, deriction.x, deriction.y, deriction.z );
    }

    const kmVec3& ParticleEmitter::GetShootDeriction() const
    {
        return m_pEmitter->m_ShootDeriction;
    }

    void ParticleEmitter::SetShootOrigin( const kmVec3& origin )
    {
        kmVec3Fill( &m_pEmitter->m_ShootOrigin, origin.x, origin.y, origin.z );
    }

    const kmVec3& ParticleEmitter::GetShootOrigin() const
    {
        return m_pEmitter->m_ShootOrigin;
    }

    void ParticleEmitter::SetShootDegree( const kmVec3& degree )
    {
         kmVec3Fill( &m_pEmitter->m_ShootDegree, degree.x, degree.y, degree.z );
    }

    const kmVec3& ParticleEmitter::GetShootDegree() const
    {
        return m_pEmitter->m_ShootDegree;
    }

    void ParticleEmitter::SetShootVelocity( unsigned int velocity )
    {
        m_pEmitter->m_ShootVelocity = velocity;
        m_MinPreShootTime = 1.0f / velocity;
    }

    unsigned int ParticleEmitter::GetShootVelocity()
    {
        return m_pEmitter->m_ShootVelocity;
    }

    void ParticleEmitter::SetMinParticleVelocity( float velocity )
    {
        m_pEmitter->m_MinParticleVelocity = velocity;
    }

    float ParticleEmitter::GetMinParticleVelocity()
    {
        return m_pEmitter->m_MinParticleVelocity;
    }

    void ParticleEmitter::SetMaxParticleVelocity( float velocity )
    {
        m_pEmitter->m_MaxParticleVelocity = velocity;
    }

    float ParticleEmitter::GetMaxParticleVelocity()
    {
        return m_pEmitter->m_MaxParticleVelocity;
    }

    void ParticleEmitter::SetMinParticleVelocityDecay( float decay )
    {
        m_pEmitter->m_MinVelocityDecay = decay;
    }

    float ParticleEmitter::GetMinParticleVelocityDecay()
    {
        return m_pEmitter->m_MinVelocityDecay;
    }

    void ParticleEmitter::SetMaxParticleVelocityDecay( float decay )
    {
        m_pEmitter->m_MaxVelocityDecay = decay;
    }

    float ParticleEmitter::GetMaxParticleVelocityDecay()
    {
        return m_pEmitter->m_MaxVelocityDecay;
    }

    void ParticleEmitter::SetForcePower( float power )
    {
        m_pEmitter->m_ForcePower = power;
    }

    float ParticleEmitter::GetForcePower()
    {
        return m_pEmitter->m_ForcePower;
    }

    void ParticleEmitter::SetFroceDeriction( const kmVec3& deriction )
    {
        kmVec3Fill( &m_pEmitter->m_ForceDeriction, deriction.x, deriction.y, deriction.z );
    }

    const kmVec3& ParticleEmitter::GetFroceDeriction() const
    {
        return m_pEmitter->m_ForceDeriction;
    }

    void ParticleEmitter::SetMinLiveTime( float time )
    {
        m_pEmitter->m_MinLiveTime = time;
    }

    float ParticleEmitter::GetMinLiveTime()
    {
        return m_pEmitter->m_MinLiveTime;
    }

    void ParticleEmitter::SetMaxLiveTime( float time )
    {
        m_pEmitter->m_MaxLiveTime = time;
    }

    float ParticleEmitter::GetMaxLiveTime()
    {
        return m_pEmitter->m_MaxLiveTime;
    }

    void ParticleEmitter::SetDelayTime( float time )
    {
        m_pEmitter->m_DelayTime = time;
    }

    float ParticleEmitter::GetDelayTime()
    {
        return m_pEmitter->m_DelayTime;
    }

    void ParticleEmitter::SetParticleConstraint( bool bConstraint )
    {
        m_pEmitter->m_Constraint = bConstraint;
    }

    bool ParticleEmitter::GetParticleConstraint()
    {
        return m_pEmitter->m_Constraint;
    }

    void ParticleEmitter::SetEmitterAttribute( const SEmitter& pEmitter )
    {
        SetParticleConstraint( pEmitter.m_Constraint );
        SetDelayTime( pEmitter.m_DelayTime );
        SetFroceDeriction( pEmitter.m_ForceDeriction );
        SetForcePower( pEmitter.m_ForcePower );
        SetPlayLoop( pEmitter.m_IsLoop );
        SetMaxLiveTime( pEmitter.m_MaxLiveTime );
        SetMaxParticleVelocity( pEmitter.m_MaxParticleVelocity );
        SetMaxParticleVelocityDecay( pEmitter.m_MaxVelocityDecay );
        SetMinLiveTime( pEmitter.m_MinLiveTime );
        SetMinParticleVelocity( pEmitter.m_MinParticleVelocity );
        SetMinParticleVelocityDecay( pEmitter.m_MinVelocityDecay );
        SetShootDegree( pEmitter.m_ShootDegree );
        SetShootDeriction( pEmitter.m_ShootDeriction );
        SetShootOrigin( pEmitter.m_ShootOrigin );
        SetShootVelocity( pEmitter.m_ShootVelocity );
        SetTotalCount( pEmitter.m_TotalCount );
    }

    const SEmitter& ParticleEmitter::GetEmitterAttribute() const
    {
        return *m_pEmitter;
    }

    void ParticleEmitter::UpdateActiveParticles( float dtt )
    {
        for ( auto i : m_AliveParticleList )
        {
            ParticleEntity* pParticle = i;
            pParticle->Update( dtt );
        }
    }

    void ParticleEmitter::SetParticleInitProperty( ParticleEntity** pParticleEntity )
    {
        BEATS_ASSERT( NULL == m_pParticleProperty, _T( "The particle property not be set" ));
        ParticleEntity* pParticle = *pParticleEntity;
        pParticle->SetProperty( m_pParticleProperty );
        pParticle->SetForcePower( m_pEmitter->m_ForcePower );
        pParticle->SetForceDirection( m_pEmitter->m_ForceDeriction );

        //方向偏移
        kmVec3 tempDegree;
        kmVec3Fill( &tempDegree, 0.0, 0.0, 0.0 );
        if ( m_pEmitter->m_ShootDegree.x != 0 )
        {
            tempDegree.x = RANGR_RANDOM_FLOAT( 0, m_pEmitter->m_ShootDegree.x );
            tempDegree.x -= m_pEmitter->m_ShootDegree.x * 0.5f;
        }
        if ( m_pEmitter->m_ShootDegree.y != 0 )
        {
            tempDegree.y = RANGR_RANDOM_FLOAT( 0, m_pEmitter->m_ShootDegree.y );
            tempDegree.y -= m_pEmitter->m_ShootDegree.y * 0.5f;
        }
        if ( m_pEmitter->m_ShootDegree.z != 0 )
        {
            tempDegree.z = RANGR_RANDOM_FLOAT( 0, m_pEmitter->m_ShootDegree.z );
            tempDegree.z -= m_pEmitter->m_ShootDegree.z * 0.5f;
        }
        
        kmMat4 rotateMat;
        kmMat4Identity( &rotateMat );
        kmMat4RotationPitchYawRoll( &rotateMat, tempDegree.x / MATH_PI_DEGREE * MATH_PI, tempDegree.y / MATH_PI_DEGREE * MATH_PI, tempDegree.z / MATH_PI_DEGREE * MATH_PI );
        kmVec3 pDeriction;
        kmVec3Transform( &pDeriction, &m_pEmitter->m_ShootDeriction, &rotateMat );

        pParticle->SetBirthDirection( pDeriction );
        float tempNum = RANGR_RANDOM_FLOAT( m_pEmitter->m_MinParticleVelocity, m_pEmitter->m_MaxParticleVelocity );
        pParticle->SetVelocity( tempNum );
        tempNum = RANGR_RANDOM_FLOAT( m_pEmitter->m_MinVelocityDecay, m_pEmitter->m_MaxVelocityDecay );
        pParticle->SetVelocityDecay( tempNum );
        tempNum = RANGR_RANDOM_FLOAT( m_pEmitter->m_MinLiveTime, m_pEmitter->m_MaxLiveTime );
        pParticle->SetLiveTime( tempNum );
    }

    ParticleEntity* ParticleEmitter::GetParticleFromRecycle()
    {
        ParticleEntity* pParticle = 0;
        if ( !m_ParticleRecycle.empty() )
        {
            pParticle =  *m_ParticleRecycle.begin();
            m_ParticleRecycle.pop_front();
        }
        return pParticle;
    }

    void ParticleEmitter::SetPlayLoop( bool bLoop )
    {
        m_pEmitter->m_IsLoop = bLoop;
    }

    bool ParticleEmitter::GetPlayLoop()
    {
        return m_pEmitter->m_IsLoop;
    }

    void ParticleEmitter::InitVAOAndVBO( int count )
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        
        BEATS_SAFE_DELETE_ARRAY( m_VertData );
        BEATS_SAFE_DELETE_ARRAY( m_IndexBuffer );

        m_VertData = new CVertexPTC[ count * 4 ];
        m_IndexBuffer = new unsigned short[ count * 6 ];

        memset( m_VertData, 0, sizeof( CVertexPTC ) * count * 4 );
        memset( m_IndexBuffer, 0, sizeof( unsigned short ) * count * 6 );
        if ( !m_bInit )
        {
            pRenderer->GenVertexArrays( 1, &m_VAO );
            pRenderer->BindVertexArray( m_VAO ); 
            pRenderer->GenBuffers( 2, m_VBO ); 
            pRenderer->BindBuffer( GL_ARRAY_BUFFER, m_VBO[ 0 ] );

            pRenderer->EnableVertexAttribArray( ATTRIB_INDEX_POSITION );
            pRenderer->EnableVertexAttribArray( ATTRIB_INDEX_COLOR );
            pRenderer->EnableVertexAttribArray( ATTRIB_INDEX_TEXCOORD0 );

            pRenderer->VertexAttribPointer( ATTRIB_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(CVertexPTC), (const GLvoid *)offsetof(CVertexPTC, position) ); 
            pRenderer->VertexAttribPointer( ATTRIB_INDEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(CVertexPTC), (const GLvoid *)offsetof(CVertexPTC, color) ); 
            pRenderer->VertexAttribPointer( ATTRIB_INDEX_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, sizeof(CVertexPTC), (const GLvoid *)offsetof(CVertexPTC, tex) ); 
            FC_CHECK_GL_ERROR_DEBUG( );

            pRenderer->BindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_VBO[ 1 ] ); 

            pRenderer->BindBuffer( GL_ARRAY_BUFFER, 0 ); 
            pRenderer->BindVertexArray( 0 ); 
        }
        
        m_bInit = true;
    }

};

