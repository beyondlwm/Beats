#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "IdManager/IdManager.h"
#include "ComponentProxy.h"
CComponentInstance::CComponentInstance()
    : m_pProxyComponent(NULL)
{

}

CComponentInstance::~CComponentInstance()
{
    bool bDeleteFromCode = m_pProxyComponent != NULL && m_pProxyComponent->GetGraphics() != NULL;
    if (bDeleteFromCode)
    {
        BEATS_ASSERT(m_pProxyComponent->GetHostComponent() == this);
        m_pProxyComponent->SetHostComponent(NULL); //Avoid recursion.
        BEATS_SAFE_DELETE(m_pProxyComponent);
    }
}

void CComponentInstance::Uninitialize()
{
    super::Uninitialize();
    bool bUnintializeFromCode = m_pProxyComponent != NULL && m_pProxyComponent->IsInitialized();
    if (bUnintializeFromCode)
    {
        BEATS_ASSERT(m_pProxyComponent->GetHostComponent() == this);
        m_pProxyComponent->SetHostComponent(NULL); // Avoid recursion.
        m_pProxyComponent->Uninitialize();
        m_pProxyComponent->SetHostComponent(this); // Restore.
    }
    size_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF)
    {
        CComponentInstanceManager::GetInstance()->UnregisterInstance(this);
        CComponentInstanceManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
    }
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
