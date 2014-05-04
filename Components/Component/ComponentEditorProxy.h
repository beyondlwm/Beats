#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXY_H__INCLUDE

#include "ComponentBase.h"
#include "../expdef.h"
class CComponentGraphic;
class TiXmlElement;
class CDependencyDescription;
class CDependencyDescriptionLine;
class CPropertyDescriptionBase;
class CComponentGraphic;

class COMPONENTS_DLL_DECL CComponentEditorProxy : public CComponentBase
{
    typedef CComponentBase super;
public:
    CComponentEditorProxy(CComponentGraphic* pGraphics);
    CComponentEditorProxy(CComponentGraphic* pGraphics, size_t uGuid, size_t uParentGuid, const TCHAR* pszClassName);
    virtual ~CComponentEditorProxy();

    const TString& GetDisplayName() const;
    void SetDisplayName(const TCHAR* pDisplayName);
    const TString& GetCatalogName() const;
    void SetCatalogName(const TCHAR* pCatalogName);
    CComponentGraphic* GetGraphics();

    void SetHostComponent(CComponentBase* pComponent);
    CComponentBase* GetHostComponent() const;
    void UpdateHostComponent();

    void AddProperty(CPropertyDescriptionBase* pProperty);
    void ClearProperty();
    const std::vector<CPropertyDescriptionBase*>* GetPropertyPool() const;

    CDependencyDescription* GetDependency(size_t uIndex);
    const std::vector<CDependencyDescription*>& GetDependencies();
    void AddDependencyDescription(CDependencyDescription* pDependencyDesc);
    void AddBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    void RemoveBeConnectedDependencyDescriptionLine(CDependencyDescriptionLine* pDependencyDescLine);
    const std::vector<CDependencyDescriptionLine*>& GetBeConnectedDependencyLines();

    //Simulate the real component.
    static const size_t REFLECT_GUID = 1;
    static const size_t PARENT_REFLECT_GUID = super::REFLECT_GUID;
    virtual size_t GetGuid() const;
    virtual size_t GetParentGuid();
    virtual const TCHAR* GetClassStr() const;
    void Save();

    virtual CComponentBase* Clone(bool bCloneValue, CSerializer* pSerializer);

    ///*
    //    We have two ways for interaction:
    //    1. Serialize && Deserialize, this is for binarize all data which comes from editor, for program running.
    //    2. SaveToXML && LoadFromXML, this is for save and load data for editing.
    //*/

    virtual void SaveToXML(TiXmlElement* pNode, bool bSaveOnlyNoneNativePart = false);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void Serialize(CSerializer& serializer);
    virtual void Deserialize(CSerializer& serializer);

    virtual void SetId(size_t id);
    virtual void Initialize();

protected:
    std::vector<CPropertyDescriptionBase*>* m_pProperties;
    std::vector<CDependencyDescription*>* m_pDependenciesDescription;
    std::vector<CDependencyDescriptionLine*>* m_pBeConnectedDependencyLines;

private:
    size_t m_uGuid;
    size_t m_uParentGuid;
    CComponentGraphic* m_pGraphics;
    CComponentBase* m_pHostComponent;
    std::vector<char>* m_pSerializeOrder; // this variable save the order of property/dependency data comes in when deserialize. we have to keep the order when serialize.
    TString m_szClassName;
    TString m_displayName;
    TString m_catalogName;
};

#endif