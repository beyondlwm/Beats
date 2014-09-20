#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "IdManager/IdManager.h"
#include "ComponentProxy.h"
#include "ComponentProject.h"

CComponentInstance::CComponentInstance()
    : m_pProxyComponent(NULL)
{

}

CComponentInstance::~CComponentInstance()
{
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

void CComponentInstance::Serialize(CSerializer& serializer)
{
    if (m_pProxyComponent != NULL)
    {
        m_pProxyComponent->Serialize(serializer, eVT_CurrentValue);
    }
    else
    {
        size_t uFilePos = 0;
        size_t uDataSize = 0;
        CSerializer* pSerializer = CComponentInstanceManager::GetInstance()->GetFileSerializer();
        if (CComponentInstanceManager::GetInstance()->GetProject()->QueryComponentFileLayout(GetId(), uFilePos, uDataSize))
        {
            pSerializer->SetReadPos(uFilePos);
            serializer.Serialize(*pSerializer, uDataSize);
        }
    }
}