#include "stdafx.h"
#include "ComponentManagerBase.h"
#include "IdManager/IdManager.h"

void DefaultAddDependencyFunc(void* pContainer, void* pDependency)
{
    ((std::vector<void*>*)(pContainer))->push_back((void*)pDependency);
}

CComponentManagerBase::CComponentManagerBase()
{
    m_pIdManager = new CIdManager;
    m_pComponentTemplateMap = new std::map<size_t, CComponentBase*>;
    m_pComponentInstanceMap = new std::map<size_t, std::map<size_t, CComponentBase*>*>;
    m_pDependencyResolver = new std::vector<SDependencyResolver*>;
}

CComponentManagerBase::~CComponentManagerBase()
{
    BEATS_SAFE_DELETE(m_pIdManager);
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
    for (; iter != m_pComponentInstanceMap->end(); ++iter)
    {
        std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin();
        for (; subIter != iter->second->end(); ++subIter)
        {
            BEATS_SAFE_DELETE(subIter->second);
        }
        BEATS_SAFE_DELETE(iter->second);
    }
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);

    typedef std::map<size_t, CComponentBase*> TComponentMap;
    BEATS_SAFE_DELETE_MAP((*m_pComponentTemplateMap), TComponentMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pDependencyResolver);
}

bool CComponentManagerBase::RegisterTemplate( CComponentBase* pComponent )
{
    std::map<size_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->find(pComponent->GetGuid());
    BEATS_ASSERT(iter == m_pComponentTemplateMap->end(), _T("component %s guid 0x%x is already registered!"), pComponent->GetClassStr(), pComponent->GetGuid());
    if (iter == m_pComponentTemplateMap->end())
    {
        (*m_pComponentTemplateMap)[pComponent->GetGuid()] = pComponent;
    }
    return iter == m_pComponentTemplateMap->end();
}

bool CComponentManagerBase::RegisterInstance(CComponentBase* pNewInstance)
{
    size_t guid = pNewInstance->GetGuid();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    if (iter == m_pComponentInstanceMap->end())
    {
        (*m_pComponentInstanceMap)[guid] = new std::map<size_t, CComponentBase*>;
        iter = m_pComponentInstanceMap->find(guid);
    }
    BEATS_ASSERT(iter->second->find(pNewInstance->GetId()) == iter->second->end(), _T("Create an existing component! GUID:0x%x, InstanceId: %d"), guid, pNewInstance->GetId());
    (*iter->second)[pNewInstance->GetId()] = pNewInstance;
    return true;
}

