#ifndef RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

enum ERenderState
{
    eRS_StencilMode = 0,
    eRS_ShadeMode,
    eRS_CullMode,
    eRS_ClockWiseMode,
    eRS_BlendMode,
    eRS_BlendEquationMode,
    eRS_UnitMode,
    eRS_PolygonMode,
    eRS_IntMode,
    eRS_FuncMode,
    eRS_FloatMode,
    eRS_BoolMode
};

class CRenderStateParamBase : public CComponentBase
{
    typedef CComponentBase super;
public:
    DECLARE_REFLECT_GUID_ABSTRACT(CRenderStateParamBase, 0xECB09FDA, CComponentBase)
public:

    CRenderStateParamBase();
    CRenderStateParamBase(CSerializer& serializer);

    virtual ERenderState GetRenderStateType() const = 0;

    virtual void Apply() = 0;

    virtual bool operator==( const CRenderStateParamBase& other ) const = 0;
};

#endif