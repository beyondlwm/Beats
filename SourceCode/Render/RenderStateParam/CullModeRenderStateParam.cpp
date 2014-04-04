#include "stdafx.h"
#include "CullModeRenderStateParam.h"
#include "Render/Renderer.h"

CCullModeRenderStateParam::CCullModeRenderStateParam()
{

}

CCullModeRenderStateParam::CCullModeRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, ECullModeType, true, 0xFFFFFFFF, _T("±³ÃæÌÞ³ýÄ£Ê½"), NULL, NULL, NULL);
}

void CCullModeRenderStateParam::Apply()
{
    CRenderer::GetInstance()->CullFace(m_nValue);
}
