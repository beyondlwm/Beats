#include "stdafx.h"
#include "Material.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Renderer.h"
#include "Resource/ResourceManager.h"
#include "Shader.h"
#include "ShaderProgram.h"

CMaterial::CMaterial()
    : m_pRenderState(NULL)
{
    m_AmbientColor = CColor( 0.2f, 0.2f, 0.2f, 1.0f );
    m_DiffuseColor = CColor( 0.8f, 0.8f, 0.8f, 1.0f );
    m_SpecularColor = CColor( 0.0f, 0.0f, 0.0f, 1.0f );
    m_Shininess = 0;
    m_pRenderState = new CRenderState();
}

CMaterial::CMaterial(CSerializer& serializer)
    : super(serializer)
    , m_pRenderState(NULL)
{
    m_AmbientColor = CColor( 0.2f, 0.2f, 0.2f, 1.0f );
    m_DiffuseColor = CColor( 0.8f, 0.8f, 0.8f, 1.0f );
    m_SpecularColor = CColor( 0.0f, 0.0f, 0.0f, 1.0f );
    m_Shininess = 0;
    DECLARE_PROPERTY(serializer, m_AmbientColor, true, 0xFFFFFFFF, _T("环境反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_DiffuseColor, true, 0xFFFFFFFF, _T("漫反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_SpecularColor, true, 0xFFFFFFFF, _T("镜面反射"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_Shininess, true, 0xFFFFFFFF, _T("耀光度"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pRenderState, true, 0xFFFFFFFF, _T("渲染状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uniforms, true, 0xFFFFFFFF, _T("shader自定义参数"), NULL, NULL, NULL);
}

CMaterial::~CMaterial()
{
    BEATS_SAFE_DELETE(m_pRenderState);
}

bool CMaterial::Load()
{
    return true;
}

bool CMaterial::Unload()
{
    return false;
}

void CMaterial::Use()
{
    m_pRenderState->Restore();
    for ( size_t i = 0; i < m_textures.size(); ++i )
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        pRenderer->ActiveTexture( GL_TEXTURE0 + i );
        pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        pRenderer->TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        pRenderer->BindTexture( GL_TEXTURE_2D, m_textures[ i ]->ID() );
        GLuint currProgram = pRenderer->GetCurrentState()->GetCurrentShaderProgram();
        if ( 0 != currProgram )
        {
            GLint textureLocation = pRenderer->GetUniformLocation(currProgram, COMMON_UNIFORM_NAMES[UNIFORM_TEX0 + i]);
            pRenderer->SetUniform1i( textureLocation, i);
        }
        FC_CHECK_GL_ERROR_DEBUG();
    }
    SenBeamInfo();
    SendUniform();
}

void CMaterial::SendUniform()
{
    for ( auto iter : m_uniforms )
    {
        iter->SendUniform( );
    }
}

void CMaterial::SenBeamInfo()
{
    float ambientColor[4] = {(float)m_AmbientColor.r / 0xFF, (float)m_AmbientColor.g / 0xFF, (float)m_AmbientColor.b / 0xFF, (float)m_AmbientColor.a / 0xFF};
    float diffuseColor[4] = {(float)m_DiffuseColor.r / 0xFF, (float)m_DiffuseColor.g / 0xFF, (float)m_DiffuseColor.b / 0xFF, (float)m_DiffuseColor.a / 0xFF};
    float specularColor[4] = {(float)m_SpecularColor.r / 0xFF, (float)m_SpecularColor.g / 0xFF, (float)m_SpecularColor.b / 0xFF, (float)m_SpecularColor.a / 0xFF};

    GLuint currProgram = m_pRenderState->GetCurrentShaderProgram();
    if ( currProgram != 0 )
    {
        GLint ambientLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_AMBIENT_COLOR]);
        if ( -1 != ambientLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( ambientLocation, ambientColor, 4 );
        }
        GLint diffuseLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_DIFFUSE_COLOR]);
        if ( -1 != diffuseLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( diffuseLocation, diffuseColor, 4 );
        }
        GLint specularLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_SPECULAR_COLOR]);
        if ( -1 != specularLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( specularLocation, specularColor, 4 );
        }
        GLint shininessLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, COMMON_UNIFORM_NAMES[UNIFORM_SHININESS]);
        if ( -1 != shininessLocation )
        {
            CRenderer::GetInstance()->SetUniform1fv( shininessLocation, &m_Shininess, 1 );
        }
    }
}

CRenderState* CMaterial::GetRenderState() const
{
    return m_pRenderState;
}

void CMaterial::AddTextrue( const TString& fileName )
{
    SharePtr< CTexture > pTextrue( 
        CResourceManager::GetInstance()->GetResource< CTexture >( fileName, false));
    m_textures.push_back( pTextrue );
}

void CMaterial::SetAmbientColor( const CColor& color )
{
    m_AmbientColor = color;
}

void CMaterial::SetDiffuseColor( const CColor& color )
{
    m_DiffuseColor = color;
}

void CMaterial::SetSpecularColor( const CColor& color )
{
    m_SpecularColor = color;
}

void CMaterial::SetShininess( float shininess )
{
    m_Shininess = shininess;
}

bool CMaterial::operator==( const CMaterial& other ) const
{
    bool bEqual = false;
    if ( ComperaVector( m_textures, other.m_textures ) && ComperaPtrVector( m_uniforms, other.m_uniforms ) )
    {
        if ( m_AmbientColor == other.m_AmbientColor &&
            m_DiffuseColor == other.m_DiffuseColor &&
            m_SpecularColor == other.m_SpecularColor &&
            m_Shininess == other.m_Shininess )
        {
            if ( *m_pRenderState == *other.m_pRenderState )
            {
                bEqual = true;
            }
        }
    }
    return bEqual;
}

void CMaterial::AddUniform( CShaderUniform* uniform )
{
    m_uniforms.push_back( uniform );
}

bool CMaterial::ComperaVector( const std::vector< SharePtr<CTexture>>& v1, const std::vector< SharePtr<CTexture>>& v2 ) const
{
    bool bReturn = true;
    if ( v1.size() == v2.size() )
    {
        size_t size = v1.size();
        for ( size_t i = 0; i < size; ++i )
        {
            if ( v1[ i ] != v2[ i ])
            {
                bReturn = false;
                break;
            }
        }
    }
    else
    {
        bReturn = false;
    }
    return bReturn;
}
