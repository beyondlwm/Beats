#ifndef RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_INTRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"



class CIntRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
public:
    enum EIntStateParam
    {
        eISP_ClearStencil,
        eISP_StencilReference,
        eISP_StencilValueMask,

        eISP_Count = 3,
        eISP_Force32Bit = 0xFFFFFFFF
    };
    DECLARE_REFLECT_GUID(CIntRenderStateParam, 0x70C59FDD, CRenderStateParamBase)
public:
    CIntRenderStateParam();
    CIntRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    void SetType( EIntStateParam type );
    EIntStateParam GetType() const;

    void SetValue( int value );
    int GetValue() const;
private:
    EIntStateParam m_type;
    int m_nValue;
};
#endif