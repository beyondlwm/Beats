#ifndef BEATS_GAME_OBJECTMANAGER_H__INCLUDE
#define BEATS_GAME_OBJECTMANAGER_H__INCLUDE

#include "GameObjectDef.h"

class CBaseObject;

class CObjectManager
{
    BEATS_DECLARE_SINGLETON(CObjectManager);

public:
    void Init();
    void UnInit();
    void Update(float fDeltaTime);
    void RecycleObject(CBaseObject* pObject);
    CBaseObject* CreateObject(EGameObjectType type);
    void RemoveAllObjects(bool recycle);
    CBaseObject* GetObject(unsigned long id);

    void Register(CBaseObject* pElem);
    void Unregister(CBaseObject* pElem);


private:
    std::vector<CBaseObject*> m_objects;
    std::vector<CBaseObject*> m_recycleObjects;
};


#endif
