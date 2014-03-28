#ifndef _TESTTESTSTE
#define _TESTTESTSTE
//////////////////////////////////////////////////////////////////////////
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

enum ERenderState
{
    eRS_GL_AlphaEnable,
    eRS_GL_AlphaFunction,
    eRS_GL_AlphaReference,
    eRS_GL_BlendColor,
    eRS_GL_BlendDestAlpha,
    eRS_GL_BlendDestRGB,
    eRS_GL_BlendEnable,
    eRS_GL_BlendEquation,
    eRS_GL_BlendSourceAlpha,
    eRS_GL_BlendSourceRGB,
    eRS_GL_ClearColor,
    eRS_GL_ClearDepth,
    eRS_GL_ClearStencil,
    eRS_GL_CullMode,
    eRS_GL_CurrentColor,
    eRS_GL_DepthEnable,
    eRS_GL_DepthFar,
    eRS_GL_DepthFunction,
    eRS_GL_DepthNear,
    eRS_GL_EdgeFlag,
    eRS_GL_FogColor,
    eRS_GL_FogDensity,
    eRS_GL_FogEnd,
    eRS_GL_FogMode,
    eRS_GL_FogStart,
    eRS_GL_FronFace,
    eRS_GL_LineSmooth,
    eRS_GL_LineWidh,
    eRS_GL_PointMax,
    eRS_GL_PointMin,
    eRS_GL_PointSize,
    eRS_GL_PointSmooth,
    eRS_GL_PolyBackMode,
    eRS_GL_PolyFrontMode,
    eRS_GL_PolyOffsetFactor,
    eRS_GL_PolyOffsetFill,
    eRS_GL_PolyOffsetLine,
    eRS_GL_PolyOffsetPoint,
    eRS_GL_PolyOffsetUnits,
    eRS_GL_PolySmooth,
    eRS_GL_SecondaryColor,
    eRS_GL_ShadeModel,
    eRS_GL_StencilEnable,
    eRS_GL_StencilFail,
    eRS_GL_StencilFunction,
    eRS_GL_StencilPassDepthFail,
    eRS_GL_StencilPassDepthPass,
    eRS_GL_StencilReference,
    eRS_GL_StencilValueMask,
    eRS_GL_VertexProgramTwoSideARB,
    eRS_GL_WriteMaskColor,
    eRS_GL_WriteMaskDepth,
    eRS_GL_WriteMaskStencil,

    eRS_GL_Count,
    eRS_GL_Force32Bit = 0xFFFFFFFF
};

class CStateParam : public CComponentBase
{
    typedef CComponentBase super;
public:
    DECLARE_REFLECT_GUID(CStateParam, 0x12345678, CComponentBase)
public:
    CStateParam(){}
    CStateParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_type, 0, NULL, ERenderState, true, 0xFFFFFFFF, _T("Test"), NULL, NULL, NULL);
    }
    ERenderState m_type;
};


class CBoolParam : public CStateParam
{
    typedef CStateParam super;
public:
    DECLARE_REFLECT_GUID(CBoolParam, 0x12345a18, CStateParam)
public:
    CBoolParam(){}
    CBoolParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY(serializer, m_bValue, true, 0xFFFFFFFF, _T("布尔值"), NULL, NULL, NULL);
    }
    bool m_bValue;
};

class CUintParam : public CStateParam
{
    typedef CStateParam super;
public:
    DECLARE_REFLECT_GUID(CUintParam, 0xEA159FC1, CStateParam)
public:
    CUintParam(){}
    CUintParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY(serializer, m_uValue, true, 0xFFFFFFFF, _T("整型值"), NULL, NULL, NULL);
    }
    size_t m_uValue;
};

class CFunctionParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EFunctionType
    {
        eFT_NEVER = GL_NEVER,
        eFT_LESS = GL_LESS,
        eFT_EQUAL = GL_EQUAL,
        eFT_LESS_EQUAL = GL_LEQUAL,
        eFT_GREATER = GL_GREATER,
        eFT_NOT_EQUAL = GL_NOTEQUAL,
        eFT_GREATER_EQUAL = GL_GEQUAL,
        eFT_ALWAYS = GL_ALWAYS,

        eFT_Count = 8, 
        eFT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CFunctionParam, 0x1ACE5218, CStateParam)
public:
    CFunctionParam(){}
    CFunctionParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EFunctionType, true, 0xFFFFFFFF, _T("比较函数"), NULL, NULL, NULL);
    }
    EFunctionType m_nValue;
};

class CBlendParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EBlendParamType
    {
        eBPT_ZERO = GL_ZERO,
        eBPT_ONE = GL_ONE,
        eBPT_SRC_COLOR = GL_SRC_COLOR,
        eBPT_INV_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
        eBPT_DEST_COLOR = GL_DST_COLOR,
        eBPT_INV_DEST_COLOR = GL_ONE_MINUS_DST_COLOR,
        eBPT_SRC_ALPHA = GL_SRC_ALPHA,
        eBPT_INV_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
        eBPT_DEST_ALPHA = GL_DST_ALPHA,
        eBPT_INV_DEST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
        eBPT_CONST_COLOR = GL_CONSTANT_COLOR,
        eBPT_INV_CONST_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
        eBPT_CONST_ALPHA = GL_CONSTANT_ALPHA,
        eBPT_INV_CONST_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
        eBPT_SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,

        eBPT_Count = 15,
        eBPT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CBlendParam, 0x15CA0218, CStateParam)
public:
    CBlendParam(){}
    CBlendParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EBlendParamType, true, 0xFFFFFFFF, _T("混合因子"), NULL, NULL, NULL);
    }
    EBlendParamType m_nValue;
};

class CBlendEquationParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EBlendEquationType
    {
        eBET_ADD = GL_FUNC_ADD,
        eBET_SUBTRACT = GL_FUNC_SUBTRACT,
        eBET_REV_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
        eBET_MIN = GL_MIN,
        eBET_MAX = GL_MAX,

        eBET_Count = 5,
        eBET_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CBlendEquationParam, 0x013473F6, CStateParam)
public:
    CBlendEquationParam(){}
    CBlendEquationParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EBlendEquationType, true, 0xFFFFFFFF, _T("混合方式"), NULL, NULL, NULL);
    }
    EBlendEquationType m_nValue;
};

class CCullModeParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum ECullModeType
    {
        eCMT_NONE = GL_NONE,
        eCMT_FRONT = GL_FRONT,
        eCMT_BACK = GL_BACK,
        eCMT_FRONT_AND_BACK = GL_FRONT_AND_BACK,

        eCMT_Count = 4, 
        eCMT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CCullModeParam, 0x1A0373B6, CStateParam)
public:
    CCullModeParam(){}
    CCullModeParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, ECullModeType, true, 0xFFFFFFFF, _T("背面剔除模式"), NULL, NULL, NULL);
    }
    ECullModeType m_nValue;
};

class CClockWiseParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EClockWiseType
    {
        eCWT_CCW = GL_CCW,
        eCWT_CW = GL_CW,

        eCWT_Count = 2,
        eCWT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CClockWiseParam, 0x1019A346, CStateParam)
public:
    CClockWiseParam(){}
    CClockWiseParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EClockWiseType, true, 0xFFFFFFFF, _T("绘制顺序"), NULL, NULL, NULL);
    }
    EClockWiseType m_nValue;
};

class CPolygonModeParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EPolygonModeType
    {
        ePMT_POINTS = GL_POINT,
        ePMT_LINES = GL_LINE,
        ePMT_FILL = GL_FILL,

        ePMT_Count = 3,
        ePMT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CPolygonModeParam, 0x1AFE03B6, CStateParam)
public:
    CPolygonModeParam(){}
    CPolygonModeParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EPolygonModeType, true, 0xFFFFFFFF, _T("图元模式"), NULL, NULL, NULL);
    }
    EPolygonModeType m_nValue;
};

class CShadeModeParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EShadeModeType
    {
        eSMT_FLAT_SHADE = GL_FLAT,
        eSMT_SMOOTH_SHADE = GL_SMOOTH,

        eSMT_Count = 2,
        eSMT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CShadeModeParam, 0xF01C9BA6, CStateParam)
public:
    CShadeModeParam(){}
    CShadeModeParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EShadeModeType, true, 0xFFFFFFFF, _T("着色模式"), NULL, NULL, NULL);
    }
    EShadeModeType m_nValue;
};

class CStencilParam : public CStateParam
{
    typedef CStateParam super;
public:
    enum EStencilType
    {
        eST_ZERO = GL_ZERO,
        eST_INVERT = GL_INVERT,
        eST_KEEP = GL_KEEP,
        eST_REPLACE = GL_REPLACE,
        eST_INCR = GL_INCR,
        eST_DECR = GL_DECR,
        eST_INCR_WRAP = GL_INCR_WRAP,
        eST_DECR_WRAP = GL_DECR_WRAP,

        eST_Count = 8,
        eST_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CStencilParam, 0x11AFDA3, CStateParam)
public:
    CStencilParam(){}
    CStencilParam(CSerializer& serializer)
        : super(serializer)
    {
        DECLARE_PROPERTY_ENUM(serializer, m_nValue, 0, NULL, EStencilType, true, 0xFFFFFFFF, _T("模板参数"), NULL, NULL, NULL);
    }
    EStencilType m_nValue;
};


#endif