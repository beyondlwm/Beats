#include "stdafx.h"
#include "PhysicsManager.h"

CPhysicsManager* CPhysicsManager::m_pInstance = NULL;

CPhysicsManager::CPhysicsManager()
: m_bActive(true)
{

}

CPhysicsManager::~CPhysicsManager()
{
}

void CPhysicsManager::Init()
{

}

void CPhysicsManager::Update(const float deltaTimeMs)
{
    if (m_bActive)
    {
        for (uint32_t i = 0; i < m_elements.size(); ++i)
        {
            if (m_elements[i] != NULL)
            {
                if (m_elements[i]->IsActive())
                {
                    m_elements[i]->Update(deltaTimeMs);
                }
            }
        }
    }
}

void CPhysicsManager::SetActive(bool bActive)
{
    m_bActive = bActive;
}

void CPhysicsManager::Register(CPhysicsElement* elem)
{
    if (m_freePos.size() == 0)
    {
        elem->SetID(m_elements.size());
        m_elements.push_back(elem);
    }
    else
    {
        m_elements[m_freePos.back()] = elem;
        elem->SetID(m_freePos.back());
        m_freePos.pop_back();
    }
}

void CPhysicsManager::Unregister(CPhysicsElement* elem)
{
    uint32_t uId = elem->GetID();
    BEATS_ASSERT(m_elements[uId] == elem);
    m_elements[uId] = NULL;
    m_freePos.push_back(uId);
}
