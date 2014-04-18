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
    :m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
    , m_bDepthMark(true)
    , m_bEdgeFlag(false)
{
}

CRenderState::CRenderState(CSerializer& serializer)
    : m_uCurrShaderProgram(0)
    , m_uCurrActiveTexture(0)
    , m_bDepthMark(true)
    , m_bEdgeFlag(false)
{
    DECLARE_PROPERTY(serializer, m_pRenderStateParams, true, 0xFFFFFFFF, _T("渲染状态"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bDepthMark, true, 0xFFFFFFFF, _T("深度写入"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bEdgeFlag, true, 0xFFFFFFFF, _T("边缘模式"), NULL, NULL, NULL);
}

CRenderState::~CRenderState()
{
    for ( auto iter : m_pRenderStateParams )
    {
        BEATS_SAFE_DELETE( iter );
    }
    m_pRenderStateParams.clear();
    m_boolRenderStateVector.clear();
    m_intRenderStateVector.clear();
    m_funcRenderStateVector.clear();
    m_floatRenderStateVector.clear();
    m_unitRenderStateVector.clear();
}

void CRenderState::SetBoolState(GLenum state, bool bEnable)
{
    CBoolRenderStateParam* pParam = ( CBoolRenderStateParam* )GetBoolRenderStateParamPtr( 
        (CBoolRenderStateParam::EBoolStateParam) state );
    if ( NULL == pParam )
    {
        pParam = new CBoolRenderStateParam();
        pParam->SetType( (CBoolRenderStateParam::EBoolStateParam)state );
        m_pRenderStateParams.push_back( pParam );
        m_boolRenderStateVector.push_back( pParam );
    }
    pParam->SetValue(bEnable);
}

bool CRenderState::GetBoolState(GLenum state) const
{
    bool bRet = false;
    CBoolRenderStateParam* pParam = ( CBoolRenderStateParam* )GetBoolRenderStateParamPtr(
       (CBoolRenderStateParam::EBoolStateParam) state );
    if ( NULL == pParam )
    {
        bRet = CRenderer::GetInstance()->IsEnable( state );
    }
    else
    {
        bRet = pParam->GetValue();
    }
    return bRet;
}

void CRenderState::SetDepthMask(bool bWriteable)
{
    m_bDepthMark = bWriteable;
}

bool CRenderState::GetDepthMask() const
{
    return m_bDepthMark;
}

void CRenderState::SetEdgeFlag(bool bEdgeFlag)
{
    m_bEdgeFlag = bEdgeFlag;
}

bool CRenderState::GetEdgetFlag() const
{
    return m_bEdgeFlag;
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
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetSrcFactor( (CBlendRenderStateParam::EBlendParamType)src );
}

void CRenderState::SetBlendFuncTargetFactor(GLenum target)
{
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetTargetFactor( (CBlendRenderStateParam::EBlendParamType)target );
}

GLenum CRenderState::GetBlendSrcFactor() const
{
    GLint retEnum = 0;
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_SRC, &retEnum );
    }
    else
    {
        retEnum = (GLint)pParam->GetSrcFactor();
    }
    return retEnum;
}

GLenum CRenderState::GetBlendTargetFactor() const
{
    GLint retEnum = 0;
    CBlendRenderStateParam* pParam = ( CBlendRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_DST, &retEnum );
    }
    else
    {
        retEnum = (GLenum)pParam->GetTargetFactor();
    }
    return retEnum;
}

void CRenderState::SetBlendEquation(GLenum func)
{
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendEquationMode );
    if ( NULL == pParam )
    {
        pParam = new CBlendEquationRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetBlendEquationType( (CBlendEquationRenderStateParam::EBlendEquationType) func );
}

GLenum CRenderState::GetBlendEquation()
{
    GLint retEnum = 0;
    CBlendEquationRenderStateParam* pParam = ( CBlendEquationRenderStateParam* )GetRenderStateParamBasePtr( eRS_BlendEquationMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_BLEND_EQUATION_RGB, &retEnum );
    }
    else
    {
        retEnum = (GLint)pParam->GetBlendEquationType();
    }
    return retEnum;
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
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRS_ClockWiseMode );
    if ( NULL == pParam )
    {
        pParam = new CClockWiseRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetClockWiseType( (CClockWiseRenderStateParam::EClockWiseType)frontFace );
}

