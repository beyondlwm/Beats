#include "stdafx.h"
#include "ObjectManager.h"
#include "BaseObject.h"
#include "GameObject/Beat.h"

CObjectManager* CObjectManager::m_pInstance = NULL;

CObjectManager::CObjectManager()
{
}

CObjectManager::~CObjectManager()
{
    BEATS_SAFE_DELETE_VECTOR(m_objects);
    BEATS_SAFE_DELETE_VECTOR(m_recycleObjects);
}

void CObjectManager::Init()
{
}

void CObjectManager::UnInit()
{

}

void CObjectManager::RecycleObject( CBaseObject* pObject )
{
    pObject->OnRecycle();
    pObject->SetActive(false);
    size_t uId = pObject->GetID();
    BEATS_ASSERT(uId < m_objects.size(), _T("Invalid Id %d"), uId);
    m_recycleObjects.push_back(m_objects[uId]);
    m_objects[uId] = NULL;
}

CBaseObject* CObjectManager::CreateObject(EGameObjectType type)
{
    CBaseObject* pRet = NULL;
    switch(type)
    {
    case eGOT_Beat:
        {
            if (m_recycleObjects.size() > 0)
            {
                pRet = m_recycleObjects.back();
                m_recycleObjects.pop_back();
            }
            else
            {
                pRet = new CMusicBeat(INVALID_ID);
            }
            pRet->OnReCreate();
            pRet->SetActive(true);
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Invalid game object type %d"), type);
    }
    return pRet;
}

CBaseObject* CObjectManager::GetObject( unsigned long id )
{
    CBaseObject* pRet = NULL;
    BEATS_ASSERT(id < m_objects.size(), _T("Invalid ID %d"), id);
    if (id < m_objects.size())
    {
        pRet = m_objects[id];
    }
    return pRet;
}

void CObjectManager::RemoveAllObjects( bool recycle )
{
    if (recycle)
    {
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (m_objects[i] != NULL)
            {
                m_objects[i]->UnInit();
                m_recycleObjects.push_back(m_objects[i]);
            }
        }
        m_objects.clear();
    }
    else
    {
        for (size_t i = 0; i < m_objects.size(); ++i)
        {
            if (m_objects[i] != NULL)
            {
                m_objects[i]->UnInit();
                BEATS_SAFE_DELETE(m_objects[i]);
            }
        }
        m_objects.clear();
        for (size_t i = 0; i < m_recycleObjects.size(); ++i)
        {
            BEATS_SAFE_DELETE(m_recycleObjects[i]);
        }
        m_recycleObjects.clear();
    }
}


void CObjectManager::Update( float deltaTimeMs )
{
    for (size_t i = 0; i < m_objects.size();++i)
    {
        CBaseObject* pObject = m_objects[i];
        if (pObject != NULL && pObject->IsActive())
        {
            pObject->Update(deltaTimeMs);
        }
    }
}

void CObjectManager::Register( CBaseObject* pElem )
{
    if (pElem->GetID() == INVALID_ID)
    {
        pElem->SetID(m_objects.size());
        m_objects.push_back(pElem);
    }
    else
    {
        m_objects[pElem->GetID()] = pElem;
    }
}

void CObjectManager::Unregister( CBaseObject* pElem )
{
    size_t uID = pElem->GetID();
    BEATS_ASSERT(uID < m_objects.size(), _T("Invalid ID of object %d"), uID);
    m_objects[uID] = NULL;
}