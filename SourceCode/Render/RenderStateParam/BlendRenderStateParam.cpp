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

