#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "IdManager/IdManager.h"
#include "ComponentProxy.h"
#include "ComponentProject.h"

CComponentInstance::CComponentInstance()
    : m_uDataPos(0xFFFFFFFF)
    , m_uDataSize(0xFFFFFFFF)
    , m_pReflectComponentOwner(nullptr)
    , m_pProxyComponent(NULL)
    , m_pSyncProxyComponent(NULL)
{

}

CComponentInstance::~CComponentInstance()
{
    if (m_pReflectComponentOwner != nullptr)
    {
        m_pReflectComponentOwner->UnregisterReflectComponent(this);
        m_pReflectComponentOwner = nullptr;
    }
    while (m_reflectComponents.size() > 0)//m_reflectComponents size will reduce because when we delete the component, it will call UnregisterReflectComponent.
    {
        CComponentInstance* pReflectComponent = *m_reflectComponents.begin();
        BEATS_ASSERT(pReflectComponent->GetReflectOwner() == this);
        BEATS_ASSERT(!pReflectComponent->IsLoaded() && !pReflectComponent->IsInitialized());
        BEATS_SAFE_DELETE(pReflectComponent);
    }
    SetSyncProxyComponent(NULL);
    if (m_pProxyComponent != NULL)
    {
        BEATS_SAFE_DELETE(m_pProxyComponent);
    }
}

void CComponentInstance::Initialize()
{
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        (*iter)->Initialize();
    }

    // For Editor operation, we always call proxy initialize before instance's initialize.
    // But if we call Initialize manually from code, we need to call proxy initialize.
    if (m_pProxyComponent != NULL && !m_pProxyComponent->IsInitialized())
    {
        m_pProxyComponent->Initialize();
    }
    uint32_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF && CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId, GetGuid()) == nullptr)
    {
        // We must call RegisterInstance before Initialize (so we can resolve dependency).
        // But if we call Uninitialize in code and call Initialize again, we have to Register this component again manually.
        CComponentProxyManager::GetInstance()->GetIdManager()->ReserveId(uComponentId);
        CComponentProxyManager::GetInstance()->RegisterInstance(m_pProxyComponent);
        CComponentInstanceManager::GetInstance()->GetIdManager()->ReserveId(uComponentId);
        CComponentInstanceManager::GetInstance()->RegisterInstance(this);
    }

    super::Initialize();
}

void CComponentInstance::Uninitialize()
{
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        (*iter)->Uninitialize();
    }

    if (m_pProxyComponent != NULL)
    {
        BEATS_ASSERT(m_pProxyComponent->GetHostComponent() == this);
        m_pProxyComponent->Uninitialize();
    }
    uint32_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF)
    {
        CComponentInstanceManager::GetInstance()->UnregisterInstance(this);
        CComponentInstanceManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
    }
    super::Uninitialize();
}

bool CComponentInstance::Load()
{
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        if (!(*iter)->IsLoaded())
        {
            (*iter)->Load();
        }
    }
    return super::Load();
}

bool CComponentInstance::Unload()
{
    for (auto iter = m_reflectComponents.begin(); iter != m_reflectComponents.end(); ++iter)
    {
        if ((*iter)->IsLoaded())
        {
            (*iter)->Unload();
        }
    }
    return super::Unload();
}

void CComponentInstance::SetDataPos(uint32_t uDataPos)
{
    m_uDataPos = uDataPos;
}

uint32_t CComponentInstance::GetDataPos() const
{
    return m_uDataPos;
}

void CComponentInstance::SetDataSize(uint32_t uDataSize)
{
    m_uDataSize = uDataSize;
}

uint32_t CComponentInstance::GetDataSize() const
{
    return m_uDataSize;
}

void CComponentInstance::SetProxyComponent(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_pProxyComponent == NULL || pProxy == NULL, _T("Proxy component can only set once!"));
    m_pProxyComponent = pProxy;
}

CComponentProxy* CComponentInstance::GetProxyComponent() const
{
    return m_pProxyComponent;
}

