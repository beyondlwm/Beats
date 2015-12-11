#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTREFERENCE_H__INCLUDE

#include "ComponentProxy.h"
#include "Utility/RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_utils.hpp"

class CComponentReference : public CComponentProxy
{
    typedef CComponentProxy super;
public:
    CComponentReference(uint32_t uProxyId, uint32_t uProxyGuid, CComponentGraphic* pGraphics);
    CComponentReference(CComponentProxy* pComponentHost);
    virtual ~CComponentReference();

    CComponentProxy* GetHostProxy();
    virtual uint32_t GetProxyId();
    virtual uint32_t GetGuid() const override;
    virtual uint32_t GetParentGuid() const override;
    virtual const TCHAR* GetClassStr() const override;
    virtual void Save() override;
    virtual void SaveToXML(rapidxml::xml_node<>* pNode, bool bSaveOnlyNoneNativePart = false) override;
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode) override;

    virtual void Initialize() override;
    virtual void Uninitialize() override;

private:
    uint32_t m_uReferenceId;
    uint32_t m_uReferenceGuid;
    CComponentProxy* m_pHostProxy;
};

#endif