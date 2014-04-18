#include "stdafx.h"
#include "IntRenderStateParam.h"
#include "Render/Renderer.h"

CIntRenderStateParam::CIntRenderStateParam()
{

}
CIntRenderStateParam::CIntRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EIntStateParam, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_nValue, true, 0xFFFFFFFF, _T("符号整型值"), NULL, NULL, NULL);
}

void CIntRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eISP_ClearStencil:
        pRender->ClearStencil(m_nValue);
        break;
    case eISP_StencilReference:
        pRender->StencilReference(m_nValue);
        break;
    case eISP_StencilValueMask:
        pRender->StencilValueMask(m_nValue);
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CIntRenderStateParam"));
        break;
    }
}

ERenderState CIntRenderStateParam::GetRenderStateType() const
{
    return eRS_IntMode;
}

void CIntRenderStateParam::SetType( EIntStateParam type )
{
    m_type = type;
}

CIntRenderStateParam::EIntStateParam CIntRenderStateParam::GetType() const
{
    return m_type;
}

void CIntRenderStateParam::SetValue( int value )
{
    m_nValue = value;
}

int CIntRenderStateParam::GetValue() const
{
    return m_nValue;
}

bool CIntRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_IntMode );
    CIntRenderStateParam* pOther = ( CIntRenderStateParam* )&other;
    return ( m_type == pOther->m_type && m_nValue == pOther->m_nValue );
}
