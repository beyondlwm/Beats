#include "stdafx.h"
#include "ShaderProgram.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "RenderManager.h"
#include "Renderer.h"
#include "RenderPublic.h"
#include "Utility/BeatsUtility/StringHelper.h"

CSerializer CShaderProgram::m_vsPub;

CShaderProgram::CShaderProgram()
    : m_uProgram(0)
    , m_uVertexShader(0)
    , m_uFragmentShader(0)
{

}

CShaderProgram::~CShaderProgram()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->DetachShader(m_uProgram, m_uVertexShader);
    pRenderer->DetachShader(m_uProgram, m_uFragmentShader);

    pRenderer->DeleteShader( m_uVertexShader );
    pRenderer->DeleteShader( m_uFragmentShader );
    pRenderer->DeleteProgram( m_uProgram );
}

EResourceType CShaderProgram::GetType()
{
    return eRT_ShaderProgram;    
}

bool CShaderProgram::Load()
{
    std::vector<TString> result;
    CStringHelper::GetInstance()->SplitString(GetFilePath().c_str(), _T("@"), result);
    BEATS_ASSERT(result.size() == 2, _T("Invalid file name for shader: %s\nShould contain a @ to split vertex and pixel shader."), GetFilePath().c_str());
    m_bIsLoaded = InitWithFile(result[0].c_str(), result[1].c_str());
    return m_bIsLoaded;
}

bool CShaderProgram::Unload()
{
    m_bIsLoaded = false;
    return false;    
}

GLuint CShaderProgram::ID() const
{
    return m_uProgram;
}

bool CShaderProgram::InitWithFile( const TCHAR* pszVSFilePath, const TCHAR* pszFSFilePath )
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    bool bRet = true;
    m_uProgram = pRenderer->CreateProgram();
    FC_CHECK_GL_ERROR_DEBUG();

    bRet = CompileShader(&m_uVertexShader, GL_VERTEX_SHADER, pszVSFilePath) &&
                CompileShader(&m_uFragmentShader, GL_FRAGMENT_SHADER, pszFSFilePath);

    if (m_uVertexShader)
    {
        pRenderer->AttachShader(m_uProgram, m_uVertexShader);
    }
    FC_CHECK_GL_ERROR_DEBUG();

    if (m_uFragmentShader)
    {
        pRenderer->AttachShader(m_uProgram, m_uFragmentShader);
    }
    FC_CHECK_GL_ERROR_DEBUG();

    //TODO:Move it outside
    for(size_t i = 0; i < ATTRIB_INDEX_COUNT; ++i)
    {
        pRenderer->BindAttribLocation(m_uProgram, static_cast<ECommonAttribIndex>(i));
    }

    bRet = Link();
    BEATS_ASSERT(bRet, _T("Link shader program failed!"));
    FC_CHECK_GL_ERROR_DEBUG();

    for(size_t i = 0; i < UNIFORM_COUNT; ++i)
    {
        m_uniformLocations[static_cast<ECommonUniformType>(i)] = pRenderer->GetUniformLocation(m_uProgram, COMMON_UNIFORM_NAMES[i]);    
    }

#ifdef USE_UBO
    for(size_t i = 0; i < UNIFORM_BLOCK_COUNT; ++i)
    {
        GLint index = glGetUniformBlockIndex(m_uProgram, COMMON_UNIFORM_BLOCK_NAMES[i]);
        if(index != GL_INVALID_INDEX)
        {
            GLuint ubo = CRenderManager::GetInstance()->GetUBO(static_cast<ECommonUniformBlockType>(i));
            GLint blockSize;
            pRenderer->GetActiveUniformBlockiv(
                m_uProgram, index, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
            pRenderer->BindBuffer(GL_UNIFORM_BUFFER, ubo);
            pRenderer->BindBufferRange(GL_UNIFORM_BUFFER, i, ubo, 0, blockSize);
            pRenderer->UniformBlockBinding(m_uProgram, index, i);
        }
    }
#endif
    return bRet;
}

bool CShaderProgram::CompileShader(GLuint *shader, GLenum type, const TCHAR* pszShaderFilePath)
{
    bool bRet = false;
    CSerializer fileSerializer(pszShaderFilePath);
    if (fileSerializer.GetReadPos() < fileSerializer.GetWritePos())
    {
        bRet = CompileShader(shader, type, (const GLchar*)fileSerializer.GetBuffer(), fileSerializer.GetWritePos());
        BEATS_ASSERT(bRet, _T("Failed to compile vertex shader"));
    }
    return bRet;
}

bool CShaderProgram::CompileShader(GLuint * shader, GLenum type, const GLchar* source, GLint length)
{
    GLint status = GL_FALSE;

    if(m_vsPub.GetWritePos() == 0)
    {
#ifdef USE_UBO
        char def[] = "#define USE_UBO\r\n";
        m_vsPub.Serialize((const void *)def, sizeof(def) - 1);
#endif
        m_vsPub.Serialize(_T("..\\SourceCode\\Shader\\vs_pub.txt"));
    }

    if (source != NULL)
    {
        *shader = CRenderer::GetInstance()->CreateShader(type);
        BEATS_ASSERT( type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);
        if(type == GL_VERTEX_SHADER)
        {
            const GLchar *sources[2] = {0};
            GLint lengths[2] = {0};

            sources[0] = reinterpret_cast<const GLchar *>(m_vsPub.GetBuffer());
            sources[1] = source;
            lengths[0] = m_vsPub.GetWritePos();
            lengths[1] = length;
            CRenderer::GetInstance()->ShaderSource(*shader,  2, sources, lengths );
        }
        else
        {
            CRenderer::GetInstance()->ShaderSource(*shader, 1, &source, &length);
        }
        CRenderer::GetInstance()->CompileShader(*shader);
        CRenderer::GetInstance()->GetShaderiv(*shader, GL_COMPILE_STATUS, &status);
        BEATS_ASSERT(status);
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

bool CShaderProgram::Link()
{
    BEATS_ASSERT(m_uProgram != 0, _T("Cannot link invalid program"));

    GLint status = GL_TRUE;

    CRenderer::GetInstance()->LinkProgram(m_uProgram);

    if (m_uVertexShader)
    {
        CRenderer::GetInstance()->DeleteShader(m_uVertexShader);
    }

    if (m_uFragmentShader)
    {
        CRenderer::GetInstance()->DeleteShader(m_uFragmentShader);
    }

    m_uFragmentShader = m_uVertexShader = 0;

#ifdef _DEBUG
    CRenderer::GetInstance()->GetProgramiv(m_uProgram, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        BEATS_ASSERT(_T("cocos2d: ERROR: Failed to link program: %i"), m_uProgram);
        CRenderer::GetInstance()->DeleteProgram( m_uProgram );
        m_uProgram = 0;
    }
#endif

    return (status == GL_TRUE);
}