GLenum CRenderState::GetFrontFace() const
{
    GLint retEunm = 0;
    CClockWiseRenderStateParam* pParam = ( CClockWiseRenderStateParam* )GetRenderStateParamBasePtr( eRS_ClockWiseMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_FRONT_FACE, &retEunm );
    }
    else
    {
        retEunm = (GLint)pParam->GetClockWiseType();
    }
    
    return retEunm;
}

void CRenderState::SetCullFace(GLenum cullFace)
{
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_CullMode );
    if ( NULL == pParam )
    {
        pParam = new CCullModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetCullModeType( (CCullModeRenderStateParam::ECullModeType)cullFace );
}

GLenum CRenderState::GetCullFace() const
{
    GLint retEnum = 0;
    CCullModeRenderStateParam* pParam = ( CCullModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_CullMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_CULL_FACE_MODE, &retEnum);
    }
    else
    {
        retEnum = (CCullModeRenderStateParam::ECullModeType)pParam->GetCullModeType();
    }
    
    return retEnum;
}

void CRenderState::SetDepthNear(float fDepthNear)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthNear );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthNear );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fDepthNear );
}

void CRenderState::SetDepthFar(float fDepthFar)
{
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthFar );
    if ( NULL == pParam )
    {
        pParam = new CFloatRenderStateParam();
        pParam->SetFloatParamType( CFloatRenderStateParam::eFSP_DepthFar );
        m_pRenderStateParams.push_back( pParam );
        m_floatRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( fDepthFar );
}

float CRenderState::GetDepthNear() const
{
    float renFloat = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthNear );
    if ( NULL == pParam )
    {
        float range[2];
        CRenderer::GetInstance()->GetFloatV(GL_DEPTH_RANGE, range );
        renFloat = range[0];
    }
    else
    {
        renFloat = pParam->GetValue();
    }
    return renFloat;
}

float CRenderState::GetDepthFar() const
{
    float renFloat = 0;
    CFloatRenderStateParam* pParam = ( CFloatRenderStateParam* )GetFloatRenderStateParamPtr( CFloatRenderStateParam::eFSP_DepthFar );
    if ( NULL == pParam )
    {
        float range[2];
        CRenderer::GetInstance()->GetFloatV(GL_DEPTH_RANGE, range );
        renFloat = range[1];
    }
    else
    {
        renFloat = pParam->GetValue();
    }
    return renFloat;
}

void CRenderState::SetDepthFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_DepthFunction );
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_DepthFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetDepthFunc() const
{
    GLint retEnum = 0;
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_DepthFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_DEPTH_FUNC, &retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}

void CRenderState::SetStencilFunc(GLenum func)
{
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_StencilFunction );
    if ( NULL == pParam )
    {
        pParam = new CFunctionRenderStateParam();
        pParam->SetType( CFunctionRenderStateParam::eFSP_StencilFunction );
        m_pRenderStateParams.push_back( pParam );
        m_funcRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( (CFunctionRenderStateParam::EFunctionType)func );
}

GLenum CRenderState::GetStencilFunc() const
{
    GLint retEnum = 0;
    CFunctionRenderStateParam* pParam = ( CFunctionRenderStateParam* )GetFuncRenderStateParamPtr( CFunctionRenderStateParam::eFSP_StencilFunction );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_FUNC, &retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}

void CRenderState::SetStencilReference(GLint nRef)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilReference );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilReference );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nRef );
}

GLint CRenderState::GetStencilReference() const
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilReference );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_REF, &retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetStencilValueMask(GLint nValueMask)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilValueMask );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_StencilValueMask );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nValueMask );
}

GLint CRenderState::GetStencilValueMask() const
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_StencilValueMask );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_VALUE_MASK, &retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetClearStencil(GLint nClearValue)
{
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_ClearStencil );
    if ( NULL == pParam )
    {
        pParam = new CIntRenderStateParam();
        pParam->SetType( CIntRenderStateParam::eISP_ClearStencil );
        m_pRenderStateParams.push_back( pParam );
        m_intRenderStateVector.push_back( pParam );
    }
    pParam->SetValue( nClearValue );
}

