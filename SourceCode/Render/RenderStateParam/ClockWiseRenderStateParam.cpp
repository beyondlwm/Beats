#include "stdafx.h"
#include "ClockWiseRenderStateParam.h"
#include "Render/Renderer.h"

CClockWiseRenderStateParam::CClockWiseRenderStateParam()
{

}

CClockWiseRenderStateParam::CClockWiseRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EClockWiseType, true, 0xFFFFFFFF, _T("»æÖÆË³Ðò"), NULL, NULL, NULL);
}

void CClockWiseRenderStateParam::Apply()
{
    CRenderer::GetInstance()->FrontFace(m_nValue);
}

ERenderState CClockWiseRenderStateParam::GetRenderStateType() const
{
    return eRS_ClockWiseMode;
}

void CClockWiseRenderStateParam::SetClockWiseType( EClockWiseType type )
{
    m_nValue = type;
}

CClockWiseRenderStateParam::EClockWiseType CClockWiseRenderStateParam::GetClockWiseType() const
{
    return m_nValue;
}

bool CClockWiseRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ClockWiseMode );
    CClockWiseRenderStateParam* pOther = (CClockWiseRenderStateParam*)&other;
    return m_nValue == pOther->m_nValue;
}
