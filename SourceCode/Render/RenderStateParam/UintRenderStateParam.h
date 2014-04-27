#ifndef RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_UINTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CUintRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
public:
    enum EUintStateParam
    {
        eUSP_BlendColor,
        eUSP_ClearColor,

        eUSP_Count = 2,
        eUSP_Force32Bit =0xFFFFFFFF
    };
    DECLARE_REFLECT_GUID(CUintRenderStateParam, 0xEA159FC1, CRenderStateParamBase)
public:
    CUintRenderStateParam();
    CUintRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    EUintStateParam GetType( ) const;
    void SetType( const EUintStateParam& type );

    const CColor& GetValue( ) const;
    void SetValue( const CColor& value );
private:
    EUintStateParam m_type;
    CColor m_uValue;
};

#endif