GLint CRenderState::GetClearStencil() const
{
    GLint retInt = 0;
    CIntRenderStateParam* pParam = ( CIntRenderStateParam* )GetIntRenderStateParamPtr( CIntRenderStateParam::eISP_ClearStencil );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_CLEAR_VALUE, &retInt );
    }
    else
    {
        retInt = pParam->GetValue();
    }
    return retInt;
}

void CRenderState::SetShadeModel(GLenum shadeModel)
{
    CShadeModeRenderStateParam* pParam = ( CShadeModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_ShadeMode );
    if ( NULL == pParam )
    {
        pParam = new CShadeModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( (CShadeModeRenderStateParam::EShadeModeType)shadeModel );
}

GLenum CRenderState::GetShadeModel() const
{
    GLint retEnum = 0;
    CShadeModeRenderStateParam* pParam = ( CShadeModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_ShadeMode );
    if ( NULL == pParam )
    {
        CRenderer::GetInstance()->GetIntegerV( GL_SHADE_MODEL, &retEnum );
    }
    else
    {
        retEnum = pParam->GetValue();
    }
    return retEnum;
}

void CRenderState::Restore()
{
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->UseProgram(m_uCurrShaderProgram);
    for ( auto iter : m_pRenderStateParams )
    {
        iter->Apply( );
    }
}

CRenderStateParamBase* CRenderState::GetRenderStateParamBasePtr( ERenderState state ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_pRenderStateParams.begin();
    for ( ; iter != m_pRenderStateParams.end(); ++iter )
    {
        if ( (*iter)->GetRenderStateType() == state )
        {
            pParam = *iter;
            break;
        }
    }
    return pParam;
}

CRenderStateParamBase* CRenderState::GetFloatRenderStateParamPtr( CFloatRenderStateParam::EFloatStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_floatRenderStateVector.begin();
    for ( ; iter != m_floatRenderStateVector.end(); ++iter )
    {
        CFloatRenderStateParam* param = (CFloatRenderStateParam*)(*iter);
        if ( param->GetFloatParamType() == type )
        {
            pParam = param;
            break;
        }
    }
    return pParam;
}

CRenderStateParamBase* CRenderState::GetFuncRenderStateParamPtr( CFunctionRenderStateParam::EFunctionStateParam type ) const
{
     CRenderStateParamBase* pParam = NULL;
     TParamStateVectorIterConst iter = m_funcRenderStateVector.begin();
     for ( ; iter != m_funcRenderStateVector.end(); ++iter )
     {
         CFunctionRenderStateParam* param = (CFunctionRenderStateParam*)(*iter);
         if ( param->GetType() == type )
         {
             pParam = param;
             break;
         }
     }
     return pParam;
}

CRenderStateParamBase* CRenderState::GetIntRenderStateParamPtr( CIntRenderStateParam::EIntStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_intRenderStateVector.begin();
    for ( ; iter != m_intRenderStateVector.end(); ++iter )
    {
        CIntRenderStateParam* param = (CIntRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return pParam;
}

CRenderStateParamBase* CRenderState::GetBoolRenderStateParamPtr( CBoolRenderStateParam::EBoolStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_boolRenderStateVector.begin();
    for ( ; iter != m_boolRenderStateVector.end(); ++iter )
    {
        CBoolRenderStateParam* param = (CBoolRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return pParam;
}

void CRenderState::SetPolygonMode( GLenum face, GLenum mode )
{
    CPolygonModeRenderStateParam* pParam = ( CPolygonModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_PolygonMode );
    if ( NULL == pParam )
    {
        pParam = new CPolygonModeRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( (CPolygonModeRenderStateParam::EPolygonModeStateParam)face,
        (CPolygonModeRenderStateParam::EPolygonModeType)mode );
}

void CRenderState::GetPolygonMode( GLenum& face, GLenum& mode ) const
{
    CPolygonModeRenderStateParam* pParam = ( CPolygonModeRenderStateParam* )GetRenderStateParamBasePtr( eRS_PolygonMode );
    if ( NULL == pParam )
    {
        GLint value[ 2 ];
        CRenderer::GetInstance()->GetIntegerV( GL_POLYGON_MODE, value );
        face = value[ 0 ];
        mode = value[ 1 ];
    }
    else
    {
        CPolygonModeRenderStateParam::EPolygonModeStateParam tempFace;
        CPolygonModeRenderStateParam::EPolygonModeType tempMode;
        pParam->GetValue( tempFace, tempMode );
        face = tempFace;
        mode = tempMode;
    }
}

bool CRenderState::operator==( const CRenderState& other ) const
{
    return ( m_bDepthMark == other.m_bDepthMark &&
        m_bEdgeFlag == other.m_bEdgeFlag &&
        m_uCurrShaderProgram == other.m_uCurrShaderProgram &&
        m_uCurrActiveTexture == other.m_uCurrActiveTexture &&
        ComperaPtrVector( m_pRenderStateParams, other.m_pRenderStateParams ) );
}

void CRenderState::SetColor( CUintRenderStateParam::EUintStateParam type, GLclampf r, GLclampf g, GLclampf b, GLclampf a )
{
    CUintRenderStateParam* pParam = ( CUintRenderStateParam* )GetUnitRenderStateParamPtr( type );
    if ( NULL == pParam )
    {
        pParam = new CUintRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
        m_unitRenderStateVector.push_back( pParam );
    }
    CColor value( r, g, b, a );
    pParam->SetValue( value );
}

CRenderStateParamBase* CRenderState::GetUnitRenderStateParamPtr( CUintRenderStateParam::EUintStateParam type ) const
{
    CRenderStateParamBase* pParam = NULL;
    TParamStateVectorIterConst iter = m_unitRenderStateVector.begin();
    for ( ; iter != m_unitRenderStateVector.end(); ++iter )
    {
        CUintRenderStateParam* param = (CUintRenderStateParam*)(*iter);
        if ( param->GetType() == type )
        {
            pParam = param;
            break;
        }
    }
    return pParam;
}

void CRenderState::GetColor( CUintRenderStateParam::EUintStateParam type , GLclampf& r, GLclampf& g, GLclampf& b, GLclampf& a )
{
    CUintRenderStateParam* pParam = ( CUintRenderStateParam* )GetUnitRenderStateParamPtr( type );
    if ( NULL == pParam )
    {
        switch ( type )
        {
        case CUintRenderStateParam::eUSP_ClearColor:
            {
                float value[4];
                glGetFloatv( GL_COLOR_CLEAR_VALUE, value );
                r = value[ 0 ];
                g = value[ 1 ];
                b = value[ 2 ];
                a = value[ 3 ];
            }
            break;
        case CUintRenderStateParam::eUSP_BlendColor:
            {
            }
            break;
        default:
            break;
        }
    }
    else
    {
        const CColor color = ( pParam->GetValue() );
        r = (GLclampf)color.r;
        g = (GLclampf)color.g;
        b = (GLclampf)color.b;
        a = (GLclampf)color.a;
    }
}

void CRenderState::SetStencilOp( GLenum fail, GLenum zFail, GLenum zPass )
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRS_StencilMode );
    if ( NULL == pParam )
    {
        pParam = new CStencilRenderStateParam();
        m_pRenderStateParams.push_back( pParam );
    }
    pParam->SetValue( ( CStencilRenderStateParam::EStencilType )fail,
        ( CStencilRenderStateParam::EStencilType )zFail,
        ( CStencilRenderStateParam::EStencilType )zPass );
}

void CRenderState::GetStencilOp( GLenum& fail, GLenum& zFail, GLenum& zPass ) const
{
    CStencilRenderStateParam* pParam = ( CStencilRenderStateParam* )GetRenderStateParamBasePtr( eRS_StencilMode );
    if ( NULL == pParam )
    {
        GLint failTemp, zFailTemp, zPassTemp;
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_FAIL, &failTemp );
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_PASS_DEPTH_FAIL, &zFailTemp );
        CRenderer::GetInstance()->GetIntegerV( GL_STENCIL_PASS_DEPTH_PASS, &zPassTemp );
        fail = failTemp;
        zFail = zFailTemp;
        zPass = zPassTemp;
    }
    else
    {
        pParam->GetValue( ( CStencilRenderStateParam::EStencilType& )fail,
            ( CStencilRenderStateParam::EStencilType& )zFail,
            ( CStencilRenderStateParam::EStencilType& )zPass );
    }
}
