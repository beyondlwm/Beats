#ifndef RESOURCE_RESOURCEMANAGER_H__INCLUDE
#define RESOURCE_RESOURCEMANAGER_H__INCLUDE

#include <Queue>
#include <mutex>
#include <condition_variable>
#include "Resource.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "ResourcePathManager.h"

class CResourceManager
{
    BEATS_DECLARE_SINGLETON(CResourceManager);
public:
    bool InitLoadThread();
    template <typename T>
    SharePtr<T> GetResource(const TString &filename, bool bAsync);
    bool CleanUp();
    std::condition_variable& GetLoadCondition();
    std::mutex& GetLoadMutex();

private:
    bool LoadResource(SharePtr<CResource> pResource, bool bAsync);
    bool CreateLoadThread();
    void RegisterResource(const SharePtr<CResource>& pResource);
    void UnregisterResource(const SharePtr<CResource>& pResource);
    static void LoadResourceThreadFunc(CResourceManager* pMgr);

private:
    bool m_bQuit;
    bool m_bInitialized;
    typedef std::queue<SharePtr<CResource>> TResourceLoadQueue;
    typedef std::map<TString, SharePtr<CResource>> TResourceNameMap;
    TResourceLoadQueue m_loadQueue;
    TResourceNameMap m_resource;
    std::thread m_loadThread;
    std::mutex m_loadQueueMutex;

    std::condition_variable m_loadCondition;
    std::mutex m_loadMutex;

};

template <typename T>
SharePtr<T> CResourceManager::GetResource( const TString &strFilePath, bool bAsync )
{
    SharePtr<T> pRet;
    EResourcePathType pathType = CResourcePathManager::GetInstance()->GetResourcePathType(T::RESOURCE_TYPE);
    TString strPath = CResourcePathManager::GetInstance()->GetResourcePath(pathType);
    strPath.append(strFilePath);
    TResourceNameMap::iterator iter = m_resource.find(strPath);
    if(iter == m_resource.end())
    {
        pRet = new T;
        pRet->SetFilePath(strPath);
        LoadResource(pRet, bAsync);
    }
    else
    {
        pRet = iter->second;
    }
    return pRet;
}

#endif