#ifndef RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE
#define RENDER_RENDERSTATEPARAM_RENDERSTATEPARAMBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

class CRenderStateParamBase : public CComponentBase
{
    typedef CComponentBase super;
public:
    DECLARE_REFLECT_GUID_ABSTRACT(CRenderStateParamBase, 0xECB09FDA, CComponentBase)
public:

    CRenderStateParamBase();
    CRenderStateParamBase(CSerializer& serializer);

    virtual void Apply() = 0;
};

#endif