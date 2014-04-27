#include "stdafx.h"
#include "BlendEquationRenderStateParam.h"
#include "Render/Renderer.h"

CBlendEquationRenderStateParam::CBlendEquationRenderStateParam()
{

}

CBlendEquationRenderStateParam::CBlendEquationRenderStateParam(CSerializer& serializer)
: super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EBlendEquationType, true, 0xFFFFFFFF, _T("»ìºÏ·½Ê½"), NULL, NULL, NULL);
}

void CBlendEquationRenderStateParam::Apply()
{
    CRenderer::GetInstance()->BlendEquation(m_nValue);
}

ERenderState CBlendEquationRenderStateParam::GetRenderStateType() const
{
    return eRS_BlendEquationMode;
}

void CBlendEquationRenderStateParam::SetBlendEquationType( EBlendEquationType type )
{
    m_nValue = type;
}

CBlendEquationRenderStateParam::EBlendEquationType CBlendEquationRenderStateParam::GetBlendEquationType() const
{
    return m_nValue;
}

bool CBlendEquationRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_BlendEquationMode );
    CBlendEquationRenderStateParam* pOther = (CBlendEquationRenderStateParam*)&other;
    return m_nValue == pOther->m_nValue;
}
