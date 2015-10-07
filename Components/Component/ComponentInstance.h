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
    virtual bool Load() override;
    virtual bool Unload() override;

    void SetDataPos(uint32_t uDataPos);
    uint32_t GetDataPos() const;

    void SetDataSize(uint32_t uDataSize);
    uint32_t GetDataSize() const;

    void SetProxyComponent(CComponentProxy* pProxy);
    class CComponentProxy* GetProxyComponent() const;

    virtual void SetSyncProxyComponent(CComponentProxy* pProxy);

    void Serialize(CSerializer& serializer);
    CComponentBase* CloneInstance();

    void SetReflectOwner(CComponentInstance* pReflectOwner);
    CComponentInstance* GetReflectOwner() const;
    const std::set<CComponentInstance*>& GetReflectComponents() const;
    void RegisterReflectComponent(CComponentInstance* pComponent);
    void UnregisterReflectComponent(CComponentInstance* pComponent);

private:
    uint32_t m_uDataPos;
    uint32_t m_uDataSize;
    CComponentInstance* m_pReflectComponentOwner; // TODO: this is only for avoid delete reflect components manually. maybe it is not necessary.
    std::set<CComponentInstance*> m_reflectComponents;
    CComponentProxy* m_pProxyComponent;
    // This member only for: when this instance is deleted, it must tell the sync proxy to unregister itself.
    CComponentProxy* m_pSyncProxyComponent;
};

#endif