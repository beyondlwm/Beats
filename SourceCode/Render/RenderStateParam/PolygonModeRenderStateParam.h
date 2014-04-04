#ifndef RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_POLYGONMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CPolygonModeRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
    enum EPolygonModeStateParam
    {
        ePMSP_PolyBackMode,
        ePMSP_PolyFrontMode,

        eSMSP_Count,
        eSMSP_Force32Bit = 0xFFFFFFFF,
    };

public:
    enum EPolygonModeType
    {
        ePMT_POINTS = 0x1B00, //GL_POINT
        ePMT_LINES = 0x1B01, //GL_LINE
        ePMT_FILL = 0x1B02, //GL_FILL

        ePMT_Count = 3,
        ePMT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CPolygonModeRenderStateParam, 0x1AFE03B6, CRenderStateParamBase)
public:
    CPolygonModeRenderStateParam();
    CPolygonModeRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
private:
    EPolygonModeStateParam m_type;
    EPolygonModeType m_nValue;
};
#endif