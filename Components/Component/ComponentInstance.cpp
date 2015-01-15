#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "IdManager/IdManager.h"
#include "ComponentProxy.h"
#include "ComponentProject.h"

CComponentInstance::CComponentInstance()
    : m_uDataPos(0xFFFFFFFF)
    , m_uDataSize(0xFFFFFFFF)
    , m_pProxyComponent(NULL)
    , m_pSyncProxyComponent(NULL)
{

}

CComponentInstance::~CComponentInstance()
{
    if (m_pSyncProxyComponent != NULL)
    {
        m_pSyncProxyComponent->RemoveSyncComponent(this);
    }
    if (m_pProxyComponent != NULL)
    {
        BEATS_ASSERT(m_pProxyComponent->GetHostComponent() == this);
        BEATS_SAFE_DELETE(m_pProxyComponent);
    }
}

void CComponentInstance::Uninitialize()
{
    super::Uninitialize();

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
    BEATS_ASSERT(m_pSyncProxyComponent == NULL);
    m_pSyncProxyComponent = pProxy;
}

void CComponentInstance::Serialize(CSerializer& serializer)
{
    if (m_pProxyComponent != NULL)
    {
        m_pProxyComponent->Serialize(serializer, eVT_CurrentValue);
        BEATS_ASSERT(m_uDataSize == 0xFFFFFFFF && m_uDataPos == 0xFFFFFFFF);
    }
    else
    {
        BEATS_ASSERT( m_uDataPos != 0xFFFFFFFF && m_uDataSize != 0xFFFFFFFF, _T("Can't clone a component which is not created by data"));
        CSerializer* pSerializer = CComponentInstanceManager::GetInstance()->GetFileSerializer();
        if (m_uDataPos != 0xFFFFFFFF && m_uDataSize != 0xFFFFFFFF)
        {
            pSerializer->SetReadPos(m_uDataPos);
            serializer.Serialize(*pSerializer, m_uDataSize);
        }
    }
}

CComponentBase* CComponentInstance::CloneInstance()
{
    // Use bOriginState to avoid logic mistake when nested call.
    bool bOriginState = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true);
    CComponentBase* pNewInstance = Clone(false, NULL, 0xFFFFFFFF, false);
    CSerializer serializer;
    Serialize(serializer);
#ifdef _DEBUG
    uint32_t uDataSize, uGuid, uId;
    serializer >> uDataSize >> uGuid >> uId;
    BEATS_ASSERT(uGuid == GetGuid() && uId == GetId());
#else
    serializer.SetReadPos(12);
#endif
    CComponentInstanceManager::GetInstance()->SetForbidDependencyResolve(true);
    CDependencyDescription* pCurReflectDependency = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
    CComponentProxyManager::GetInstance()->SetCurrReflectDependency(NULL);
    bool bIgnoreReflect = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();
    CComponentProxyManager::GetInstance()->SetReflectCheckFlag(false);
    CPropertyDescriptionBase* pCurRelfectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
    CComponentProxyManager::GetInstance()->SetCurrReflectDescription(NULL);
    pNewInstance->ReflectData(serializer);
    CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pCurRelfectProperty);
    CComponentProxyManager::GetInstance()->SetReflectCheckFlag(bIgnoreReflect);
    CComponentProxyManager::GetInstance()->SetCurrReflectDependency(pCurReflectDependency);
    CComponentInstanceManager::GetInstance()->SetForbidDependencyResolve(false);
    BEATS_ASSERT(serializer.GetWritePos() == serializer.GetReadPos());
    CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginState);
    return pNewInstance;
}