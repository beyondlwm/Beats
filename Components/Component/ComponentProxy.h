#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE

#include "../expdef.h"

#include "Property/PropertyPublic.h"
#include "ComponentBase.h"

class CComponentGraphic;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CPropertyDescriptionBase;
class CComponentGraphic;
class CComponentInstance;

class COMPONENTS_DLL_DECL CComponentProxy : public CComponentBase
{
    typedef CComponentBase super;
    friend class CComponentInstance;
public:
    CComponentProxy();//Only ComponentReference should use this constructor.
    CComponentProxy(CComponentGraphic* pGraphics);
    CComponentProxy(CComponentGraphic* pGraphics, uint32_t uGuid, uint32_t uParentGuid, const TCHAR* pszClassName);
    virtual ~CComponentProxy();

    const TString& GetDisplayName() const;
    void SetDisplayName(const TCHAR* pDisplayName);
    const TString& GetCatalogName() const;
    void SetCatalogName(const TCHAR* pCatalogName);
    const TString& GetUserDefineDisplayName() const;
    void SetUserDefineDisplayName(const TCHAR* pszUserDefineDisplayName);
    CComponentGraphic* GetGraphics();
    void SetGraphics(CComponentGraphic* pGraphics);

    void SetHostComponent(CComponentInstance* pComponent);
    CComponentInstance* GetHostComponent() const;
    void UpdateHostComponent();

    void SetTemplateFlag(bool bFlag);
    bool GetTemplateFlag() const;

    void AddProperty(CPropertyDescriptionBase* pProperty);
    void ClearProperty();
    const std::vector<CPropertyDescriptionBase*>* GetPropertyPool() const;
    CPropertyDescriptionBase* GetPropertyDescription(const TCHAR* pszVariableName) const;

    CDependencyDescription* GetDependency(uint32_t uIndex);
    const std::vector<CDependencyDescription*>* GetDependencies();
    void AddDependencyDescription(CDependencyDescription* pDependencyDesc);
    void AddBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    void RemoveBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    const std::vector<CDependencyDescriptionLine*>* GetBeConnectedDependencyLines();

    //Simulate the real component.
    static const uint32_t REFLECT_GUID = 1;
    static const uint32_t PARENT_REFLECT_GUID = super::REFLECT_GUID;
    virtual uint32_t GetGuid() const;
    virtual uint32_t GetParentGuid() const override;
    virtual const TCHAR* GetClassStr() const;
    virtual void Save();
    virtual uint32_t GetProxyId();
    virtual CComponentBase* Clone(bool bCloneValue, CSerializer* pSerializer, uint32_t id, bool bCallInitFunc = true) override;

    ///*
    //    We have two ways for interaction:
    //    1. ExportDataToHost: Export all value from proxy, which can be read by CComponentInstance.
    //       ImportDataFromEDS: Read the basic info to create CPropertyDescriptionBase and CDependencyDescription. which comes from REGISTER_COMPONENT.
    //    2. SaveToXML && LoadFromXML, this is for save and load data for editing.
    //*/

    virtual void SaveToXML(rapidxml::xml_node<>* pNode, bool bSaveOnlyNoneNativePart = false);
    virtual void LoadFromXML(rapidxml::xml_node<>* pNode);
    virtual void ExportDataToHost(CSerializer& serializer, EValueType eValueType);
    virtual void ImportDataFromEDS(CSerializer& serializer);

    virtual void Initialize() override;
    virtual void Uninitialize() override;
    const std::vector<CComponentInstance*>& GetSyncComponents() const;

private:
    void AddSyncComponent(CComponentInstance* pInstance);
    void RemoveSyncComponent(CComponentInstance* pInstance);

protected:
    std::vector<CPropertyDescriptionBase*>* m_pProperties;
    std::vector<CDependencyDescription*>* m_pDependenciesDescription;
    std::vector<CDependencyDescriptionLine*>* m_pBeConnectedDependencyLines;
    CComponentInstance* m_pHostComponent;
    std::vector<CComponentInstance*> m_syncComponents; // these instance will be synced as m_pHostComponent, usually for prototype stuff.

private:
    bool m_bIsTemplate;
    uint32_t m_uGuid;
    uint32_t m_uParentGuid;
    CComponentGraphic* m_pGraphics;
    std::vector<char>* m_pSerializeOrder; // this variable save the order of property/dependency data comes in when deserialize. we have to keep the order when serialize.
    TString m_strClassName;
    TString m_strDisplayName;
    TString m_strUserDefineDisplayName;
    TString m_strCatalogName;
};

#endif