#include "stdafx.h"
#include "PolygonModeRenderStateParam.h"
#include "Render/Renderer.h"

CPolygonModeRenderStateParam::CPolygonModeRenderStateParam()
{

}

CPolygonModeRenderStateParam::CPolygonModeRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EPolygonModeStateParam, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EPolygonModeType, true, 0xFFFFFFFF, _T("图元模式"), NULL, NULL, NULL);
}

void CPolygonModeRenderStateParam::Apply()
{
    CRenderer::GetInstance()->PolygonMode(m_type, m_nValue);
}

ERenderState CPolygonModeRenderStateParam::GetRenderStateType() const
{
    return eRS_PolygonMode;
}

void CPolygonModeRenderStateParam::SetValue( EPolygonModeStateParam type, EPolygonModeType value )
{
    m_type = type;
    m_nValue = value;
}

void CPolygonModeRenderStateParam::GetValue( EPolygonModeStateParam& type, EPolygonModeType& value ) const
{
    type = m_type;
    value = m_nValue;
}

bool CPolygonModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_PolygonMode );
    CPolygonModeRenderStateParam* pOther = ( CPolygonModeRenderStateParam* )&other;
    return ( m_type == pOther->m_type && m_nValue == pOther->m_nValue );
}
