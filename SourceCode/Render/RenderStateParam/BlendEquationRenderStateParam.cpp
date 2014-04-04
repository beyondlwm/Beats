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
