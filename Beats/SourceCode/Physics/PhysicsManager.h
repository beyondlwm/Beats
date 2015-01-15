#ifndef BEATS_PHYSICS_PHYSICSMANAGER_H__INCLUDE
#define BEATS_PHYSICS_PHYSICSMANAGER_H__INCLUDE

#include "PhysicsElement.h"
#define INVALID_ID 0xFFFFFFFF

class CPhysicsManager
{
    BEATS_DECLARE_SINGLETON(CPhysicsManager);
public:
    void Init();
    void Update(const float fDeltaTime);
    void SetActive(bool bActive);
    void Register(CPhysicsElement* elem);
    void Unregister(CPhysicsElement* elem);
private:
    bool m_bActive;
    std::vector<CPhysicsElement*> m_elements;
    std::vector<uint32_t> m_freePos;

};

#endif