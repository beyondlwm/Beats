#include "stdafx.h"
#include "renderer.h"
#include "RenderManager.h"
#include "RenderState.h"
#include "Camera.h"

CRenderer* CRenderer::m_pInstance = NULL;

CRenderer::CRenderer()
{
    m_pCurrState = new CRenderState;
}

CRenderer::~CRenderer()
{
    BEATS_SAFE_DELETE(m_pCurrState);
}

void CRenderer::GetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
{
    glGetBufferParameteriv(target, pname, params);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::UseProgram(GLuint uProgram)
{
    if(m_pCurrState->GetCurrentShaderProgram() != uProgram)
    {
        m_pCurrState->SetCurrentShaderProgram(uProgram);
        glUseProgram(uProgram);
        FC_CHECK_GL_ERROR_DEBUG();
        CRenderManager::GetInstance()->GetCamera()->ApplyCameraChange();
    }
}

GLuint CRenderer::CreateShader(GLenum type)
{
    GLuint uRet = glCreateShader(type);
    FC_CHECK_GL_ERROR_DEBUG();
    return uRet;
}

void CRenderer::DeleteShader(GLuint uProgram)
{
    glDeleteShader(uProgram);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
    glShaderSource(shader, count, strings, lengths);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::CompileShader(GLuint shader)
{
    glCompileShader(shader);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetProgramiv(GLuint program, GLenum pname, GLint* param)
{
    glGetProgramiv(program, pname, param);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetShaderiv(GLuint shader, GLenum pname, GLint* param)
{
    glGetShaderiv(shader, pname, param);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog(shader, bufSize, length, infoLog);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::LinkProgram(GLuint program)
{
    glLinkProgram(program);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::AttachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DetachShader(GLuint program, GLuint shader)
{
    glDetachShader(program, shader);
    FC_CHECK_GL_ERROR_DEBUG();
}

GLuint CRenderer::CreateProgram()
{
    GLuint uProgram = glCreateProgram();
    FC_CHECK_GL_ERROR_DEBUG();
    return uProgram;
}

void CRenderer::DeleteProgram(GLuint uProgram)
{
    glDeleteProgram(uProgram);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindAttribLocation(GLuint uProgram, ECommonAttribIndex attribIndex)
{
    glBindAttribLocation(uProgram, attribIndex, COMMON_ATTIB_NAMES[attribIndex]);
    FC_CHECK_GL_ERROR_DEBUG();
}

GLint CRenderer::GetUniformLocation(GLuint uProgram, const char *name ) const
{
    BEATS_ASSERT(name != nullptr, _T("Invalid uniform name") );
    BEATS_ASSERT(uProgram != 0, _T("Invalid operation. Cannot get uniform location when program is not initialized"));
    GLint nRet = glGetUniformLocation(uProgram, name);
    FC_CHECK_GL_ERROR_DEBUG();
    return nRet;
}

void CRenderer::GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
    glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1i(GLint location, GLint i1)
{
    glUniform1i(location, i1);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
    glUniform4f(location, f1, f2, f3, f4);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats)
{
    glUniform1fv(location, numberOfFloats, floats);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix4fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices)
{
    glUniformMatrix4fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::LineWidth(GLfloat fLineWidth)
{
    BEATS_ASSERT(fLineWidth > 0);
    glLineWidth(fLineWidth);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* pIndices )
{
    BEATS_ASSERT(type == GL_UNSIGNED_BYTE || type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT,
        _T("Invalid type parameter of glDrawElement!\ntype: %d"), type);
    glDrawElements(mode, count, type, pIndices);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GenVertexArrays( GLsizei n, GLuint* arrays )
{
    glGenVertexArrays(n, arrays);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindVertexArray( GLuint array )
{
    glBindVertexArray(array);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteVertexArrays(GLsizei n, GLuint *arrays)
{
    glDeleteVertexArrays(n, arrays);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GenBuffers( GLsizei n, GLuint* buffers )
{
    glGenBuffers(n, buffers);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindBuffer(GLenum target, GLuint buffer )
{
    glBindBuffer(target, buffer);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
{
    glBindBufferRange(target, index, buffer, offset, size);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DeleteBuffers(GLsizei n, GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::EnableVertexAttribArray( GLuint attribute )
{
    glEnableVertexAttribArray(attribute);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DisableVertexAttribArray(GLuint attribute)
{
    glDisableVertexAttribArray(attribute);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    BEATS_ASSERT( type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT );
    glVertexAttribIPointer(index, size, type, stride, pointer);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ActiveTexture(GLenum texture)
{
    if (m_pCurrState->GetActiveTexture() != texture)
    {
        glActiveTexture(texture);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetActiveTexture(texture);
    }
}

void CRenderer::GenTextures(GLsizei n, GLuint *textures)
{
    glGenTextures(n, textures);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BindTexture(GLenum target, GLuint texture)
{
    glBindTexture( target, texture);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TextureImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::TexParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DelTexture(GLsizei n, GLuint* textures)
{
    glDeleteTextures(n, textures);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::GetIntegerV( GLenum pname, GLint * params )
{
    glGetIntegerv(pname, params);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::PixelStorei (GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glScissor(x, y, width, height);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::EnableGL(GLenum cap)
{
    if (!m_pCurrState->GetBoolState(cap))
    {
        glEnable(cap);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetBoolState(cap, true);
    }
}

void CRenderer::DisableGL(GLenum cap)
{
    if (m_pCurrState->GetBoolState(cap))
    {
        glDisable(cap);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetBoolState(cap, false);
    }
}

bool CRenderer::IsEnable(GLenum cap)
{
    return glIsEnabled(cap) == GL_TRUE;
}

void CRenderer::BlendFunc(GLenum sfactor, GLenum dfactor)
{
    bool bChanged = false;
    if (m_pCurrState->GetBlendSrcFactor() != sfactor)
    {
        bChanged = true;
        m_pCurrState->SetBlendFuncSrcFactor(sfactor);
    }
    if (m_pCurrState->GetBlendTargetFactor() != dfactor)
    {
        bChanged = true;
        m_pCurrState->SetBlendFuncTargetFactor(dfactor);
    }
    if (bChanged)
    {
        glBlendFunc(sfactor, dfactor);
    }
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BlendEquation(GLenum func)
{
    if (m_pCurrState->GetBlendEquation() != func)
    {
        m_pCurrState->SetBlendEquation(func);
        glBlendEquation(func);
        FC_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::BlendColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
    GLclampf rtemp, gtemp, btemp, atemp;
    m_pCurrState->GetColor( CUintRenderStateParam::EUintStateParam::eUSP_BlendColor, rtemp, gtemp, btemp, atemp );
    if ( rtemp != r || gtemp != g || btemp != b || atemp != a )
    {
        glBlendColor(r, g, b, a);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetColor( CUintRenderStateParam::EUintStateParam::eUSP_BlendColor, r, g, b, a );
    }
}

void CRenderer::PolygonMode (GLenum face, GLenum mode)
{
    GLenum tempFace, tempMode;
    m_pCurrState->GetPolygonMode( tempFace, tempMode );
    if ( tempFace != face || tempMode != mode )
    {
        glPolygonMode(face, mode);
        FC_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::ClearBuffer(GLbitfield bit )
{
    glClear(bit);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLclampf rtemp, gtemp, btemp, atemp;
    m_pCurrState->GetColor( CUintRenderStateParam::EUintStateParam::eUSP_ClearColor, rtemp, gtemp, btemp, atemp );
    if ( rtemp != red || gtemp != green || btemp != blue || atemp != alpha )
    {
        glClearColor(red, green, blue, alpha);
        FC_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::ClearDepth(GLclampf depth)
{
    BEATS_ASSERT(depth >= 0 && depth <= 1);
    glClearDepth(depth);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target, offset, size, data);
    FC_CHECK_GL_ERROR_DEBUG();
}

GLvoid* CRenderer::MapBuffer(GLenum target, GLenum access)
{
    GLvoid* pData = glMapBuffer(target, access);
    FC_CHECK_GL_ERROR_DEBUG();
    return pData;
}

void CRenderer::UnmapBuffer(GLenum target)
{
    glUnmapBuffer(target);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::DepthMask(bool bWriteable)
{
    if (m_pCurrState->GetDepthMask() != bWriteable)
    {
        glDepthMask(bWriteable);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetDepthMask(bWriteable);
    }
}

void CRenderer::EdgeFlag(bool bEdgeFlag)
{
    if (m_pCurrState->GetEdgetFlag() != bEdgeFlag)
    {
        glEdgeFlag(bEdgeFlag);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetEdgeFlag(bEdgeFlag);
    }
}

void CRenderer::FrontFace(GLenum frontFace)
{
    if (m_pCurrState->GetFrontFace() != frontFace)
    {
        glFrontFace(frontFace);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetFrontFace(frontFace);
    }
}

void CRenderer::CullFace(GLenum cullFace)
{
    if (m_pCurrState->GetCullFace() != cullFace)
    {
        glCullFace(cullFace);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetCullFace(cullFace);
    }
}

void CRenderer::DepthRange(float fNear, float fFar)
{
    bool bChanged = false;
    if (!BEATS_FLOAT_EQUAL(m_pCurrState->GetDepthFar(), fFar))
    {
        m_pCurrState->SetDepthFar(fFar);
        bChanged = true;
    }
    if (!BEATS_FLOAT_EQUAL(m_pCurrState->GetDepthNear(), fNear))
    {
        m_pCurrState->SetDepthNear(fNear);
        bChanged = true;
    }
    if (bChanged)
    {
        glDepthRange(fNear, fFar);
        FC_CHECK_GL_ERROR_DEBUG();
    }
}

void CRenderer::DepthFunc(GLenum depthFunc)
{
    if(m_pCurrState->GetDepthFunc() != depthFunc)
    {
        glDepthFunc(depthFunc);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetDepthFunc(depthFunc);
    }
}

void CRenderer::StencilFunc(GLenum stencilFunc)
{
    if(m_pCurrState->GetStencilFunc() != stencilFunc)
    {
        glStencilFunc(stencilFunc, m_pCurrState->GetStencilReference(), m_pCurrState->GetStencilValueMask());
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilFunc(stencilFunc);
    }
}

void CRenderer::StencilReference(GLint nRef)
{
    if (m_pCurrState->GetStencilReference() != nRef)
    {
        m_pCurrState->SetStencilReference(nRef);
    }
}

void CRenderer::StencilValueMask(GLint nValueMask)
{
    if (m_pCurrState->GetStencilValueMask() != nValueMask)
    {
        m_pCurrState->SetStencilValueMask(nValueMask);
    }
}

void CRenderer::ClearStencil(GLint nClearValue)
{
    if (m_pCurrState->GetClearStencil() != nClearValue)
    {
        glClearStencil(nClearValue);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetClearStencil(nClearValue);
    }
    
}

void CRenderer::StencilOp(GLenum fail, GLenum zFail, GLenum zPass)
{
    GLenum failTemp, zFailTemp, zPassTemp;
    m_pCurrState->GetStencilOp( failTemp, zFailTemp, zPassTemp );
    if ( failTemp != fail || zFailTemp != zFail || zPassTemp != zPass )
    {
        glStencilOp(fail, zFail, zPass);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetStencilOp( fail, zFail, zPass );
    }
}

void CRenderer::ShadeModel(GLenum shadeModel)
{
    if (m_pCurrState->GetShadeModel() != shadeModel)
    {
        glShadeModel(shadeModel);
        FC_CHECK_GL_ERROR_DEBUG();
        m_pCurrState->SetShadeModel(shadeModel);
    }
}

CRenderState* CRenderer::GetCurrentState() const
{
    return m_pCurrState;
}

void CRenderer::GetFloatV( GLenum pname, GLfloat * params )
{
    glGetFloatv( pname, params );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform2fv( location, numberOfFloats, floats );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform3fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform3fv( location, numberOfFloats, floats );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats )
{
    glUniform4fv( location, numberOfFloats, floats );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform1iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform1iv( location, numberOfInt, ints );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform2iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform2iv( location, numberOfInt, ints );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform3iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform3iv( location, numberOfInt, ints );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniform4iv( GLint location, const GLint *ints, GLsizei numberOfInt )
{
    glUniform3iv( location, numberOfInt, ints );
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix2fv( GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices )
{
    glUniformMatrix2fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    FC_CHECK_GL_ERROR_DEBUG();
}

void CRenderer::SetUniformMatrix3fv( GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices )
{
    glUniformMatrix3fv(location, numberOfMatrices, GL_FALSE, matrixArray);
    FC_CHECK_GL_ERROR_DEBUG();
}
