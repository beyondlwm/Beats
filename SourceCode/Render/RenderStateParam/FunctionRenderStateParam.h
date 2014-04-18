#ifndef RENDER_RENDERSTATEPARAM_FUNCTIONRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_FUNCTIONRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"

class CFunctionRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;
public:
    enum EFunctionStateParam
    {
        eFSP_DepthFunction,
        eFSP_StencilFunction,

        eFSP_Count,
        eFSP_Force32Bit = 0xFFFFFFFF,
    };

    enum EFunctionType
    {
        eFT_NEVER = 0x0200, //GL_NEVER
        eFT_LESS = 0x0201, //GL_LESS
        eFT_EQUAL = 0x0202, //GL_EQUAL
        eFT_LESS_EQUAL = 0x0203, //GL_LEQUAL
        eFT_GREATER = 0x0204, //GL_GREATER
        eFT_NOT_EQUAL = 0x0205, //GL_NOTEQUAL
        eFT_GREATER_EQUAL = 0x0206, //GL_GEQUAL
        eFT_ALWAYS = 0x0207, //GL_ALWAYS

        eFT_Count = 8, 
        eFT_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CFunctionRenderStateParam, 0x67CE5218, CRenderStateParamBase)
public:
    CFunctionRenderStateParam();
    CFunctionRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;

    virtual ERenderState GetRenderStateType() const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;

    void SetType( EFunctionStateParam type );
    EFunctionStateParam GetType() const;

    void SetValue( EFunctionType value );
    EFunctionType GetValue() const;

private:
    EFunctionStateParam m_type;
    EFunctionType m_nValue;
};

#endif