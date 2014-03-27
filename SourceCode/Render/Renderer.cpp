#include "stdafx.h"
#include "renderer.h"
#include "RenderManager.h"
#include "RenderState.h"

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
}

void CRenderer::UseProgram(GLuint uProgram)
{
    if(m_pCurrState->GetCurrentShaderProgram() != uProgram)
    {
        m_pCurrState->SetCurrentShaderProgram(uProgram);
        glUseProgram(uProgram);
    }

    CRenderManager::GetInstance()->TransferMVPMatrix();
}

GLuint CRenderer::CreateShader(GLenum type)
{
    return glCreateShader(type);
}

void CRenderer::DeleteShader(GLuint uProgram)
{
    glDeleteShader(uProgram);
}

void CRenderer::ShaderSource(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths)
{
    glShaderSource(shader, count, strings, lengths);
}

void CRenderer::CompileShader(GLuint shader)
{
    glCompileShader(shader);
}

void CRenderer::GetProgramiv(GLuint program, GLenum pname, GLint* param)
{
    glGetProgramiv(program, pname, param);
}

void CRenderer::GetShaderiv(GLuint shader, GLenum pname, GLint* param)
{
    glGetShaderiv(shader, pname, param);
}

void CRenderer::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
    glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

void CRenderer::LinkProgram(GLuint program)
{
    glLinkProgram(program);
}

void CRenderer::AttachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
}

void CRenderer::DetachShader(GLuint program, GLuint shader)
{
    glDetachShader(program, shader);
}

GLuint CRenderer::CreateProgram()
{
    return glCreateProgram();
}

void CRenderer::DeleteProgram(GLuint uProgram)
{
    glDeleteProgram(uProgram);
}

void CRenderer::BindAttribLocation(GLuint uProgram, ECommonAttribIndex attribIndex)
{
    glBindAttribLocation(uProgram, attribIndex, COMMON_ATTIB_NAMES[attribIndex]);
}

GLint CRenderer::GetUniformLocation(GLuint uProgram, const char *name ) const
{
    BEATS_ASSERT(name != nullptr, _T("Invalid uniform name") );
    BEATS_ASSERT(uProgram != 0, _T("Invalid operation. Cannot get uniform location when program is not initialized"));

    return glGetUniformLocation(uProgram, name);
}

void CRenderer::GetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
{
    glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
}

void CRenderer::SetUniform1i(GLint location, GLint i1)
{
    glUniform1i(location, i1);
}

void CRenderer::SetUniform4f(GLint location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
    glUniform4f(location, f1, f2, f3, f4);
}

void CRenderer::SetUniform1fv( GLint location, const GLfloat *floats, GLsizei numberOfFloats)
{
    glUniform1fv(location, numberOfFloats, floats);
}

void CRenderer::SetUniformMatrix4fv(GLint location, const GLfloat *matrixArray, GLsizei numberOfMatrices)
{
    glUniformMatrix4fv(location, numberOfMatrices, GL_FALSE, matrixArray);
}

void CRenderer::UniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
}

void CRenderer::LineWidth(GLfloat fLineWidth)
{
    BEATS_ASSERT(fLineWidth > 0);
    glLineWidth(fLineWidth);
}

void CRenderer::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
}

void CRenderer::DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid* pIndices )
{
    BEATS_ASSERT(type == GL_UNSIGNED_BYTE || type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT,
        _T("Invalid type parameter of glDrawElement!\ntype: %d"), type);
    glDrawElements(mode, count, type, pIndices);
}

void CRenderer::GenVertexArrays( GLsizei n, GLuint* arrays )
{
    glGenVertexArrays(n, arrays);
}

void CRenderer::BindVertexArray( GLuint array )
{
    glBindVertexArray(array);
}

void CRenderer::DeleteVertexArrays(GLsizei n, GLuint *arrays)
{
    glDeleteVertexArrays(n, arrays);
}

void CRenderer::GenBuffers( GLsizei n, GLuint* buffers )
{
    glGenBuffers(n, buffers);
}

void CRenderer::BindBuffer(GLenum target, GLuint buffer )
{
    glBindBuffer(target, buffer);
}

void CRenderer::BindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
{
    glBindBufferRange(target, index, buffer, offset, size);
}

void CRenderer::DeleteBuffers(GLsizei n, GLuint *buffers)
{
    glDeleteBuffers(n, buffers);
}

void CRenderer::EnableVertexAttribArray( GLuint attribute )
{
    glEnableVertexAttribArray(attribute);
}

void CRenderer::DisableVertexAttribArray(GLuint attribute)
{
    glDisableVertexAttribArray(attribute);
}

void CRenderer::VertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void CRenderer::VertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    BEATS_ASSERT( type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT );
    glVertexAttribIPointer(index, size, type, stride, pointer);
}

void CRenderer::BufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}
void CRenderer::ActiveTexture(GLenum texture)
{
    if (m_pCurrState->GetActiveTexture() != texture)
    {
        glActiveTexture(texture);
        m_pCurrState->SetActiveTexture(texture);
    }
}

void CRenderer::GenTextures(GLsizei n, GLuint *textures)
{
    glGenTextures(n, textures);
}

void CRenderer::BindTexture(GLenum target, GLuint texture)
{
    glBindTexture( target, texture);
}

void CRenderer::CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void CRenderer::TextureImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void CRenderer::TexParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

void CRenderer::DelTexture(GLsizei n, GLuint* textures)
{
    glDeleteTextures(n, textures);
}

void CRenderer::GetIntegerV( GLenum pname, GLint * params )
{
    glGetIntegerv(pname, params);
}

void CRenderer::PixelStorei (GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
}

void CRenderer::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glViewport(x, y, width, height);
}

void CRenderer::SetScissorRect(GLint x, GLint y, GLsizei width, GLsizei height)
{
    glScissor(x, y, width, height);
}

void CRenderer::EnableGL(GLenum cap)
{
    if (!m_pCurrState->GetBoolState(cap))
    {
        glEnable(cap);
        m_pCurrState->SetBoolState(cap, true);
    }
}

void CRenderer::DisableGL(GLenum cap)
{
    if (m_pCurrState->GetBoolState(cap))
    {
        glDisable(cap);
        m_pCurrState->SetBoolState(cap, false);
    }
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
}

void CRenderer::BlendEquation(GLenum func)
{
    if (m_pCurrState->GetBlendEquation() != func)
    {
        m_pCurrState->SetBlendEquation(func);
        glBlendEquation(func);
    }
}

void CRenderer::PolygonMode (GLenum face, GLenum mode)
{
    glPolygonMode(face, mode);
}

void CRenderer::PointSize(GLfloat fSize)
{
    glPointSize(fSize);
}

void CRenderer::ClearBuffer(GLbitfield bit )
{
    glClear(bit);
}

void CRenderer::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    glClearColor(red, green, blue, alpha);
}

void CRenderer::ClearDepth(GLclampf depth)
{
    BEATS_ASSERT(depth >= 0 && depth <= 1);
    glClearDepth(depth);
}

void CRenderer::BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target, offset, size, data);
}

GLvoid* CRenderer::MapBuffer(GLenum target, GLenum access)
{
    return glMapBuffer(target, access);
}

void CRenderer::UnmapBuffer(GLenum target)
{
    glUnmapBuffer(target);
}

void CRenderer::DepthMask(bool bWriteable)
{
    if (m_pCurrState->GetDepthMask() != bWriteable)
    {
        glDepthMask(bWriteable);
        m_pCurrState->SetDepthMask(bWriteable);
    }
}

CRenderState* CRenderer::GetCurrentState() const
{
    return m_pCurrState;
}
