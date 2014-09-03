#include "stdafx.h"
#include "IdManager.h"

CIdManager::CIdManager()
: m_lastId(0)
, m_bLock(false)
{

}

CIdManager::~CIdManager()
{

}

size_t CIdManager::GenerateId()
{
    size_t uRet = 0;
    if (m_freeIdPool.size() > 0)
    {
        std::set<size_t>::iterator iter = m_freeIdPool.begin();
        uRet = *iter;
        m_freeIdPool.erase(iter);
    }
    else
    {
        uRet = m_lastId++;
    }
    return uRet;
}

void CIdManager::RecycleId( size_t id )
{
    if (!m_bLock)
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

bool CIdManager::ReserveId( size_t id , bool bCheckIsAlreadyRequested/* = true*/)
{
    bool bRet = true;
    if (!m_bLock)
    {
        bRet = false;
        if (id >= m_lastId)
        {
            for (size_t i = m_lastId; i < id; ++i)
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

bool CIdManager::IsIdFree(size_t id)
{
    bool bRet = id >= m_lastId || m_freeIdPool.find(id) != m_freeIdPool.end();
    return bRet;
}

void CIdManager::Lock()
{
    BEATS_ASSERT(!m_bLock, _T("Can't lock id manager twice!"));
    m_bLock = true;
}

void CIdManager::UnLock()
{
    BEATS_ASSERT(m_bLock, _T("Can't unlock id manager twice!"));
    m_bLock = false;
}
