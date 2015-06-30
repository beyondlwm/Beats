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
    virtual void Initialize() override;
    virtual void Uninitialize() override;

    void SetDataPos(uint32_t uDataPos);
    uint32_t GetDataPos() const;

    void SetDataSize(uint32_t uDataSize);
    uint32_t GetDataSize() const;

public:
    void SetProxyComponent(CComponentProxy* pProxy);
    class CComponentProxy* GetProxyComponent() const;

    virtual void SetSyncProxyComponent(CComponentProxy* pProxy);

    void Serialize(CSerializer& serializer);
    CComponentBase* CloneInstance();

private:
    uint32_t m_uDataPos;
    uint32_t m_uDataSize;
    CComponentProxy* m_pProxyComponent;
    // This member only for: when this instance is deleted, it must tell the sync proxy to unregister itself.
    CComponentProxy* m_pSyncProxyComponent;
};

#endif