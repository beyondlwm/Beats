#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE

#include "ComponentManagerBase.h"

class CComponentProxy;
class CComponentProject;
class CPropertyDescriptionBase;
class CComponentGraphic;

typedef CComponentProxy* (*TCreateComponentEditorProxyFunc)(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* className);
typedef CComponentGraphic* (*TCreateGraphicFunc)();

class CComponentProxyManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentProxyManager);
    typedef CPropertyDescriptionBase* (*TCreatePropertyFunc)(CSerializer* serializer);
public:
    // param bOpenAsCopy means we will create a new file from template file.
    void OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy = false);
    void CloseFile(bool bRefreshProjectData = true);
    const TString& GetCurrentWorkingFilePath() const;

    void Export(const std::vector<TString>& fileList, const TCHAR* pSavePath);

    void QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse);
    void RegisterClassInheritInfo(size_t uDerivedClassGuid, size_t uBaseClassGuid);
    TString QueryComponentName(size_t uGuid) const;

    CComponentProject* GetProject() const;

    void SaveTemplate(const TCHAR* pszFilePath);
    void SaveToFile(const TCHAR* pFileName = NULL);

    void RegisterPropertyCreator(size_t enumType, TCreatePropertyFunc func);
    CPropertyDescriptionBase* CreateProperty(size_t propertyType, CSerializer* serializer);

    void DeserializeTemplateData(const TCHAR* pWorkingPath, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);
    virtual void ResolveDependency() override;

    CPropertyDescriptionBase* GetCurrReflectDescription() const;
    void SetCurrReflectDescription(CPropertyDescriptionBase* pPropertyDescription);
    void SetReflectCheckFlag(bool bFlag);
    bool GetReflectCheckFlag() const;

    CDependencyDescription* GetCurrReflectDependency() const;
    void SetCurrReflectDependency(CDependencyDescription* pDependency);

    const std::map<size_t, TString>& GetAbstractComponentNameMap() const;
    virtual void Uninitialize() override;

private:
    void LoadTemplateDataFromXML(const TCHAR* pWorkingPath);
    void LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);

private:
    bool m_bReflectCheckFlag;
    CComponentProject* m_pProject;
    TString m_currentWorkingFilePath;
    CPropertyDescriptionBase* m_pCurrReflectPropertyDescription;
    CDependencyDescription* m_pCurrReflectDependency;
    std::map<size_t, TCreatePropertyFunc>* m_pPropertyCreatorMap;
    std::map<size_t, TString> m_abstractComponentNameMap;
    // This map save the inherit relationship for all components. so when we instance a component pointer, we can decide which instance to generate.
    std::map<size_t, std::vector<size_t> >* m_pComponentInheritMap;
};



#endif