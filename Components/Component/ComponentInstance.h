#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE

#include "../expdef.h"
#include "ComponentBase.h"

class COMPONENTS_DLL_DECL CComponentInstance : public CComponentBase
{
    typedef CComponentBase super;
public:
    CComponentInstance();
    virtual ~CComponentInstance();
    virtual void Initialize() override;
    virtual void Uninitialize() override;
    DECLARE_REFLECT_GUID(CComponentInstance, 0x4314AEF, CComponentBase)

};

#endif