#include "stdafx.h"
#include "ResourceManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"

CResourceManager* CResourceManager::m_pInstance = 0;

CResourceManager::CResourceManager()
    : m_bQuit(false)
    , m_bInitialized(false)
{

}

CResourceManager::~CResourceManager()
{
    m_bQuit = true;
    m_loadCondition.notify_all();
    if(m_bInitialized)
        m_loadThread.detach();
    m_loadQueueMutex.lock();
    while(m_loadQueue.size())
    {
        m_loadQueue.pop();
    }
    m_loadQueueMutex.unlock();

    for (TResourceNameMap::iterator iter = m_resource.begin(); iter != m_resource.end(); ++iter)
    {
#ifdef _DEBUG
        int iRefCount = iter->second.RefCount();
        if (iRefCount > 1)
        {
            TCHAR szBuffer1[MAX_PATH];
            _stprintf(szBuffer1, _T("File: %s still has %d reference!\nRecyle all resources before CResourceManager destroied!\nReference Pos:\n"), 
                            iter->second->GetFilePath().c_str(), 
                            iter->second.RefCount());
            std::string strRet;
            iter->second.GetReferencePosString(strRet);
            TCHAR szBuffer2[10240];
            CStringHelper::GetInstance()->ConvertToTCHAR(strRet.c_str(), szBuffer2, 10240);
            BEATS_ASSERT(false, _T("%s %s"), szBuffer1, szBuffer2);
        }
#endif
    }
    
}

bool CResourceManager::InitLoadThread()
{
    bool bRet = true;
    if (!m_bInitialized)
    {
        m_bQuit = false;
        m_bInitialized = true;
        bRet = CreateLoadThread();
        if (bRet)
        {
            m_bInitialized = true;
        }
    }
    return bRet;
}

bool CResourceManager::LoadResource(SharePtr<CResource> pResource, bool bAsync )
{
    BEATS_ASSERT(pResource, _T("Need a fresh share pointer for output."));

    bool bRet = false;
    RegisterResource(pResource);
    if (bAsync)
    {
        InitLoadThread();
        m_loadQueueMutex.lock();
        m_loadQueue.push(pResource);
        m_loadQueueMutex.unlock(); 
        m_loadCondition.notify_all();
    }
    else
    {
        pResource->Load();
        bRet = true;
    }
    return bRet;
}

bool CResourceManager::CleanUp()
{
    //TODO: clean those resource are not referenced.
    return true;
}

bool CResourceManager::CreateLoadThread()
{
    m_loadThread = std::thread(LoadResourceThreadFunc, this);
    return true;
}

std::condition_variable& CResourceManager::GetLoadCondition()
{
    
    return m_loadCondition;
}

std::mutex& CResourceManager::GetLoadMutex()
{
    return m_loadMutex;
}

void CResourceManager::RegisterResource(const SharePtr<CResource>& pResource)
{
    const TString& strFilePath = pResource->GetFilePath();
    BEATS_ASSERT(m_resource.find(strFilePath) == m_resource.end(), _T("Can't register a resource twice: %s"), strFilePath.c_str());
    m_resource[strFilePath] = pResource;
}

void CResourceManager::UnregisterResource(const SharePtr<CResource>& pResource)
{
    const TString& strFilePath = pResource->GetFilePath();
    TResourceNameMap::iterator iter = m_resource.find(strFilePath);
    BEATS_ASSERT( iter != m_resource.end(), _T("Can't Unregister a resource %s"), strFilePath.c_str());
    m_resource.erase(iter);
}

void CResourceManager::LoadResourceThreadFunc(CResourceManager* pMgr)
{
    //TODO: wait for program terminate.
    while (!pMgr->m_bQuit)
    {
        pMgr->m_loadQueueMutex.lock();
        if (pMgr->m_loadQueue.size() > 0)
        {
            SharePtr<CResource> pResource = pMgr->m_loadQueue.front();
            pMgr->m_loadQueue.pop();
            
            if (pResource->Load())
            {
                pResource->SetLoadedFlag(true);
            }
            pMgr->m_loadQueueMutex.unlock();
        }
        else
        {
            pMgr->m_loadQueueMutex.unlock();
            std::unique_lock <std::mutex> lock(CResourceManager::GetInstance()->GetLoadMutex());
            CResourceManager::GetInstance()->GetLoadCondition().wait(lock); 
        }   
    }
}