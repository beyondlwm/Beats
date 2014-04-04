#include "stdafx.h"
#include "FloatRenderStateParam.h"
#include "Render/Renderer.h"
#include "Render/RenderState.h"

CFloatRenderStateParam::CFloatRenderStateParam()
{

}
CFloatRenderStateParam::CFloatRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EFloatStateParam, true, 0xFFFFFFFF, _T("类型"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_fValue, true, 0xFFFFFFFF, _T("浮点数值"), NULL, NULL, NULL);
}

void CFloatRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eFSP_ClearDepth:
        pRender->ClearDepth(m_fValue);
        break;
    case eFSP_DepthFar:
        pRender->DepthRange(pRender->GetCurrentState()->GetDepthNear(), m_fValue);
        break;
    case eFSP_DepthNear:
        pRender->DepthRange(m_fValue, pRender->GetCurrentState()->GetDepthFar());
        break;
    case eFSP_LineWidh:
        pRender->LineWidth(m_fValue);
        break;
    default:
        BEATS_ASSERT(false, _T("Unkonwn type of CFloatRenderStateParam"));
        break;
    }
}
