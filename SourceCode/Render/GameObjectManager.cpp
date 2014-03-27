#include "stdafx.h"
#include "GameObjectManager.h"
#include "GameObject.h"


CGameObjectManager::CGameObjectManager()
{

}

CGameObjectManager::~CGameObjectManager()
{

}

CGameObject * CGameObjectManager::CreateGameObject()
{
    CGameObject *object = new CGameObject;
    m_objects.push_back(object);
    return object;
}

void CGameObjectManager::DestroyObject( CGameObject *object)
{
    for(auto itr = m_objects.begin(); itr != m_objects.end(); ++itr)
    {
        if(*itr == object)
        {
            m_objects.erase(itr);
            delete object;
            return;
        }
    }
}
