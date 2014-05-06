#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTMANAGERBASE_H__INCLUDE

class CComponentBase;
class CIdManager;
class CSerializer;
class CDependencyDescription;

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

class CComponentManagerBase
{
public:
    CComponentManagerBase();
    virtual ~CComponentManagerBase();

    bool RegisterTemplate(CComponentBase* pComponent);
    bool RegisterInstance(CComponentBase* pComponent);
    bool UnregisterInstance(CComponentBase* pComponent);

    /* 
    1. param bCloneFromTemplate means, if you wanna create a brand new component or make a copy of template component. When we create component from data, we usually create
    a brand new data, if we create components in editor, we always copy the template.
    2. param bManualManage means, if you wanna manage this new create component by yourself or component manager, if you manage it yourself, you have to remember to recycle
    the memory, and it can't be found by the manager as if component manager never knows your component.
    3. param specifiedInstanceId means, you can specify id by yourself instead of manager, if you request an id which is taken by manager, there will be an assert.
    */
    CComponentBase* GetComponentTemplate(size_t guid) const;
    CComponentBase* CreateComponent(size_t guid, bool bCloneFromTemplate, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL);
    CComponentBase* CreateComponentByRef(CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage = false, size_t specifiedInstanceId = 0xFFFFFFFF, bool bCheckIsAlreadyRequest = true, CSerializer* pSerializer = NULL);
    bool DeleteComponent(CComponentBase* pComponent);

    const std::map<size_t, CComponentBase*>* GetComponentTemplateMap();
    const std::map<size_t, std::map<size_t, CComponentBase*>*>* GetComponentInstanceMap();
    CComponentBase* GetComponentInstance(size_t uId, size_t uGuid = 0xFFFFFFFF);

    CIdManager* GetIdManager() const;

    void AddDependencyResolver( CDependencyDescription* pDescription, size_t uIndex, size_t uGuid, size_t uInstanceId, void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc = NULL);
    virtual void ResolveDependency() = 0;

protected:
    CIdManager* m_pIdManager;
    std::map<size_t, CComponentBase*>* m_pComponentTemplateMap;
    std::map<size_t, std::map<size_t, CComponentBase*>*>* m_pComponentInstanceMap;
    std::vector<SDependencyResolver*>* m_pDependencyResolver;
};

#endif