#ifndef RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"


class CUintRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
    enum EUintStateParam
    {
        eUSP_BlendColor,
        eUSP_ClearColor,

        eUSP_Count = 2,
        eUSP_Force32Bit =0xFFFFFFFF
    };

public:
    DECLARE_REFLECT_GUID(CUintRenderStateParam, 0xEA159FC1, CRenderStateParamBase)
public:
    CUintRenderStateParam();
    CUintRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
private:
    EUintStateParam m_type;
    size_t m_uValue;
};

#endif