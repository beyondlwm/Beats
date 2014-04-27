#include "stdafx.h"
#include "BlendRenderStateParam.h"
#include "Render/Renderer.h"

CBlendRenderStateParam::CBlendRenderStateParam()
{

}

CBlendRenderStateParam::CBlendRenderStateParam(CSerializer& serializer)
: super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_source, 0, NULL, EBlendParamType, true, 0xFFFFFFFF, _T("Ô´"), NULL, NULL, NULL);
    DECLARE_PROPERTY_ENUM(serializer, m_dest, 0, NULL, EBlendParamType, true, 0xFFFFFFFF, _T("Ä¿±ê"), NULL, NULL, NULL);
}

void CBlendRenderStateParam::Apply()
{
    CRenderer::GetInstance()->BlendFunc(m_source, m_dest);
}

ERenderState CBlendRenderStateParam::GetRenderStateType() const
{
    return eRS_BlendMode;
}

void CBlendRenderStateParam::SetSrcFactor( EBlendParamType src )
{
    m_source = src;
}

void CBlendRenderStateParam::SetTargetFactor( EBlendParamType des )
{
    m_dest = des;
}

CBlendRenderStateParam::EBlendParamType CBlendRenderStateParam::GetSrcFactor() const
{
    return m_source;
}

CBlendRenderStateParam::EBlendParamType CBlendRenderStateParam::GetTargetFactor() const
{
    return m_dest;
}

bool CBlendRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BlendMode );
    CBlendRenderStateParam* pOther = (CBlendRenderStateParam*)&other;
    return ( m_source == pOther->m_source && m_dest == pOther->m_dest );
}

