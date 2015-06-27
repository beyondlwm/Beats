#include "stdafx.h"
#include "IdManager.h"

CIdManager::CIdManager()
: m_lastId(0)
, uLockCount(0)
{

}

CIdManager::~CIdManager()
{

}

uint32_t CIdManager::GenerateId()
{
    BEATS_ASSERT(0 == uLockCount, _T("can't Generate id when it is locked!"));
    uint32_t uRet = 0;
    if (m_freeIdPool.size() > 0)
    {
        std::set<uint32_t>::iterator iter = m_freeIdPool.begin();
        uRet = *iter;
        m_freeIdPool.erase(iter);
    }
    else
    {
        uRet = m_lastId++;
    }
    return uRet;
}

void CIdManager::RecycleId( uint32_t id )
{
    if (0 == uLockCount)
    {
        BEATS_ASSERT( id < m_lastId, _T("Can't recycle an id which is not reserved."));
        if (id < m_lastId)
        {
            if (id == m_lastId - 1)
            {
                --m_lastId;
                while (m_freeIdPool.find(m_lastId - 1) != m_freeIdPool.end())
                {
                    m_freeIdPool.erase(--m_lastId);
                }
            }
            else
            {
                BEATS_ASSERT(m_freeIdPool.find(id) == m_freeIdPool.end(), _T("Id: %d can't be recycled twice!"), id);
                m_freeIdPool.insert(id);
            }
        }
    }
}

bool CIdManager::ReserveId( uint32_t id , bool bCheckIsAlreadyRequested/* = true*/)
{
    bool bRet = true;
    if (0 == uLockCount)
    {
        bRet = false;
        if (id >= m_lastId)
        {
            for (uint32_t i = m_lastId; i < id; ++i)
            {
                BEATS_ASSERT(m_freeIdPool.find(i) == m_freeIdPool.end());
                m_freeIdPool.insert(i);
            }
            m_lastId = id + 1;
            bRet = true;
        }
        else
        {
            bRet = m_freeIdPool.find(id) != m_freeIdPool.end();
            if (bRet)
            {
                m_freeIdPool.erase(id);
            }
        }
        BEATS_ASSERT(!bCheckIsAlreadyRequested || bRet, _T("Id: %d can't be request twice!"), id);
    }

    return bRet;
}

void CIdManager::Reset()
{
    m_lastId = 0;
    m_freeIdPool.clear();
}

bool CIdManager::IsIdFree(uint32_t id)
{
    bool bRet = id >= m_lastId || m_freeIdPool.find(id) != m_freeIdPool.end();
    return bRet;
}

void CIdManager::Lock()
{
    ++uLockCount;
}

void CIdManager::UnLock()
{
    BEATS_ASSERT(uLockCount > 0);
    --uLockCount;
}

bool CIdManager::IsLocked() const
{
    return uLockCount > 0;
}
