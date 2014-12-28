#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTINSTANCE_H__INCLUDE

#include "../expdef.h"
#include "ComponentBase.h"

class CComponentProxy;

class COMPONENTS_DLL_DECL CComponentInstance : public CComponentBase
{
    DECLARE_REFLECT_GUID(CComponentInstance, 0x4314AEF, CComponentBase)
public:
    CComponentInstance();
    virtual ~CComponentInstance();
    virtual void Uninitialize() override;

    void SetDataPos(size_t uDataPos);
    size_t GetDataPos() const;

    void SetDataSize(size_t uDataSize);
    size_t GetDataSize() const;

public:
    void SetProxyComponent(CComponentProxy* pProxy);
    class CComponentProxy* GetProxyComponent() const;

    virtual void SetSyncProxyComponent(CComponentProxy* pProxy);

    void Serialize(CSerializer& serializer);
    CComponentBase* CloneInstance();

private:
    size_t m_uDataPos;
    size_t m_uDataSize;
    CComponentProxy* m_pProxyComponent;
    // This member only for: when this instance is deleted, it must tell the sync proxy to unregister itself.
    CComponentProxy* m_pSyncProxyComponent;
};

#endif