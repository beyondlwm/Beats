#include "stdafx.h"
#include "BoolRenderStateParam.h"
#include "Render/Renderer.h"

CBoolRenderStateParam::CBoolRenderStateParam()
{

}

CBoolRenderStateParam::CBoolRenderStateParam(CSerializer& serializer)
    : super(serializer)
{
    DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, EBoolStateParam, true, 0xFFFFFFFF, _T("����"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_bValue, true, 0xFFFFFFFF, _T("����ֵ"), NULL, NULL, NULL);
}

void CBoolRenderStateParam::SetType(EBoolStateParam type)
{
    m_type = type;
}

CBoolRenderStateParam::EBoolStateParam CBoolRenderStateParam::GetType() const
{
    return m_type;
}

void CBoolRenderStateParam::SetValue(bool bValue)
{
    m_bValue = bValue;
}

bool CBoolRenderStateParam::GetValue() const
{
    return m_bValue;
}

void CBoolRenderStateParam::Apply()
{
    CRenderer* pRender = CRenderer::GetInstance();
    switch (m_type)
    {
    case eBSP_EdgeFlag:
        pRender->EdgeFlag(m_bValue);
        break;
    case eBSP_WriteMaskDepth:
        pRender->DepthMask(m_bValue);
        break;
    default:
        m_bValue ? pRender->EnableGL(m_type) : pRender->DisableGL(m_type);
        break;
    }
}
