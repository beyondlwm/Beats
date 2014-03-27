#ifndef RENDER_SHADERPROGRAM_H__INCLUDE
#define RENDER_SHADERPROGRAM_H__INCLUDE

#include "resource/Resource.h"
#include "Utility/BeatsUtility/Serializer.h"

class CShaderProgram : public CResource
{
public:
    CShaderProgram();
    virtual ~CShaderProgram();
    
    virtual EResourceType GetType();

    virtual bool Load();

    virtual bool Unload();

    GLuint ID() const;

private:
    bool InitWithFile(const TCHAR* pszVSFilePath, const TCHAR* pszFSFilePath);
    bool CompileShader(GLuint *shader, GLenum type, const TCHAR* pszShaderFilePath);
    bool CompileShader(GLuint * shader, GLenum type, const GLchar* source, GLint length);
    bool Link();

private:
    GLuint m_uProgram;
    GLuint m_uVertexShader;
    GLuint m_uFragmentShader;
    static CSerializer m_vsPub;

    std::map<ECommonUniformType, GLuint> m_uniformLocations;

};

#endif