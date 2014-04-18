
#include "stdafx.h"
#include "ShaderUniform.h"
#include "Renderer.h"

CShaderUniform::CShaderUniform()
{

}

CShaderUniform::CShaderUniform( CSerializer& serializer )
{
    DECLARE_PROPERTY(serializer, m_strName, true, 0xFFFFFFFF, _T("变量名"), NULL, NULL, NULL);
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EShaderUniformType, true, 0xFFFFFFFF, _T("变量类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_data, true, 0xFFFFFFFF, _T("变量数据"), NULL, NULL, NULL);
}

CShaderUniform::~CShaderUniform()
{
    m_data.clear();
}

void CShaderUniform::SetValue( const TString& name, EShaderUniformType type, const TUniformData& data )
{
    m_strName = name;
    m_type = type;
    TUniformData::const_iterator iter = data.begin();
    for ( ; iter != data.end(); ++iter )
    {
        m_data.push_back( *iter );
    }
}

void CShaderUniform::SendUniform()
{
    BEATS_ASSERT( CheckType() );
    GLuint currProgram = CRenderer::GetInstance()->GetCurrentState()->GetCurrentShaderProgram();
    char name[ MAX_PATH ];
    CStringHelper::GetInstance()->ConvertToCHAR( m_strName.c_str(), name, MAX_PATH );
    GLint nameLocation = CRenderer::GetInstance()->GetUniformLocation( currProgram, name );
    if ( -1 != nameLocation )
    {
        switch ( m_type)
        {
        case eSUT_1i:
            CRenderer::GetInstance()->SetUniform1iv(nameLocation, (GLint*)m_data.data(), m_data.size());
            break;
        case eSUT_2i:
            CRenderer::GetInstance()->SetUniform2iv(nameLocation, (GLint*)m_data.data(), m_data.size());
            break;
        case eSUT_3i:
            CRenderer::GetInstance()->SetUniform3iv(nameLocation, (GLint*)m_data.data(), m_data.size());
            break;
        case eSUT_4i:
            CRenderer::GetInstance()->SetUniform4iv(nameLocation, (GLint*)m_data.data(), m_data.size());
            break;
        case eSUT_1f:
            CRenderer::GetInstance()->SetUniform1fv(nameLocation, (GLfloat*)m_data.data(), m_data.size());
            break;
        case eSUT_2f:
            CRenderer::GetInstance()->SetUniform2fv(nameLocation, (GLfloat*)m_data.data(), m_data.size());
            break;
        case eSUT_3f:
            CRenderer::GetInstance()->SetUniform3fv(nameLocation, (GLfloat*)m_data.data(), m_data.size());
            break;
        case eSUT_4f:
            CRenderer::GetInstance()->SetUniform4fv(nameLocation, (GLfloat*)m_data.data(), m_data.size());
            break;
        case eSUT_Matrix2f:
            CRenderer::GetInstance()->SetUniformMatrix2fv(nameLocation, (GLfloat*)m_data.data(), 1 );
            break;
        case eSUT_Matrix3f:
            CRenderer::GetInstance()->SetUniformMatrix3fv(nameLocation, (GLfloat*)m_data.data(), 1);
            break;
        case eSUT_Matrix4f:
            CRenderer::GetInstance()->SetUniformMatrix4fv(nameLocation, (GLfloat*)m_data.data(), 1);
            break;
        default:
            BEATS_ASSERT(false, _T("Unknown Type of SShaderUniform"));
            break;
        }
    }
}

bool CShaderUniform::CheckType()
{
    size_t size = 0;
    switch ( m_type )
    {
    case eSUT_1i:
    case eSUT_1f:
        size = 1;
        break;
    case eSUT_2i:
    case eSUT_2f:
        size = 2;
        break;
    case eSUT_3i:
    case eSUT_3f:
        size = 3;
        break;
    case eSUT_4i:
    case eSUT_4f:
    case eSUT_Matrix2f:
        size = 4;
        break;
    case eSUT_Matrix3f:
        size = 9;
        break;
    case eSUT_Matrix4f:
        size = 16;
        break;
    default:
        break;
    }

    return (size == m_data.size());
}

bool CShaderUniform::operator==( const CShaderUniform& other ) const
{
    return ( m_strName == other.m_strName && m_type == other.m_type && ComperaPtrVector( m_data, other.m_data ));
}