bool CComponentManagerBase::UnregisterInstance(CComponentBase* pNewInstance)
{
    size_t guid = pNewInstance->GetGuid();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    BEATS_ASSERT(iter != m_pComponentInstanceMap->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    std::map<size_t, CComponentBase*>::iterator subIter = iter->second->find(pNewInstance->GetId());
    BEATS_ASSERT(subIter != iter->second->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    iter->second->erase(subIter);
    if (iter->second->size() == 0)
    {
        BEATS_SAFE_DELETE(iter->second);
        m_pComponentInstanceMap->erase(iter);
    }
    return true;
}

CComponentBase* CComponentManagerBase::GetComponentTemplate( size_t guid ) const
{
    CComponentBase* pResult = NULL;
    std::map<size_t, CComponentBase*>::const_iterator iter = m_pComponentTemplateMap->find(guid);
    if (iter != m_pComponentTemplateMap->end())
    {
        pResult = iter->second;
    }
    return pResult;
}

CComponentBase* CComponentManagerBase::CreateComponent( size_t guid, bool bCloneFromTemplate, bool bManualManage/* = false*/, size_t specifiedInstanceId /*=0xffffffff*/, bool bCheckRequestId/* = true*/, CSerializer* pData /*=NULL*/, bool bCallInitFunc/* = true*/)
{
    CComponentBase* pNewInstance = NULL;
#if(BEATS_PLATFORM == PLATFORM_WIN32)
    __try
    {
#endif
        CComponentBase* pTemplate = GetComponentTemplate(guid);
        BEATS_ASSERT(pTemplate != NULL, _T("Create an unknown component, Guid:0x%x id: %d"), guid, specifiedInstanceId);
        pNewInstance = CreateComponentByRef(pTemplate, bCloneFromTemplate, bManualManage, specifiedInstanceId, bCheckRequestId, pData, bCallInitFunc);
#if(BEATS_PLATFORM == PLATFORM_WIN32)
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        BEATS_ASSERT(false, _T("Create Component Failed! GUID :0x%x ManualManage:%s"), guid, bManualManage ? _T("Yes"):_T("No"));
    }
#endif
    return pNewInstance;
}

CComponentBase* CComponentManagerBase::CreateComponentByRef( CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage /*= false*/, size_t specifiedInstanceId /*= 0xFFFFFFFF*/, bool bCheckRequestId /*= true*/, CSerializer* pData /*= NULL*/, bool bCallInitFunc/* = true*/)
{
    BEATS_ASSERT(pComponentRef != NULL, _T("Create an unknown component by NULL ref"));
    if (!bManualManage)
    {
        if (specifiedInstanceId != 0xFFFFFFFF)
        {
            m_pIdManager->ReserveId(specifiedInstanceId, bCheckRequestId);
        }
        else
        {
            specifiedInstanceId = m_pIdManager->GenerateId();
        }
    }
    else
    {
        BEATS_ASSERT(specifiedInstanceId == 0xFFFFFFFF, _T("If you want to manage component instance by yourself, don't require instance id!"));
    }

    // TODO: bCloneValue only makes sense when this is a CComponentEditorProxy.
    CComponentBase* pNewInstance = static_cast<CComponentBase*>(pComponentRef->Clone(bCloneValue, pData, specifiedInstanceId, bCallInitFunc));
    if (!bManualManage)
    {
        RegisterInstance(pNewInstance);
    }
    return pNewInstance;
}

bool CComponentManagerBase::DeleteComponent( CComponentBase* pComponent )
{
    UnregisterInstance(pComponent);
    m_pIdManager->RecycleId(pComponent->GetId());
    BEATS_SAFE_DELETE(pComponent);

    return true;
}

const std::map<size_t, CComponentBase*>* CComponentManagerBase::GetComponentTemplateMap()
{
    return m_pComponentTemplateMap;
}

const std::map<size_t, std::map<size_t, CComponentBase*>*>* CComponentManagerBase::GetComponentInstanceMap()
{
    return m_pComponentInstanceMap;
}

CComponentBase* CComponentManagerBase::GetComponentInstance( size_t uId , size_t uGUID/* = 0xffffffff*/)
{
    CComponentBase* pResult = NULL;
    if (uGUID != 0xFFFFFFFF)
    {
        std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(uGUID);
        if (iter != m_pComponentInstanceMap->end())
        {
            std::map<size_t, CComponentBase*>::iterator subIter = iter->second->find(uId);
            if (subIter != iter->second->end())
            {
                pResult = subIter->second;
            }
        }
    }
    else
    {
        std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
        for (; iter != m_pComponentInstanceMap->end(); ++iter)
        {
            pResult = GetComponentInstance(uId, iter->first);
            if (pResult != NULL)
            {
                break;
            }
        }
    }
    return pResult;
}

CIdManager* CComponentManagerBase::GetIdManager() const
{
    return m_pIdManager;
}

void CComponentManagerBase::AddDependencyResolver( CDependencyDescription* pDescription, size_t uIndex, size_t uGuid, size_t uInstanceId , void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc /*= NULL*/)
{
    SDependencyResolver* pDependencyResovler = new SDependencyResolver;
    pDependencyResovler->pDescription = pDescription;
    pDependencyResovler->uIndex = uIndex;
    pDependencyResovler->uGuid = uGuid;
    pDependencyResovler->uInstanceId = uInstanceId;
    pDependencyResovler->pVariableAddress = pVariableAddress;
    pDependencyResovler->bIsList = bIsList;
    pDependencyResovler->pAddFunc = pFunc == NULL ? DefaultAddDependencyFunc : pFunc;
    m_pDependencyResolver->push_back(pDependencyResovler);
}