void CComponentInstance::SetSyncProxyComponent(CComponentProxy* pProxy)
{
    if (pProxy != m_pSyncProxyComponent)
    {
        if (m_pSyncProxyComponent != NULL)
        {
            m_pSyncProxyComponent->RemoveSyncComponent(this);
        }
        m_pSyncProxyComponent = pProxy;
        if (m_pSyncProxyComponent != NULL)
        {
            m_pSyncProxyComponent->AddSyncComponent(this);
        }
    }
}

CComponentInstance* CComponentInstance::CloneInstance()
{
    bool bOriginalValue = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true);
    CComponentInstanceManager::GetInstance()->GetFileSerializerMutex().lock();
    CSerializer* pSerializer = CComponentInstanceManager::GetInstance()->GetFileSerializer();
    static CSerializer serializer;
    // If we are in editor mode, we get the data from proxy. if we are not in editor mode, we can only get the data from the record.
    if (m_pProxyComponent != NULL)
    {
        serializer.Reset();
        m_pProxyComponent->ExportDataToHost(serializer, eVT_CurrentValue);
        BEATS_ASSERT(m_uDataSize == 0xFFFFFFFF && m_uDataPos == 0xFFFFFFFF);
        pSerializer = &serializer;
    }
    else
    {
        BEATS_ASSERT(m_uDataPos != 0xFFFFFFFF && m_uDataSize != 0xFFFFFFFF, _T("Can't clone a component which is not created by data"));
        pSerializer->SetReadPos(m_uDataPos);
    }
#ifdef _DEBUG
    uint32_t uDataSize, uGuid, uId;
    (*pSerializer) >> uDataSize >> uGuid >> uId;
    BEATS_ASSERT(uGuid == GetGuid() && uId == GetId());
#else
    pSerializer->SetReadPos(pSerializer->GetReadPos() + 12);
#endif
    CComponentBase* pNewInstance = Clone(false, pSerializer, 0xFFFFFFFF, false);
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginalValue);
    CComponentInstanceManager::GetInstance()->GetFileSerializerMutex().unlock();
    return static_cast<CComponentInstance*>(pNewInstance);
}

void CComponentInstance::SetReflectOwner(CComponentInstance* pReflectOwner)
{
    m_pReflectComponentOwner = pReflectOwner;
}

CComponentInstance* CComponentInstance::GetReflectOwner() const
{
    return m_pReflectComponentOwner;
}

const std::set<CComponentInstance*>& CComponentInstance::GetReflectComponents() const
{
    return m_reflectComponents;
}

void CComponentInstance::RegisterReflectComponent(CComponentInstance* pComponent)
{
    BEATS_ASSERT(m_reflectComponents.find(pComponent) == m_reflectComponents.end());
    m_reflectComponents.insert(pComponent);
    BEATS_ASSERT(pComponent->GetReflectOwner() == nullptr && pComponent->GetId() == 0xFFFFFFFF);
    pComponent->SetReflectOwner(this);
}

void CComponentInstance::UnregisterReflectComponent(CComponentInstance* pComponent)
{
    BEATS_ASSERT(m_reflectComponents.find(pComponent) != m_reflectComponents.end());
    m_reflectComponents.erase(pComponent);
    BEATS_ASSERT(pComponent->GetReflectOwner() == this && pComponent->GetId() == 0xFFFFFFFF);
    pComponent->SetReflectOwner(nullptr);
}

bool CComponentInstance::OnPropertyChange(void* /*pVariableAddr*/, CSerializer* /*pNewValueToBeSet*/)
{
    return false;
}

bool CComponentInstance::OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent)
{
    return false;
}

bool CComponentInstance::OnDependencyListChange(void* pComponentAddr, enum EDependencyChangeAction action, CComponentBase* pComponent)
{
    return false;
}

void CComponentInstance::OnSave()
{
    // Do nothing
}

void CComponentInstance::OnPropertyImport(const TString& strPropertyName, const TString& strFile)
{
    // Do nothing
}
