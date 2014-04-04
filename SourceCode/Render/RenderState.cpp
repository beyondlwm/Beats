#include "stdafx.h"
#include "RenderState.h"
#include "Renderer.h"
#include "Render/RenderStateParam/BoolRenderStateParam.h"
#include "Render/RenderStateParam/UintRenderStateParam.h"
#include "Render/RenderStateParam/IntRenderStateParam.h"
#include "Render/RenderStateParam/FloatRenderStateParam.h"
#include "Render/RenderStateParam/FunctionRenderStateParam.h"
#include "Render/RenderStateParam/BlendRenderStateParam.h"
#include "Render/RenderStateParam/BlendEquationRenderStateParam.h"
#include "Render/RenderStateParam/CullModeRenderStateParam.h"
#include "Render/RenderStateParam/ClockWiseRenderStateParam.h"
#include "Render/RenderStateParam/PolygonModeRenderStateParam.h"
#include "Render/RenderStateParam/ShadeModeRenderStateParam.h"
#include "Render/RenderStateParam/StencilRenderStateParam.h"

CRenderState::CRenderState()
    : m_pStencilRenderState(NULL)
    , m_pShadeModeRenderState(NULL)
    , m_pCullModeRenderState(NULL)
    , m_pClockWiseRenderState(NULL)
    , m_pBlendRenderState(NULL)
    , m_pBlendEquationRenderState(NULL)
    , m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
{
}

