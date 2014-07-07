#include "stdafx.h"
#include "ComponentInstanceManager.h"
#include "ComponentBase.h"
#include "ComponentProxy.h"
#include "Utility/Serializer/Serializer.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/UtilityManager.h"
#include "Utility/IdManager/IdManager.h"

CComponentInstanceManager* CComponentInstanceManager::m_pInstance = NULL;

CComponentInstanceManager::CComponentInstanceManager()
{
}

CComponentInstanceManager::~CComponentInstanceManager()
{
    Release();
    // Don't remove GetInstance(), it is force to create an instance of CUtilityManager, so we can visit the member of it.
    CUtilityManager::GetInstance();
    CUtilityManager::Destroy();
}

void CComponentInstanceManager::Import( CSerializer& serializer)
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
                CComponentBase* pComponent = CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, &serializer, false);
                pComponent;
                BEATS_ASSERT(uStartPos + uDataSize == serializer.GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uDataSize, serializer.GetReadPos() - uStartPos);
                serializer.SetReadPos(uStartPos + uDataSize);
            }
        }

        // 2. Resolve dependency.
        CComponentInstanceManager::GetInstance()->ResolveDependency();
        
        // 3. Call Initialize.
        InitializeAllInstance();
    }
}

void CComponentInstanceManager::ResolveDependency()
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

size_t CComponentInstanceManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}
