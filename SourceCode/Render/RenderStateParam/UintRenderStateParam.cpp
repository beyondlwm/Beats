#include "stdafx.h"
#include "UintRenderStateParam.h"
#include "Render/Renderer.h"

CUintRenderStateParam::CUintRenderStateParam()
{

}
CUintRenderStateParam::CUintRenderStateParam(CSerializer& serializer)
: super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EUintStateParam, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_uValue, true, 0xFFFFFFFF, _T("整型值"), NULL, NULL, NULL);
}

void CUintRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    CColor colorTmp(m_uValue);

    switch (m_type)
    {
    case eUSP_BlendColor:
        pRender->BlendColor((float)colorTmp.r / 0xFF, (float)colorTmp.g / 0xFF, (float)colorTmp.b / 0xFF, (float)colorTmp.a / 0xFF);
        break;
    case eUSP_ClearColor:
        pRender->ClearColor((float)colorTmp.r / 0xFF, (float)colorTmp.g / 0xFF, (float)colorTmp.b / 0xFF, (float)colorTmp.a / 0xFF);
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CUintRenderStateParam"));
        break;
    }
}


