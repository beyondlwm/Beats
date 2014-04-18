#include "stdafx.h"
#include "ShadeModeRenderStateParam.h"
#include "Render/Renderer.h"

CShadeModeRenderStateParam::CShadeModeRenderStateParam()
{

}

CShadeModeRenderStateParam::CShadeModeRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EShadeModeType, true, 0xFFFFFFFF, _T("着色模式"), NULL, NULL, NULL);
}

void CShadeModeRenderStateParam::Apply()
{
    CRenderer::GetInstance()->ShadeModel(m_nValue);
}

ERenderState CShadeModeRenderStateParam::GetRenderStateType() const
{
    return eRS_ShadeMode;
}

void CShadeModeRenderStateParam::SetValue( EShadeModeType value )
{
    m_nValue = value;
}

CShadeModeRenderStateParam::EShadeModeType CShadeModeRenderStateParam::GetValue() const
{
    return m_nValue;
}

bool CShadeModeRenderStateParam::operator==( const CRenderStateParamBase& other ) const
{
    BEATS_ASSERT( other.GetRenderStateType() == eRS_ShadeMode );
    CShadeModeRenderStateParam* pOther = ( CShadeModeRenderStateParam* )&other;
    return m_nValue == pOther->m_nValue;
}
