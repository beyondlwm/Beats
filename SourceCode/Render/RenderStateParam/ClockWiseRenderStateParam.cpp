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
