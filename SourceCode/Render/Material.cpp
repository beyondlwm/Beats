#include "stdafx.h"
#include "Material.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Utility/BeatsUtility/Serializer.h"

CMaterial::CMaterial()
    : m_pRenderState(NULL)
{
}

CMaterial::CMaterial(CSerializer& serializer)
    : super(serializer)
    , m_pRenderState(NULL)
{
    DECLARE_PROPERTY(serializer, m_pRenderState, true, 0xFFFFFFFF, _T("äÖÈ¾×´Ì¬"), NULL, NULL, NULL);
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
    for (size_t i = 0; i < m_textures.size(); ++i)
    {
        CRenderManager::GetInstance()->ApplyTexture(i, m_textures[i]->ID());
    }
    SendUniform();
}

void CMaterial::SendUniform()
{
    for (std::map<size_t, SShaderUniform>::iterator iter = m_uniforms.begin(); iter != m_uniforms.end(); ++iter)
    {
        SShaderUniform& uniform = iter->second;
        GLuint uLocation = iter->first;
        switch (uniform.m_type)
        {
        case eSUT_1i:
            glUniform1iv(uLocation, uniform.m_uParamCount, (GLint*)uniform.m_data.GetBuffer());
            break;
        case eSUT_2i:
            glUniform2iv(uLocation, uniform.m_uParamCount, (GLint*)uniform.m_data.GetBuffer());
            break;
        case eSUT_3i:
            glUniform3iv(uLocation, uniform.m_uParamCount, (GLint*)uniform.m_data.GetBuffer());
            break;
        case eSUT_4i:
            glUniform4iv(uLocation, uniform.m_uParamCount, (GLint*)uniform.m_data.GetBuffer());
            break;
        case eSUT_1f:
            glUniform1fv(uLocation, uniform.m_uParamCount, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_2f:
            glUniform2fv(uLocation, uniform.m_uParamCount, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_3f:
            glUniform3fv(uLocation, uniform.m_uParamCount, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_4f:
            glUniform4fv(uLocation, uniform.m_uParamCount, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_Matrix2f:
            glUniformMatrix2fv(uLocation, uniform.m_uParamCount, false, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_Matrix3f:
            glUniformMatrix3fv(uLocation, uniform.m_uParamCount, false, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        case eSUT_Matrix4f:
            glUniformMatrix4fv(uLocation, uniform.m_uParamCount, false, (GLfloat*)uniform.m_data.GetBuffer());
            break;
        default:
            BEATS_ASSERT(false, _T("Unknown Type of SShaderUniform"));
            break;
        }
    }
}
