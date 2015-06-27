#ifndef BEATS_UTILITY_IDMANAGER_IDMANAGER_H__INCLUDE
#define BEATS_UTILITY_IDMANAGER_IDMANAGER_H__INCLUDE

#include <set>

class CIdManager
{
public:
    CIdManager();
    ~CIdManager();
public:
    uint32_t GenerateId();
    bool ReserveId(uint32_t id, bool bCheckIsAlreadyRequested = true);
    void RecycleId(uint32_t id);
    void Reset();
    bool IsIdFree(uint32_t id);
    void Lock();
    void UnLock();
    bool IsLocked() const;
private:
    uint32_t uLockCount;
    uint32_t m_lastId;
    std::set<uint32_t> m_freeIdPool;
};


#endif