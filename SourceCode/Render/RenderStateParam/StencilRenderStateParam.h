#ifndef RENDER_RENDERSTATEPARAM_STENCILRENDERSTATEPARAM_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_STENCILRENDERSTATEPARAM_H__INCLUDE
#include "RenderStateParamBase.h"


class CStencilRenderStateParam : public CRenderStateParamBase
{
    typedef CRenderStateParamBase super;

public:
    enum EStencilType
    {
        eST_ZERO = 0, //GL_ZERO
        eST_INVERT = 0x150A, //GL_INVERT
        eST_KEEP = 0x1E00, //GL_KEEP
        eST_REPLACE = 0x1E01, //GL_REPLACE
        eST_INCR = 0x1E02, //GL_INCR
        eST_DECR = 0x1E03, //GL_DECR
        eST_INCR_WRAP = 0x8507, //GL_INCR_WRAP
        eST_DECR_WRAP = 0x8508, //GL_DECR_WRAP

        eST_Count = 8,
        eST_Force32Bit = 0xFFFFFFFF
    };

    DECLARE_REFLECT_GUID(CStencilRenderStateParam, 0x11AFDA3, CComponentBase)
public:
    CStencilRenderStateParam();
    CStencilRenderStateParam(CSerializer& serializer);
    virtual void Apply() override;
    virtual ERenderState GetRenderStateType() const;

    void SetValue( EStencilType fail, EStencilType zFali, EStencilType zPass );

    void GetValue( EStencilType& fail, EStencilType& zFali, EStencilType& zPass ) const;

    virtual bool operator==( const CRenderStateParamBase& other ) const;
private:
    EStencilType m_nFail;
    EStencilType m_nZFail;
    EStencilType m_nZPass;
};

#endif