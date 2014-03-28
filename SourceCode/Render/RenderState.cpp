#include "stdafx.h"
#include "RenderState.h"
#include "Renderer.h"

CRenderState::CRenderState()
    : m_bDepthMask(true)
    , m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
    , m_uBlendSourceFactor(GL_SRC_ALPHA)
    , m_uBlendTargetFactor(GL_ONE_MINUS_SRC_ALPHA)
    , m_uBlendEquation(GL_FUNC_ADD)
{

}

CRenderState::~CRenderState()
{

}

void CRenderState::SetBoolState(GLenum state, bool bEnable)
{
    m_boolStateMap[state] = bEnable;
}

bool CRenderState::GetBoolState(GLenum state) const
{
    auto stateIter = m_boolStateMap.find(state);
    return stateIter != m_boolStateMap.end() ? stateIter->second : glIsEnabled(state) == GL_TRUE;
}

void CRenderState::SetDepthMask(bool bWriteable)
{
    m_bDepthMask = bWriteable;
}

bool CRenderState::GetDepthMask() const
{
    return m_bDepthMask;
}

void CRenderState::SetActiveTexture(GLenum activeTexture)
{
    m_uCurrActiveTexture = activeTexture;
}

GLenum CRenderState::GetActiveTexture() const
{
    return m_uCurrActiveTexture;
}

void CRenderState::SetBlendFuncSrcFactor(GLenum src)
{
    m_uBlendSourceFactor = src;
}

void CRenderState::SetBlendFuncTargetFactor(GLenum target)
{
    m_uBlendTargetFactor = target;
}

GLenum CRenderState::GetBlendSrcFactor() const
{
    return m_uBlendSourceFactor;
}

GLenum CRenderState::GetBlendTargetFactor() const
{
    return m_uBlendTargetFactor;
}

void CRenderState::SetBlendEquation(GLenum func)
{
    m_uBlendEquation = func;
}

GLenum CRenderState::GetBlendEquation()
{
    return m_uBlendEquation;
}

void CRenderState::SetCurrentShaderProgram(GLuint program)
{
    BEATS_ASSERT(program != m_uCurrShaderProgram);
    m_uCurrShaderProgram = program;
}

GLuint CRenderState::GetCurrentShaderProgram()const
{
    return  m_uCurrShaderProgram;
}

void CRenderState::Restore()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->UseProgram(m_uCurrShaderProgram);
    pRenderer->DepthMask(m_bDepthMask);
    pRenderer->BlendFunc(m_uBlendSourceFactor, m_uBlendTargetFactor);
    pRenderer->BlendEquation(m_uBlendEquation);

    for (std::map<GLenum, bool>::iterator iter = m_boolStateMap.begin(); iter != m_boolStateMap.end(); ++iter)
    {
        iter->second ? pRenderer->EnableGL(iter->first) : pRenderer->DisableGL(iter->first);
    }
}
