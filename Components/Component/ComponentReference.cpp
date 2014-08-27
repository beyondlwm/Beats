#include "stdafx.h"
#include "ComponentReference.h"

CComponentReference::CComponentReference(CComponentProxy* pComponentHost)
    : m_pHostProxy(pComponentHost)
{
    BEATS_ASSERT(m_pHostProxy != NULL);
    m_pProperties = (std::vector<CPropertyDescriptionBase*>*)pComponentHost->GetPropertyPool();
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>();
    SetDisplayName(m_pHostProxy->GetDisplayName().c_str());
    SetUserDefineDisplayName(m_pHostProxy->GetUserDefineDisplayName().c_str());
    SetCatalogName(m_pHostProxy->GetCatalogName().c_str());
}

CComponentReference::~CComponentReference()
{
    m_pProperties = NULL;
    BEATS_SAFE_DELETE(m_pBeConnectedDependencyLines);
}

CComponentProxy* CComponentReference::GetHostProxy()
{
    return m_pHostProxy;
}

size_t CComponentReference::GetGuid() const
{
    return m_pHostProxy->GetGuid();
}

size_t CComponentReference::GetParentGuid() const
{
    return m_pHostProxy->GetGuid();
}

const TCHAR* CComponentReference::GetClassStr() const
{
    return m_pHostProxy->GetClassStr();
}