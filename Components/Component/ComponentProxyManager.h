#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTPROXYMANAGER_H__INCLUDE

#include "ComponentManagerBase.h"

class CComponentProxy;
class CPropertyDescriptionBase;
class CComponentGraphic;
class CComponentReference;
class CComponentProjectDirectory;

typedef CComponentProxy* (*TCreateComponentEditorProxyFunc)(CComponentGraphic* pGraphics, uint32_t guid, uint32_t parentGuid, TCHAR* className);
typedef CComponentGraphic* (*TCreateGraphicFunc)();

enum class EReflectOperationType
{
    ChangeValue,
    AddChild,
    RemoveChild,
    ChangeListOrder
};

class CComponentProxyManager : public CComponentManagerBase
{
    typedef CComponentManagerBase super;
    BEATS_DECLARE_SINGLETON(CComponentProxyManager);
    typedef CPropertyDescriptionBase* (*TCreatePropertyFunc)(CSerializer* serializer);
public:
    void InitializeAllTemplate();
    void UninitializeAllTemplate();

    // param bCloseLoadedFile means if current working file include pFilePath, shall we close those loaded sub file.
    void OpenFile(const TCHAR* pFilePath, bool bCloseLoadedFile = false);
    virtual void LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pLoadComponents) override;
    virtual void UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents) override;
    void LoadFileFromDirectory(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>* pComponentContainer);
    virtual void CloseFile(uint32_t uFileId) override;
    const uint32_t GetCurrentViewFileId() const;
    void RebuildCurrSceneComponents(uint32_t uCurViewFileId);

    void Export(const TCHAR* pSavePath, std::function<void(CComponentProxy*)> exportCallback = nullptr);

    void QueryDerivedClass(uint32_t uBaseClassGuid, std::vector<uint32_t>& result, bool bRecurse) const;
    uint32_t QueryBaseClase(uint32_t uGuid) const;
    void RegisterClassInheritInfo(uint32_t uDerivedClassGuid, uint32_t uBaseClassGuid);
    TString QueryComponentName(uint32_t uGuid) const;

    void SaveTemplate(const TCHAR* pszFilePath);
    void SaveCurFile();
    void SaveToFile(const TCHAR* pszFileName, std::map<uint32_t, std::vector<CComponentProxy*>>& components);

    void RegisterPropertyCreator(uint32_t enumType, TCreatePropertyFunc func);
    CPropertyDescriptionBase* CreateProperty(uint32_t propertyType, CSerializer* serializer);

    void DeserializeTemplateData(const TCHAR* pszPath,
                                 const TCHAR* pszEDSFileName,
                                 const TCHAR* pszPatchXMLFileName,
                                TCreateComponentEditorProxyFunc func,
                                TCreateGraphicFunc pGraphicFunc);
    virtual void ResolveDependency() override;

    CPropertyDescriptionBase* GetCurrReflectProperty(EReflectOperationType* pOperateType = nullptr) const;
    void SetCurrReflectProperty(CPropertyDescriptionBase* pPropertyDescription, EReflectOperationType operateType);

    CDependencyDescription* GetCurrReflectDependency() const;
    void SetCurrReflectDependency(CDependencyDescription* pDependency);
    CSerializer& GetRemoveChildInfo();

    CComponentProxy* GetCurrUpdateProxy() const;
    void SetCurrUpdateProxy(CComponentProxy* pProxy);

    const std::map<uint32_t, TString>& GetAbstractComponentNameMap() const;

    bool IsParent(uint32_t uParentGuid, uint32_t uChildGuid) const;

    void RegisterComponentReference(CComponentReference* pReference);
    void UnregisterComponentReference(CComponentReference* pReference);
    const std::map<uint32_t, std::vector<CComponentReference*>>& GetReferenceIdMap() const;
    const std::map<uint32_t, CComponentReference*>& GetReferenceMap() const;

    CComponentReference* CreateReference(uint32_t uProxyId, uint32_t uReferenceGuid, uint32_t uId = 0xFFFFFFFF);

    std::map<uint32_t, CComponentProxy*>& GetComponentsInCurScene();
    void OnCreateComponentInScene(CComponentProxy* pProxy);
    void OnDeleteComponentInScene(CComponentProxy* pProxy);

    bool IsExporting() const;

    // Useful for exporting, at that phase, no need to create any component instance.
    bool IsEnableCreateInstanceWithProxy() const;
    void SetEnableCreateInstanceWithProxy(bool bFlag);

    std::set<uint32_t>& GetRefreshFileList();
    uint32_t GetOperateProgress(TString& strCurrOperateFile) const;

    void CheckForUnInvokedGuid(std::set<uint32_t>& uninvokeGuidList);

private:
    void LoadTemplateDataFromXML(const TCHAR* pWorkingPath);
    void LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);
    void ReSaveFreshFile();
    bool ExportComponentProxy(CComponentProxy* pProxy, CSerializer& serializer, std::function<void(CComponentProxy*)> exportCallback);

private:
    bool m_bCreateInstanceWithProxy;
    bool m_bExportingPhase;
    uint32_t m_uOperateProgress;
    TString m_strCurrOperateFile;
    uint32_t m_uCurrViewFileId;
    CComponentProxy* m_pCurrUpdateProxy; //Indicate we are calling CComponentProxy::UpdateHostComponent, so the host components' ptr property will get value from CPropertyDescription::GetInstanceComponent::GetHostComponent.
    EReflectOperationType m_reflectOperateType;
    CPropertyDescriptionBase* m_pCurrReflectProperty;
    CSerializer* m_pRemoveChildInfo;
    CDependencyDescription* m_pCurrReflectDependency;

    std::map<uint32_t, TCreatePropertyFunc>* m_pPropertyCreatorMap;
    std::map<uint32_t, TString> m_abstractComponentNameMap;
    // This map save the inherit relationship for all components. so when we instance a component pointer, we can decide which instance to generate.
    std::map<uint32_t, std::vector<uint32_t> >* m_pComponentInheritMap;
    // key is the class guid to query, value is it's base class.
    std::map<uint32_t, uint32_t>* m_pComponentBaseClassMap;

    // This map store all reference info, key value is the real component id. 
    // This data is dynamic register and unregistered. while static data is CComponentProject::m_pReferenceIdMap
    std::map<uint32_t, std::vector<CComponentReference*>> m_referenceIdMap;
    std::map<uint32_t, CComponentReference*> m_referenceMap;

    std::map<uint32_t, CComponentProxy*> m_proxyInCurScene;

    // This list saves the file need to be re-save after loaded, because of property maintain logic.
    std::set<uint32_t> m_refreshFileList;
};



#endif