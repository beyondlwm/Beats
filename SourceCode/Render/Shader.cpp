#include "stdafx.h"
#include "Shader.h"
#include "Renderer.h"
#include "Resource/ResourcePathManager.h"

CSerializer CShader::m_vsPublicHeader;

CShader::CShader()
    : m_shaderType(0)
{

}

CShader::~CShader()
{
    CRenderer::GetInstance()->DeleteShader( m_uId );
}

GLenum CShader::GetShaderType()const
{
    return m_shaderType;
}

void CShader::SetShaderType(GLenum shaderType)
{
    m_shaderType = shaderType;
}

bool CShader::Load()
{
    const TCHAR* pszExtension = PathFindExtension(m_strPath.c_str());
    bool bIsVS = _tcsicmp(pszExtension, _T(".vs")) == 0;
    m_shaderType = bIsVS ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    bool bRet = Compile(&m_uId, m_strPath.c_str());

    FC_CHECK_GL_ERROR_DEBUG();

    return bRet;
}

bool CShader::Unload()
{
    return true;
}

GLuint CShader::ID() const
{
     return m_uId;
}

bool CShader::Compile( GLuint *shader, const TCHAR* pszShaderFilePath )
{
    bool bRet = false;
    CSerializer fileSerializer(pszShaderFilePath);
    if (fileSerializer.GetReadPos() < fileSerializer.GetWritePos())
    {
        bRet = Compile(shader, (const GLchar*)fileSerializer.GetBuffer(), fileSerializer.GetWritePos());
        BEATS_ASSERT(bRet, _T("Failed to compile vertex shader"));
    }
    return bRet;
}

bool CShader::Compile(GLuint * shader, const GLchar* pszSource, GLint length)
{
    GLint status = GL_FALSE;

    if(m_vsPublicHeader.GetWritePos() == 0)
    {
#ifdef USE_UBO
        char def[] = "#define USE_UBO\r\n";
        m_vsPublicHeader.Serialize((const void *)def, sizeof(def) - 1);
#endif
        TString strPath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Shader);
        strPath.append(_T("/ShaderHead.vs"));
        m_vsPublicHeader.Serialize(strPath.c_str());
    }

    if (pszSource != NULL)
    {
        CRenderer* pRenderer = CRenderer::GetInstance();
        *shader = pRenderer->CreateShader(m_shaderType);
        BEATS_ASSERT( m_shaderType == GL_VERTEX_SHADER || m_shaderType == GL_FRAGMENT_SHADER);
        if(m_shaderType == GL_VERTEX_SHADER)
        {
            const GLchar *sources[2] = {0};
            GLint lengths[2] = {0};

            sources[0] = reinterpret_cast<const GLchar *>(m_vsPublicHeader.GetBuffer());
            sources[1] = pszSource;
            lengths[0] = m_vsPublicHeader.GetWritePos();
            lengths[1] = length;
            CRenderer::GetInstance()->ShaderSource(*shader, 2, sources, lengths );
        }
        else
        {
            CRenderer::GetInstance()->ShaderSource(*shader, 1, &pszSource, &length);
        }
        CRenderer::GetInstance()->CompileShader(*shader);
        CRenderer::GetInstance()->GetShaderiv(*shader, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            GLsizei length = 0;
            CRenderer::GetInstance()->GetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
            GLchar* src = (GLchar *)new char[sizeof(GLchar) * length];

            CRenderer::GetInstance()->GetShaderInfoLog(*shader, length, nullptr, src);
            TCHAR *buf = new TCHAR[length];
            CStringHelper::GetInstance()->ConvertToTCHAR(src, buf, length);
            BEATS_ASSERT(false, _T("cocos2d: ERROR: Failed to compile shader:\n%s"), buf);

            BEATS_SAFE_DELETE_ARRAY(buf);
            BEATS_SAFE_DELETE(src);
        }
    }
    return (status == GL_TRUE);
}