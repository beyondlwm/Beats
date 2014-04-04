#ifndef RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"



class CIntRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
    enum EIntStateParam
    {
        eISP_ClearStencil,
        eISP_StencilReference,
        eISP_StencilValueMask,

        eISP_Count = 3,
        eISP_Force32Bit = 0xFFFFFFFF
    };
public:
    DECLARE_REFLECT_GUID(CIntRenderStateParam, 0x70C59FDD, CRenderStateParamBase)
public:
    CIntRenderStateParam();
    CIntRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
private:
    EIntStateParam m_type;
    int m_nValue;
};
#endif