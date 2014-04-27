#include "stdafx.h"
#include "FunctionRenderStateParam.h"
#include "Render/Renderer.h"

CFunctionRenderStateParam::CFunctionRenderStateParam()
{

}

CFunctionRenderStateParam::CFunctionRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EFunctionStateParam, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EFunctionType, true, 0xFFFFFFFF, _T("比较函数"), NULL, NULL, NULL);
}

void CFunctionRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eFSP_DepthFunction:
        pRender->DepthFunc(m_nValue);
        break;
    case eFSP_StencilFunction:
        pRender->StencilFunc(m_nValue);
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CFunctionRenderStateParam"));
        break;
    }
}

ERenderState CFunctionRenderStateParam::GetRenderStateType() const
{
    return eRS_FuncMode;
}

void CFunctionRenderStateParam::SetValue( EFunctionType value )
{
    m_nValue = value;
}

CFunctionRenderStateParam::EFunctionType CFunctionRenderStateParam::GetValue() const
{
    return m_nValue;
}

void CFunctionRenderStateParam::SetType( EFunctionStateParam type )
{
    m_type = type;
}

CFunctionRenderStateParam::EFunctionStateParam CFunctionRenderStateParam::GetType() const
{
    return m_type;
}

bool CFunctionRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_FuncMode );
    CFunctionRenderStateParam* pOther = ( CFunctionRenderStateParam* )&other;
    return ( m_nValue == pOther->m_nValue && m_type == pOther->m_type );
}

