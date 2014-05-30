#include "stdafx.h"
#include "ComponentManager.h"
#include "ComponentBase.h"
#include "ComponentEditorProxy.h"
#include "Serializer/Serializer.h"
#include "StringHelper/StringHelper.h"
#include "UtilityManager.h"
#include "IdManager/IdManager.h"

CComponentManager* CComponentManager::m_pInstance = NULL;

CComponentManager::CComponentManager()
{
}

CComponentManager::~CComponentManager()
{
    Release();
    // Don't remove GetInstance(), it is force to create an instance of CUtilityManager, so we can visit the member of it.
    CUtilityManager::GetInstance();
    CUtilityManager::Destroy();
}

void CComponentManager::SerializeTemplateData(CSerializer& serializer)
{
    TString strWorkingPath = CUtilityManager::GetInstance()->GetModuleFileName();
    strWorkingPath = CUtilityManager::GetInstance()->FileRemoveName(strWorkingPath.c_str());
    strWorkingPath.append(_T("/")).append(EXPORT_STRUCTURE_DATA_FILENAME);
    serializer.Deserialize(strWorkingPath.c_str());
}


void CComponentManager::Import( CSerializer& serializer)
{
    size_t uVersion = 0;
    serializer >> uVersion;
    if (uVersion != COMPONENT_SYSTEM_VERSION)
    {
        TCHAR szInfo[256];
        _stprintf(szInfo, _T("Data version is %d while our program version is %d"), uVersion, COMPONENT_SYSTEM_VERSION);
        MessageBox(NULL, szInfo, _T("Verion Not Match!"), MB_OK | MB_ICONERROR);
    }
    else
    {
        // 1. Load binarize data.
        size_t uFileCount = 0;
        serializer >> uFileCount;
        for (size_t j = 0; j < uFileCount; ++j)
        {
            size_t uComponentCount = 0;
            serializer >> uComponentCount;
            for (size_t i = 0; i < uComponentCount; ++i)
            {
                size_t uDataSize, uGuid, uId;
                size_t uStartPos = serializer.GetReadPos();
                serializer >> uDataSize >> uGuid >> uId;
                CComponentBase* pComponent = CComponentManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, &serializer, false);
                BEATS_ASSERT(uStartPos + uDataSize == serializer.GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uDataSize, serializer.GetReadPos() - uStartPos);
                serializer.SetReadPos(uStartPos + uDataSize);
            }
        }

        // 2. Resolve dependency.
        CComponentManager::GetInstance()->ResolveDependency();
        
        // 3. Call Initialize.
        for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
        {
            for (std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                BEATS_ASSERT(subIter->second->IsInitialized() == false, _T("Can't initialize component twice!"));
                subIter->second->Initialize();
            }
        }
    }
}

void CComponentManager::ResolveDependency()
{
    for (size_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription == NULL);
        if (pDependencyResolver->pVariableAddress != NULL)
        {
            if (pDependencyResolver->bIsList)
            {
                pDependencyResolver->pAddFunc(pDependencyResolver->pVariableAddress, pComponentToBeLink);
            }
            else
            {
                *(size_t*)pDependencyResolver->pVariableAddress = (size_t)pComponentToBeLink;
            }
        }
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

size_t CComponentManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}

void CComponentManager::ResetComponentContainer()
{
    //Delete two container, because we may also launch the debug tool, and it will init the componentmanager again in CComponentsPage::InitComponentsPage
    for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_pComponentInstanceMap->clear();
    typedef std::map<size_t, CComponentBase*> TComponentMap;
    BEATS_SAFE_DELETE_MAP((*m_pComponentTemplateMap), TComponentMap);
    m_pIdManager->Reset();
}