CRenderState::CRenderState(CSerializer& serializer)
    : m_pStencilRenderState(NULL)
    , m_pShadeModeRenderState(NULL)
    , m_pCullModeRenderState(NULL)
    , m_pClockWiseRenderState(NULL)
    , m_pBlendRenderState(NULL)
    , m_pBlendEquationRenderState(NULL)
    , m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
{
    DECLARE_PROPERTY(serializer, m_pStencilRenderState, true, 0xFFFFFFFF, _T("模板状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pShadeModeRenderState, true, 0xFFFFFFFF, _T("着色模式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pCullModeRenderState, true, 0xFFFFFFFF, _T("背面剔除模式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pClockWiseRenderState, true, 0xFFFFFFFF, _T("顶点顺序"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pBlendRenderState, true, 0xFFFFFFFF, _T("混合来源"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_pBlendEquationRenderState, true, 0xFFFFFFFF, _T("混合方式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_polygonModeRenderState, true, 0xFFFFFFFF, _T("图元模式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_funcRenderState, true, 0xFFFFFFFF, _T("运算方式"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uintRenderState, true, 0xFFFFFFFF, _T("UINT状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_intRenderState, true, 0xFFFFFFFF, _T("INT状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_floatRenderState, true, 0xFFFFFFFF, _T("FLOAT状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_boolRenderState, true, 0xFFFFFFFF, _T("BOOL状态"), NULL, NULL, NULL);
}

CRenderState::~CRenderState()
{
    BEATS_SAFE_DELETE(m_pStencilRenderState);
    BEATS_SAFE_DELETE(m_pShadeModeRenderState);
    BEATS_SAFE_DELETE(m_pCullModeRenderState);
    BEATS_SAFE_DELETE(m_pClockWiseRenderState);
    BEATS_SAFE_DELETE(m_pBlendRenderState);
    BEATS_SAFE_DELETE(m_pBlendEquationRenderState);
    BEATS_SAFE_DELETE_VECTOR(m_boolRenderState);
    m_boolRenderStateMap.clear();
}

void CRenderState::SetBoolState(GLenum state, bool bEnable)
{
    CBoolRenderStateParam* pParam = NULL;
    std::map<GLenum, CBoolRenderStateParam*>::iterator iter = m_boolRenderStateMap.find(state);
    if (iter == m_boolRenderStateMap.end())
    {
        pParam = new CBoolRenderStateParam;
        pParam->SetType((CBoolRenderStateParam::EBoolStateParam)state);
        m_boolRenderState.push_back(pParam);
        m_boolRenderStateMap[state] = pParam;
    }
    else
    {
        pParam = iter->second;
    }
    pParam->SetValue(bEnable);
}

bool CRenderState::GetBoolState(GLenum state) const
{
    bool bRet = false;
    std::map<GLenum, CBoolRenderStateParam*>::const_iterator iter = m_boolRenderStateMap.find(state);
    if (iter == m_boolRenderStateMap.end())
    {
       bRet = CRenderer::GetInstance()->IsEnable(state);
    }
    else
    {
        bRet = iter->second->GetValue();
    }
    return bRet;
}

void CRenderState::SetDepthMask(bool bWriteable)
{
}

bool CRenderState::GetDepthMask() const
{
    return false;
}

void CRenderState::SetEdgeFlag(bool bEdgeFlag)
{
}

bool CRenderState::GetEdgetFlag() const
{
    return false;
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
}

void CRenderState::SetBlendFuncTargetFactor(GLenum target)
{
}

GLenum CRenderState::GetBlendSrcFactor() const
{
    return 0;
}

GLenum CRenderState::GetBlendTargetFactor() const
{
    return 0;
}

void CRenderState::SetBlendEquation(GLenum func)
{
}

GLenum CRenderState::GetBlendEquation()
{
    return 0;
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

void CRenderState::SetFrontFace(GLenum frontFace)
{
}

GLenum CRenderState::GetFrontFace() const
{
    return 0;
}

void CRenderState::SetCullFace(GLenum cullFace)
{
}

GLenum CRenderState::GetCullFace() const
{
    return 0;
}

void CRenderState::SetDepthNear(float fDepthNear)
{
}

void CRenderState::SetDepthFar(float fDepthFar)
{
}

float CRenderState::GetDepthNear() const
{
    return 0;
}

float CRenderState::GetDepthFar() const
{
    return 0;
}

void CRenderState::SetDepthFunc(GLenum func)
{
}

GLenum CRenderState::GetDepthFunc() const
{
    return 0;
}

void CRenderState::SetStencilFunc(GLenum func)
{
}

GLenum CRenderState::GetStencilFunc() const
{
    return 0;
}

void CRenderState::SetStencilReference(GLint nRef)
{
}

GLint CRenderState::GetStencilReference() const
{
    return 0;
}

void CRenderState::SetStencilValueMask(GLint nValueMask)
{
}

GLint CRenderState::GetStencilValueMask() const
{
    return 0;
}

void CRenderState::SetClearStencil(GLint nClearValue)
{
}

GLint CRenderState::GetClearStencil() const
{
    return 0;
}

void CRenderState::SetShadeModel(GLenum shadeModel)
{
}

GLenum CRenderState::GetShadeModel() const
{
    return 0;
}

void CRenderState::Restore()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->UseProgram(m_uCurrShaderProgram);
    if (m_pStencilRenderState)
    {
        m_pStencilRenderState->Apply();
    }
    if (m_pShadeModeRenderState)
    {
        m_pShadeModeRenderState->Apply();
    }
    if (m_pCullModeRenderState)
    {
        m_pCullModeRenderState->Apply();
    }
    if (m_pClockWiseRenderState)
    {
        m_pClockWiseRenderState->Apply();
    }
    if (m_pBlendRenderState)
    {
        m_pBlendRenderState->Apply();
    }
    if (m_pBlendEquationRenderState)
    {
        m_pBlendEquationRenderState->Apply();
    }
    for (size_t i = 0; i < m_uintRenderState.size(); ++i)
    {
        CUintRenderStateParam* pParam = m_uintRenderState[i];
        pParam->Apply();
    }
    for (size_t i = 0; i < m_polygonModeRenderState.size(); ++i)
    {
        CPolygonModeRenderStateParam* pParam = m_polygonModeRenderState[i];
        pParam->Apply();
    }
    for (size_t i = 0; i < m_intRenderState.size(); ++i)
    {
        CIntRenderStateParam* pParam = m_intRenderState[i];
        pParam->Apply();
    }
    for (size_t i = 0; i < m_funcRenderState.size(); ++i)
    {
        CFunctionRenderStateParam* pParam = m_funcRenderState[i];
        pParam->Apply();
    }    
    for (size_t i = 0; i < m_floatRenderState.size(); ++i)
    {
        CFloatRenderStateParam* pParam = m_floatRenderState[i];
        pParam->Apply();
    }
    for (size_t i = 0; i < m_boolRenderState.size(); ++i)
    {
        CBoolRenderStateParam* pParam = m_boolRenderState[i];
        pParam->Apply();
    }
}
