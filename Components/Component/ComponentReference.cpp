#include "stdafx.h"
#include "ComponentReference.h"

CComponentReference::CComponentReference(CComponentProxy* pComponentHost)
    : super(NULL)
    , m_pComponentHost(pComponentHost)
{
    BEATS_ASSERT(m_pComponentHost != NULL);
    m_pProperties = (std::vector<CPropertyDescriptionBase*>*)pComponentHost->GetPropertyPool();
    m_pDependenciesDescription = new std::vector<CDependencyDescription*>();
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>();
}

CComponentReference::~CComponentReference()
{
    m_pProperties = NULL;
    BEATS_SAFE_DELETE(m_pBeConnectedDependencyLines);
    BEATS_SAFE_DELETE(m_pDependenciesDescription);
}

size_t CComponentReference::GetGuid() const
{
    return m_pComponentHost->GetGuid();
}

size_t CComponentReference::GetParentGuid() const
{
    return m_pComponentHost->GetGuid();
}

const TCHAR* CComponentReference::GetClassStr() const
{
    return m_pComponentHost->GetClassStr();
}