#ifndef RENDER_RENDERSTATEPARAM_CLOCKWISERENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_CLOCKWISERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CClockWiseRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
public:
    enum EClockWiseType
    {
        eCWT_CCW = 0x0901, //GL_CCW
        eCWT_CW = 0x0900, //GL_CW

        eCWT_Count = 2,
        eCWT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CClockWiseRenderStateParam, 0x1019A346, CRenderStateParamBase)
public:
    CClockWiseRenderStateParam();
    CClockWiseRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
private:
    EClockWiseType m_nValue;
};
#endif