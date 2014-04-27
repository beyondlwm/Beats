#ifndef RENDER_RENDERSTATEPARAM_SHADEMODERENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_SHADEMODERENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CShadeModeRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
public:
    enum EShadeModeType
    {
        eSMT_FLAT_SHADE = 0x1D00, //GL_FLAT
        eSMT_SMOOTH_SHADE = 0x1D01, //GL_SMOOTH

        eSMT_Count = 2,
        eSMT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CShadeModeRenderStateParam, 0xF01C9BA6, CRenderStateParamBase)
public:
    CShadeModeRenderStateParam();
    CShadeModeRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    void SetValue( EShadeModeType value );
    EShadeModeType GetValue() const;
private:
    EShadeModeType m_nValue;
};
#endif