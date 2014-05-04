#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGER_H__INCLUDE

#include "../expdef.h"

class CComponentBase;
class CComponentEditorProxy;
class CIdManager;
class CPropertyDescriptionBase;
class TiXmlElement;
class CSerializer;
class CDependencyDescription;
class CComponentBase;
struct SDependencyResolver;
class CComponentProject;
class CComponentGraphic;

typedef void (*TAddDependencyFunc)(void* pContainer, void* pDependency);

void DefaultAddDependencyFunc(void* pContainer, void* pDependency);


struct SDependencyResolver
{
    SDependencyResolver()
        : uIndex(0)
        , uGuid(0)
        , uInstanceId(0)
        , bIsList(false)
        , pVariableAddress(NULL)
        , pDescription(NULL)
        , pAddFunc(NULL)
    {

    }
    ~SDependencyResolver()
    {

    }
    size_t uIndex;
    size_t uGuid;
    size_t uInstanceId;
    bool bIsList;
    void* pVariableAddress;
    CDependencyDescription* pDescription;
    TAddDependencyFunc pAddFunc;
};

typedef CComponentEditorProxy* (*TCreateComponentEditorProxyFunc)(CComponentGraphic* pGraphics, size_t guid, size_t parentGuid, TCHAR* className);
typedef CComponentGraphic* (*TCreateGraphicFunc)();
class COMPONENTS_DLL_DECL CComponentManager
{
    BEATS_DECLARE_SINGLETON(CComponentManager);
    typedef CPropertyDescriptionBase* (*TCreatePropertyFunc)(CSerializer* serializer);
public:
    bool RegisterTemplate(CComponentBase* pComponent);
    bool RegisterInstance(CComponentBase* pComponent);
    bool UnregisterInstance(CComponentBase* pComponent);

    void DeserializeTemplateData(const TCHAR* pWorkingPath, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);
    void SerializeTemplateData(CSerializer& serializer);

    CComponentBase* GetComponentTemplate(size_t guid) const;
    /* 
    1. param bCloneFromTemplate means, if you wanna create a brand new component or make a copy of template component. When we create component from data, we usually create
    a brand new data, if we create components in editor, we always copy the template.
    2. param bManualManage means, if you wanna manage this new create component by yourself or component manager, if you manage it yourself, you have to remember to recycle
    the memory, and it can't be found by the manager as if component manager never knows your component.
    3. param specifiedInstanceId means, you can specify id by yourself instead of manager, if you request an id which is taken by manager, there will be an assert.
    */
    CComponentBase* CreateComponent(size_t guid, bool bCloneFromTemplate, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL);
    CComponentBase* CreateComponentByRef(CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL);
    bool DeleteComponent(CComponentBase* pComponent);

    // param bOpenAsCopy means we will create a new file from template file.
    void OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy = false);
    void CloseFile(bool bRefreshProjectData = true);
    const TString& GetCurrentWorkingFilePath() const;

    CComponentProject* GetProject() const;
    CIdManager* GetIdManager() const;

    const std::map<size_t, CComponentBase*>* GetComponentTemplateMap();
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* GetComponentInstanceMap();
    CComponentBase* GetComponentInstance(size_t uId, size_t uGuid = 0xFFFFFFFF);
    void SaveTemplate(const TCHAR* pszFilePath);
    void SaveToFile(const TCHAR* pFileName = NULL);
    size_t GetVersion();
    
    void QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse);
    void RegisterClassInheritInfo(size_t uDerivedClassGuid, size_t uBaseClassGuid);
    TString QueryComponentName(size_t uGuid) const;

    /*
    Editor will call export to binarize all data.
    Program will call import to deserialize all data to memory.
    */
    void Export(const std::vector<TString>& fileList, const TCHAR* pSavePath);
    void Import(CSerializer& serializer);

    void AddDependencyResolver( CDependencyDescription* pDescription, size_t uIndex, size_t uGuid, size_t uInstanceId, void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc = NULL);
    void ResolveDependency();
    void RegisterPropertyCreator(size_t enumType, TCreatePropertyFunc func);
    CPropertyDescriptionBase* CreateProperty(size_t propertyType, CSerializer* serializer);

    void ResetComponentContainer();
private:
    void LoadTemplateDataFromXML(const TCHAR* pWorkingPath);
    void LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc);

private:
    CIdManager* m_pIdManager;
    CComponentProject* m_pProject;
    TString m_currentWorkingFilePath;
    std::map<size_t, CComponentBase*>* m_pComponentTemplateMap;
    std::map<size_t, std::map<size_t, CComponentBase*>*>* m_pComponentInstanceMap;
    std::vector<SDependencyResolver*>* m_pDependencyResolver;
    std::map<size_t, TCreatePropertyFunc>* m_pPropertyCreatorMap;
    std::map<size_t, TString> m_abstractComponentNameMap;
    // This map save the inherit relationship for all components. so when we instance a component pointer, we can decide which instance to generate.
    std::map<size_t, std::vector<size_t>>* m_pComponentInheritMap;
};

#endif