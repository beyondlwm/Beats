#ifndef RENDER_SHADER_H__INCLUDE
#define RENDER_SHADER_H__INCLUDE

#include "resource/Resource.h"
#include "Utility/BeatsUtility/Serializer.h"

class CShader : public CResource
{
    DECLARE_RESOURCE_TYPE(eRT_Shader)

public:
    CShader();
    virtual ~CShader();

    GLenum GetShaderType()const;
    void SetShaderType(GLenum shaderType);

    virtual bool Load() override;

    virtual bool Unload() override;

    GLuint ID() const;

private:
    bool Compile(GLuint *shader, const TCHAR* pszShaderFilePath);
    bool Compile(GLuint * shader, const GLchar* pszSource, GLint length);

private:
    GLenum m_shaderType;
    GLuint m_uId;
    static CSerializer m_vsPublicHeader;
};

#endif