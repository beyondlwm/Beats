#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE

#include "../expdef.h"
#include "ComponentBase.h"

class COMPONENTS_DLL_DECL CComponentInstance : public CComponentBase
{
    DECLARE_REFLECT_GUID(CComponentInstance, 0x4314AEF, CComponentBase)
public:
    CComponentInstance();
    virtual ~CComponentInstance();
    virtual void Uninitialize() override;

public:
    void SetProxyComponent(class CComponentProxy* pProxy);
    class CComponentProxy* GetProxyComponent() const;
    void Serialize(CSerializer& serializer);
    CComponentBase* CloneInstance();

private:
    class CComponentProxy* m_pProxyComponent;
};

#endif