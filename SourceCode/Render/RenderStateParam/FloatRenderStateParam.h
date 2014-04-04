#ifndef RENDER_RENDERSTATEPARAM_FLOATRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_FLOATRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CFloatRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;

    enum EFloatStateParam
    {
        eFSP_ClearDepth,
        eFSP_DepthFar,
        eFSP_DepthNear,
        eFSP_LineWidh,

        eFSP_Count = 6,
        eFSP_Force32Bit = 0xFFFFFFFF
    };

public:
    DECLARE_REFLECT_GUID(CFloatRenderStateParam, 0x3B059AC8, CRenderStateParamBase)
public:
    CFloatRenderStateParam();
    CFloatRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
private:
    EFloatStateParam m_type;
    float m_fValue;
};

#endif