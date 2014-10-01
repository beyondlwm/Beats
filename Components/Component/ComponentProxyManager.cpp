#include "stdafx.h"
#include "ComponentProxyManager.h"
#include "ComponentInstanceManager.h"
#include "ComponentProject.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/Serializer/Serializer.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/UtilityManager.h"
#include "Utility/IdManager/IdManager.h"
#include "ComponentGraphic.h"
#include "DependencyDescriptionLine.h"
#include "DependencyDescription.h"
#include "ComponentProxy.h"
#include "ComponentPublic.h"
#include "ComponentInstance.h"
#include "FilePath/FilePathTool.h"
#include <algorithm>
#include "ComponentProjectDirectory.h"
#include "ComponentReference.h"

CComponentProxyManager* CComponentProxyManager::m_pInstance = NULL;

CComponentProxyManager::CComponentProxyManager()
    : m_bCreateInstanceWithProxy(true)
    , m_bLoadingFilePhase(false)
    , m_bExportingPhase(false)
    , m_bReflectCheckFlag(false)
    , m_pCurrReflectPropertyDescription(NULL)
    , m_pCurrReflectDependency(NULL)
{
    m_pPropertyCreatorMap = new std::map<size_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<size_t, std::vector<size_t> >();
}

CComponentProxyManager::~CComponentProxyManager()
{
    BEATS_SAFE_DELETE(m_pIdManager);
    BEATS_ASSERT(m_pComponentInstanceMap->size() == 0, _T("All proxy should be deleted by instance"));
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pDependencyResolver);
    BEATS_SAFE_DELETE(m_pUninitializedComponents);
    BEATS_SAFE_DELETE(m_pPropertyCreatorMap);
    BEATS_SAFE_DELETE(m_pComponentInheritMap);
}

void CComponentProxyManager::OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy /*= false */)
{
    bool bRestoreLoadingPhase = m_bLoadingFilePhase;
    m_bLoadingFilePhase = true;
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    size_t uFileId = pProject->GetComponentFileId(pFilePath);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    std::vector<CComponentProxy*> loadedComponents;
    bool bLoadThisFile = true;
    bool bNewAddThisFile = false;
    // Save cur file.
    if (m_currentViewFilePath.compare(pFilePath) != 0)
    {
        SaveCurFile();
        m_currentViewFilePath.assign(pFilePath);
    }

    // 1. File is in the parent directory (loaded before): just change the content of m_proxyInCurScene
    if (m_loadedFiles.find(uFileId) != m_loadedFiles.end())
    {
        // Change content will be done at last. So do nothing here.
        bLoadThisFile = false;
    }
    else
    {
        CComponentProjectDirectory* pDirectory = m_pProject->FindProjectDirectory(pFilePath, true);
        BEATS_ASSERT(pDirectory != NULL);
        TString strLogicPath;
        // new open a file.
        if (m_currentWorkingFilePath.empty())
        {
            std::vector<CComponentProjectDirectory*> directories;
            CComponentProjectDirectory* pCurDirectory = pDirectory->GetParent();
            while (pCurDirectory != NULL)
            {
                directories.push_back(pCurDirectory);
                pCurDirectory = pCurDirectory->GetParent();
            }
            while (directories.size() > 0)
            {
                LoadFileFromDirectory(directories.back(), &loadedComponents);
                directories.pop_back();
            }
        }
        else
        {
            CComponentProjectDirectory* pCurDirectory = m_pProject->FindProjectDirectory(m_currentWorkingFilePath, true);
            BEATS_ASSERT(pCurDirectory != NULL);
            strLogicPath = pDirectory->MakeRelativeLogicPath(pCurDirectory);

            // 2. File is at the same directory: close current file and open it.
            if (strLogicPath.empty())
            {
                CloseFile(m_currentWorkingFilePath.c_str());
            }
            else
            {
                std::vector<TString> logicPaths;
                CStringHelper::GetInstance()->SplitString(strLogicPath.c_str(), _T("/"), logicPaths);
                BEATS_ASSERT(logicPaths.size() > 0);
                // 3. File is in the son directory: don't close current, load rest files of directory and go on.
                if (logicPaths[0].compare(_T("..")) != 0)
                {
                    // Load rest files of the same directory.
                    const std::vector<size_t>& fileList = pCurDirectory->GetFileList();
                    size_t uCurFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(m_currentWorkingFilePath);
                    for (size_t i = 0;i < fileList.size(); ++i)
                    {
                        if (fileList[i] != uCurFileId)
                        {
                            const TString& strFileName = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileName(fileList[i]);
                            LoadFile(strFileName.c_str(), &loadedComponents);
                        }
                    }
                    // Load sub-directory to target, but don't load the last sub-directory, because we only need one file in it, the target file.
                    CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory;
                    for (int i = 0; i < (int)logicPaths.size() - 1; ++i)
                    {
                        BEATS_ASSERT(pCurLoopDirectory != NULL);
                        pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                        BEATS_ASSERT(pCurLoopDirectory != NULL);
                        LoadFileFromDirectory(pCurLoopDirectory, &loadedComponents);
                    }
                }
                else// 4. File is in other different directory: close current and change directory.
                {
                    // This means the file should be already loaded (since it's in the parent directory)
                    // But it is not in the m_loadedFiles, so this file must be a new added one
                    // So we do some nothing as if it is already loaded.
                    if (logicPaths.back().compare(_T("..")) == 0)
                    {
                        bNewAddThisFile = true;
                    }
                    else
                    {
                        CloseFile(m_currentWorkingFilePath.c_str());
                        CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory->GetParent();
                        for (int i = 1; i < (int)logicPaths.size() - 1; ++i)
                        {
                            if (logicPaths[i].compare(_T("..")) == 0)
                            {
                                const std::vector<size_t>& fileList = pCurLoopDirectory->GetFileList();
                                for (size_t i = 0; i < fileList.size(); ++i)
                                {
                                    const TString& strFileName = m_pProject->GetComponentFileName(fileList[i]);
                                    CloseFile(strFileName.c_str());
                                }
                                pCurLoopDirectory = pCurLoopDirectory->GetParent();
                            }
                            else
                            {
                                pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                                BEATS_ASSERT(pCurLoopDirectory != NULL);
                                LoadFileFromDirectory(pCurLoopDirectory, &loadedComponents);
                            }
                        }
                    }
                }
            }
        }
    }

    if (bLoadThisFile)
    {
        LoadFile(pFilePath, &loadedComponents);
        if (!bNewAddThisFile)
        {
            m_currentWorkingFilePath.assign(pFilePath);
        }
    }

    ResolveDependency();
    for (size_t i = 0; i < loadedComponents.size(); ++i)
    {
        loadedComponents[i]->Initialize();
    }
    for (size_t i = 0; i < loadedComponents.size(); ++i)
    {
        size_t uComponentId = loadedComponents[i]->GetId();
        bool bIsReference = m_referenceMap.find(uComponentId) != m_referenceMap.end();
        if (!bIsReference)
        {
            loadedComponents[i]->GetHostComponent()->Initialize();
        }
    }

    // Rebuild the m_proxyInCurScene
    m_proxyInCurScene.clear();
    size_t uCurViewFileId = m_pProject->GetComponentFileId(m_currentViewFilePath);
    std::map<size_t, std::vector<size_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
    auto fileToComponentIter = pFileToComponentMap->find(uCurViewFileId);
    if (fileToComponentIter != pFileToComponentMap->end())
    {
        std::vector<size_t>& componentList = fileToComponentIter->second;
        for (size_t i = 0; i < componentList.size(); ++i)
        {
            BEATS_ASSERT(m_proxyInCurScene.find(componentList[i]) == m_proxyInCurScene.end());
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(componentList[i]));
            // If the component is deleted in code, it is possible that proxy is null.
            BEATS_ASSERT(pProxy != NULL || GetComponentTemplate(m_pProject->QueryComponentGuid(componentList[i])) == NULL);
            if (pProxy != NULL)
            {
                m_proxyInCurScene[componentList[i]] = pProxy;
            }
        }
    }
    //TODO: bOpenAsCopy seems useless.
    bOpenAsCopy;
    m_bLoadingFilePhase = bRestoreLoadingPhase;
    m_currentViewFilePath = pFilePath;
}

void CComponentProxyManager::LoadFile(const TCHAR* pszFilePath, std::vector<CComponentProxy*>* pComponentContainer)
{
    bool bRestoreLoadingPhase = m_bLoadingFilePhase;
    m_bLoadingFilePhase = true;
    char tmp[MAX_PATH] = {0};
    CStringHelper::GetInstance()->ConvertToCHAR(pszFilePath, tmp, MAX_PATH);
    TiXmlDocument document(tmp);
    bool loadSuccess = document.LoadFile(TIXML_ENCODING_LEGACY);
    if (loadSuccess)
    {
        size_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(pszFilePath);
        BEATS_ASSERT(uFileId != 0xFFFFFFFF);
        BEATS_ASSERT(m_loadedFiles.find(uFileId) == m_loadedFiles.end());
        m_loadedFiles.insert(uFileId);

        TiXmlElement* pRootElement = document.RootElement();
        TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
        if (pComponentListNode != NULL )
        {
            std::vector<int> reservedId; 
            TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
            while (pComponentElement != NULL && loadSuccess)
            {
                const char* pGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pGuidStr, pszFilePath);
                if (GetComponentTemplate(guid) == NULL)
                {
                    BEATS_ASSERT(false, _T("Can't create component with GUID 0x%x Name %s, Have you removed this component class."), guid, pComponentElement->Attribute("Name"));
                }
                else
                {
                    TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement();
                    while (pInstanceElement != NULL && loadSuccess)
                    {
                        int id = -1;
                        pInstanceElement->Attribute("Id", &id);
                        BEATS_ASSERT(id != -1);
                        CComponentProxy* pComponent = NULL;
                        if (strcmp(pInstanceElement->Value(), "Instance") == 0)
                        {
                            pComponent = (CComponentProxy*)(CreateComponent(guid, false, false, id, false, NULL, false));
                        }
                        else if (strcmp(pInstanceElement->Value(), "Reference") == 0)
                        {
                            int nReferenceId = -1;
                            pInstanceElement->Attribute("ReferenceId", &nReferenceId);
                            pComponent = CreateReference(nReferenceId, guid, id);
                        }
                        pComponent->LoadFromXML(pInstanceElement);
                        if (pComponentContainer != NULL)
                        {
                            pComponentContainer->push_back(pComponent);
                        }
                        pInstanceElement = pInstanceElement->NextSiblingElement();
                    }
                }
                pComponentElement = pComponentElement->NextSiblingElement("Component");
            }
        }
    }
    else
    {
        TCHAR info[MAX_PATH];
        TCHAR reason[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(document.ErrorDesc(), reason, MAX_PATH);
        _stprintf(info, _T("Load file :%s Failed!Reason:%s"), pszFilePath, reason);
        MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
    }
    m_bLoadingFilePhase = bRestoreLoadingPhase;
}

void CComponentProxyManager::LoadFileFromDirectory(CComponentProjectDirectory* pDirectory, std::vector<CComponentProxy*>* pComponentContainer)
{
    const std::vector<size_t>& fileList = pDirectory->GetFileList();
    for (size_t i = 0; i < fileList.size(); ++i)
    {
        const TString& strFileName = m_pProject->GetComponentFileName(fileList[i]);
        LoadFile(strFileName.c_str(), pComponentContainer);
    }
}

void CComponentProxyManager::CloseFile(const TCHAR* pszFilePath)
{
    // TODO: Lock id manager, since we will recycle id in component's uninitialize function.
    // However, we don't want that, so we lock the id manager for now.
    m_pIdManager->Lock();
    BEATS_ASSERT(_tcslen(pszFilePath) > 0, _T("Can't close empty file!"));
    std::vector<CComponentBase*> componentToDelete;
    size_t uFileId = m_pProject->GetComponentFileId(pszFilePath);
    if (m_currentWorkingFilePath.compare(pszFilePath) == 0)
    {
#ifdef _DEBUG
        for (auto iter = m_proxyInCurScene.begin(); iter != m_proxyInCurScene.end(); ++iter)
        {
            BEATS_ASSERT(iter->second->IsInitialized());
        }
#endif
        //Closing current file, just get the m_proxyInCurScene
        for (auto iter = m_proxyInCurScene.begin(); iter != m_proxyInCurScene.end(); ++iter)
        {
            // It is possible that some components are uninitialized by other's uninitialize.
            if (iter->second->IsInitialized())
            {
                CComponentBase* pComponent = iter->second;
                if (iter->second->GetProxyId() == iter->second->GetId())
                {
                    pComponent = iter->second->GetHostComponent();
                }
                pComponent->Uninitialize();
                componentToDelete.push_back(pComponent);
            }
        }
        m_proxyInCurScene.clear();
    }
    else
    {
        // Closing file is not current scene, query id from the static data: m_pProject.
        std::map<size_t, std::vector<size_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
#ifdef _DEBUG
            // All components must exists before they call uninitialize.
            for (size_t i = 0;i < iter->second.size(); ++i)
            {
                size_t uComponentId = iter->second.at(i);
                CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                BEATS_ASSERT(pComponentProxy != NULL && pComponentProxy->IsInitialized());
            }
#endif
            for (size_t i = 0;i < iter->second.size(); ++i)
            {
                size_t uComponentId = iter->second.at(i);
                CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                // This may be null, because some components can be uninitialized by other component's uninitialize.
                if (pComponentProxy != NULL && pComponentProxy->IsInitialized())
                {
                    CComponentBase* pComponent = pComponentProxy;
                    if (pComponentProxy->GetProxyId() == pComponentProxy->GetId())
                    {
                        pComponent = pComponentProxy->GetHostComponent();
                    }
                    pComponent->Uninitialize();
                    componentToDelete.push_back(pComponent);
                }
            }
        }
    }
    for (size_t i = 0; i < componentToDelete.size(); ++i)
    {
        BEATS_SAFE_DELETE(componentToDelete[i]);
    }
    m_loadedFiles.erase(uFileId);
    m_pIdManager->UnLock();
}

const TString& CComponentProxyManager::GetCurrentWorkingFilePath() const
{
    return m_currentWorkingFilePath;
}

const TString& CComponentProxyManager::GetCurrentViewFilePath() const
{
    return m_currentViewFilePath;
}

void CComponentProxyManager::Export(const TCHAR* pSavePath)
{
    m_bExportingPhase = true;
    BEATS_ASSERT(pSavePath != NULL);
    CSerializer serializer;
    serializer << COMPONENT_SYSTEM_VERSION;
    serializer << GetComponentTemplateMap()->size();
    CComponentProjectDirectory* pRootProject = m_pProject->GetRootDirectory();
    pRootProject->Serialize(serializer);
    serializer << m_pProject->GetStartFile();

    size_t uFileCount = m_pProject->GetFileList()->size();
    serializer << uFileCount;
    for (size_t i = 0; i < uFileCount; ++i)
    {
        std::map<size_t, std::vector<size_t> >* pFileToComponent = m_pProject->GetFileToComponentMap();
        std::map<size_t, std::vector<size_t> >::iterator iter = pFileToComponent->find(i);
        BEATS_ASSERT(iter != pFileToComponent->end());
        const std::vector<size_t>& componentsInFile = iter->second;
        size_t uComponentCount = 0;
        size_t uWritePos = serializer.GetWritePos();
        serializer << uWritePos;// File Start pos.
        serializer << uWritePos;// File size placeholder.
        serializer.GetWritePos();
        serializer << uComponentCount;
        if (m_loadedFiles.find(i) != m_loadedFiles.end())
        {
            for (size_t j = 0; j < componentsInFile.size(); ++j)
            {
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(componentsInFile[j]));
                // Don't export component reference
                if (pProxy->GetProxyId() == pProxy->GetId())
                {
                    pProxy->Serialize(serializer, eVT_SavedValue);
                    ++uComponentCount;
                }
            }
        }
        else
        {
            const TString& strFileName = m_pProject->GetFileList()->at(i);
            BEATS_ASSERT(m_pProject->GetComponentFileId(strFileName) == i);
            std::vector<CComponentProxy*> vecComponents;
            CIdManager* pInstanceIdManager = CComponentInstanceManager::GetInstance()->GetIdManager();
            // Don't create instance in LoadFile when exporting.
            BEATS_ASSERT(m_bCreateInstanceWithProxy);
            m_bCreateInstanceWithProxy = false;
            m_pIdManager->Lock();
            LoadFile(strFileName.c_str(), &vecComponents);
            BEATS_ASSERT(m_loadedFiles.find(i) != m_loadedFiles.end());
            ResolveDependency();
            // Do serialize and delete operation in separate steps.
            // Because everything can be ready to serialize after initialize.
            for (size_t j = 0; j < vecComponents.size(); ++j)
            {
                CComponentProxy* pProxy = vecComponents[j];
                pProxy->Initialize();
            }
            for (size_t j = 0; j < vecComponents.size(); ++j)
            {
                CComponentProxy* pProxy = vecComponents[j];
                if (pProxy->GetProxyId() == pProxy->GetId())
                {
                    pProxy->Serialize(serializer, eVT_SavedValue);
                    ++uComponentCount;
                }
            }
            // Don't call CloseFile, because we have nothing to do with proxy's host component.
            for (size_t j = 0; j < vecComponents.size(); ++j)
            {
                CComponentProxy* pProxy = vecComponents[j];
                pProxy->Uninitialize();
            }
            for (size_t j = 0; j < vecComponents.size(); ++j)
            {
                CComponentProxy* pProxy = vecComponents[j];
                BEATS_SAFE_DELETE(pProxy);
            }
            m_loadedFiles.erase(i);
            m_pIdManager->UnLock();
            m_bCreateInstanceWithProxy = true;// Restore.
        }
        size_t uCurWritePos = serializer.GetWritePos();
        size_t uFileDataSize = uCurWritePos - uWritePos;
        serializer.SetWritePos(uWritePos + sizeof(size_t));// Skip file start pos.
        serializer << uFileDataSize; // Exclude file size.
        serializer << uComponentCount;
        serializer.SetWritePos(uCurWritePos);
    }
    serializer.Deserialize(pSavePath);
    m_bExportingPhase = false;
}

void CComponentProxyManager::QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse ) const
{
    result.clear();
    std::map<size_t, std::vector<size_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter != m_pComponentInheritMap->end())
    {
        result = iter->second;
        if (bRecurse)
        {
            for (size_t i = 0; i < iter->second.size(); ++i)
            {
                std::vector<size_t> subResult;
                QueryDerivedClass(iter->second[i], subResult, true);
                for (size_t j = 0; j < subResult.size(); ++j)
                {
                    result.push_back(subResult[j]);
                }
            }
        }
    }
}

void CComponentProxyManager::RegisterClassInheritInfo( size_t uDerivedClassGuid, size_t uBaseClassGuid )
{
    std::map<size_t, std::vector<size_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter == m_pComponentInheritMap->end())
    {
        (*m_pComponentInheritMap)[uBaseClassGuid] = std::vector<size_t>();
        iter = m_pComponentInheritMap->find(uBaseClassGuid);
    }
    BEATS_ASSERT(iter != m_pComponentInheritMap->end());
    iter->second.push_back(uDerivedClassGuid);
}

TString CComponentProxyManager::QueryComponentName(size_t uGuid) const
{
    TString strRet;
    std::map<size_t, TString>::const_iterator iter = m_abstractComponentNameMap.find(uGuid);
    if (iter != m_abstractComponentNameMap.end())
    {
        strRet = iter->second;
    }
    else
    {
        CComponentBase* pComponent = GetComponentTemplate(uGuid);
        if (pComponent != NULL)
        {
            strRet = pComponent->GetClassStr();
        }
    }
    return strRet;
}

void CComponentProxyManager::SaveTemplate(const TCHAR* pszFilePath)
{
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);

    TiXmlElement* pComponentListElement = new TiXmlElement("Components");
    const std::map<size_t, CComponentBase*>* pInstanceMap = GetComponentTemplateMap();
    std::map<size_t, CComponentBase*>::const_iterator iter = pInstanceMap->begin();
    for (; iter != pInstanceMap->end(); ++iter)
    {
        TiXmlElement* pComponentElement = new TiXmlElement("Component");
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%lx", iter->first);
        pComponentElement->SetAttribute("GUID", szGUIDHexStr);
        char tmp[MAX_PATH] = {0};
        CStringHelper::GetInstance()->ConvertToCHAR(GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
        pComponentElement->SetAttribute("Name", tmp);
        static_cast<CComponentProxy*>(iter->second)->SaveToXML(pComponentElement, true);
        // No property is saved, so don't save this template.
        TiXmlElement* pInstanceNode = pComponentElement->FirstChildElement("Instance");
        BEATS_ASSERT(pInstanceNode != NULL);
        if (pInstanceNode->FirstChildElement("VariableNode") == 0)
        {
            BEATS_SAFE_DELETE(pComponentElement);
        }
        else
        {
            pComponentListElement->LinkEndChild(pComponentElement);
        }
    }
    pRootElement->LinkEndChild(pComponentListElement);
    char pathInChar[MAX_PATH] = {0};
    CStringHelper::GetInstance()->ConvertToCHAR(pszFilePath, pathInChar, MAX_PATH);
    document.SaveFile(pathInChar);
}

void CComponentProxyManager::SaveCurFile()
{
    if (!m_currentViewFilePath.empty())
    {
        TiXmlDocument document;
        TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
        document.LinkEndChild(pDeclaration);
        TiXmlElement* pRootElement = new TiXmlElement("Root");
        document.LinkEndChild(pRootElement);

        TiXmlElement* pComponentListElement = new TiXmlElement("Components");
        const std::map<size_t, CComponentProxy*>& componentsInScene = GetComponentsInCurScene();
        std::map<size_t, std::vector<CComponentProxy*>> guidGroup;
        for (std::map<size_t, CComponentProxy*>::const_iterator iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            size_t uGuid = iter->second->GetGuid();
            if (guidGroup.find(uGuid) == guidGroup.end())
            {
                guidGroup[uGuid] = std::vector<CComponentProxy*>();
            }
            guidGroup[uGuid].push_back(iter->second);
        }

        for (std::map<size_t, std::vector<CComponentProxy*>>::iterator iter = guidGroup.begin(); iter != guidGroup.end(); ++iter)
        {
            TiXmlElement* pComponentElement = new TiXmlElement("Component");
            char szGUIDHexStr[32] = {0};
            sprintf(szGUIDHexStr, "0x%lx", iter->first);
            pComponentElement->SetAttribute("GUID", szGUIDHexStr);
            char tmp[MAX_PATH] = {0};
            CStringHelper::GetInstance()->ConvertToCHAR(GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
            pComponentElement->SetAttribute("Name", tmp);
            pComponentListElement->LinkEndChild(pComponentElement);
            for (size_t i = 0; i < iter->second.size(); ++i)
            {
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(iter->second.at(i));
                pProxy->Save();
                pProxy->SaveToXML(pComponentElement, false);
            }
        }
        pRootElement->LinkEndChild(pComponentListElement);
        char pathInChar[MAX_PATH] = {0};
        CStringHelper::GetInstance()->ConvertToCHAR(m_currentViewFilePath.c_str(), pathInChar, MAX_PATH);
        document.SaveFile(pathInChar);

        // Recycle all id, because we will reload it in CComponentProject::ReloadFile
        for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            m_pIdManager->RecycleId(iter->first);
        }
        size_t uFileId = m_pProject->GetComponentFileId(m_currentViewFilePath);
        BEATS_ASSERT(uFileId != 0xFFFFFFFF);
        // We just reload the whole file to keep the id manager is working in the right way.
        m_pProject->ReloadFile(uFileId);
    }
}

void CComponentProxyManager::RegisterPropertyCreator( size_t enumType, TCreatePropertyFunc func )
{
    BEATS_ASSERT(m_pPropertyCreatorMap->find(enumType) == m_pPropertyCreatorMap->end());
    (*m_pPropertyCreatorMap)[enumType] = func;
}

CPropertyDescriptionBase* CComponentProxyManager::CreateProperty( size_t propertyType, CSerializer* serializer )
{
    CPropertyDescriptionBase* pPropertyBase = (*(*m_pPropertyCreatorMap)[propertyType])(serializer);
    return pPropertyBase;
}

void CComponentProxyManager::DeserializeTemplateData(const TCHAR* pszPath,
                                                     const TCHAR* pszEDSFileName,
                                                     const TCHAR* pPatchXMLFileName,
                                                     TCreateComponentEditorProxyFunc func,
                                                     TCreateGraphicFunc pGraphicFunc)
{
    TString szFilePath(pszPath);
    if (szFilePath.back() != _T('\\') && szFilePath.back() != _T('/'))
    {
        szFilePath.append(_T("\\"));
    }
    szFilePath.append(pszEDSFileName);
    bool bFileExists = CFilePathTool::GetInstance()->Exists(szFilePath.c_str());
    BEATS_ASSERT(bFileExists, _T("Can't find %s!\nInitialize failed!"), szFilePath.c_str());
    if (bFileExists)
    {
        // Step 1: Load Info from serialize data comes from our code.
        CSerializer componentsSerializer(szFilePath.c_str());
        const TCHAR* pHeaderStr = COMPONENT_FILE_HEADERSTR;
        TCHAR* pbuff = NULL;
        TCHAR** buff = &pbuff;
        componentsSerializer.Read(buff);
        bool bExamFileHeader = _tcsicmp(pHeaderStr, *buff) == 0;
        BEATS_ASSERT(bExamFileHeader, _T("File format error!"));
        if (bExamFileHeader)
        {
            LoadTemplateDataFromSerializer(componentsSerializer, func, pGraphicFunc);
            // Step 2: Fix the value from XML.
            szFilePath.assign(pszPath);
            if (szFilePath.back() != _T('\\') || szFilePath.back() != _T('/'))
            {
                szFilePath.append(_T("\\"));
            }
            szFilePath.append(pPatchXMLFileName);
            LoadTemplateDataFromXML(szFilePath.c_str());
        }
    }
}

void CComponentProxyManager::ResolveDependency()
{
    for (size_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription != NULL);
        BEATS_ASSERT(pComponentToBeLink != NULL, 
            _T("Component %s id %d can't resolve its dependency %s to component guid 0x%x id %d, have you deleted that component recently?"),
            pDependencyResolver->pDescription->GetOwner()->GetClassStr(), 
            pDependencyResolver->pDescription->GetOwner()->GetId(),
            pDependencyResolver->pDescription->GetDisplayName(),
            pDependencyResolver->uGuid,
            pDependencyResolver->uInstanceId);
        BEATS_ASSERT(pDependencyResolver->pDescription->GetDependencyLine(pDependencyResolver->uIndex)->GetConnectedComponent() == NULL);
        pDependencyResolver->pDescription->SetDependency(pDependencyResolver->uIndex, static_cast<CComponentProxy*>(pComponentToBeLink));
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

CPropertyDescriptionBase* CComponentProxyManager::GetCurrReflectDescription() const
{
    return m_pCurrReflectPropertyDescription;
}

void CComponentProxyManager::SetCurrReflectDescription(CPropertyDescriptionBase* pPropertyDescription)
{
    m_pCurrReflectPropertyDescription = pPropertyDescription;
}

void CComponentProxyManager::SetReflectCheckFlag(bool bFlag)
{
    m_bReflectCheckFlag = bFlag;
}

bool CComponentProxyManager::GetReflectCheckFlag() const
{
    return m_bReflectCheckFlag;
}

CDependencyDescription* CComponentProxyManager::GetCurrReflectDependency() const
{
    return m_pCurrReflectDependency;
}

void CComponentProxyManager::SetCurrReflectDependency(CDependencyDescription* pDependency)
{
    m_pCurrReflectDependency = pDependency;
}

const std::map<size_t, TString>& CComponentProxyManager::GetAbstractComponentNameMap() const
{
    return m_abstractComponentNameMap;
}

bool CComponentProxyManager::IsLoadingFile() const
{
    return m_bLoadingFilePhase;
}

bool CComponentProxyManager::IsParent(size_t uParentGuid, size_t uChildGuid) const
{
    bool bRet = false;
    if(uParentGuid == uChildGuid)
    {
        bRet = true;
    }
    else
    {
        std::vector<size_t> subClassGuids;
        QueryDerivedClass(uParentGuid, subClassGuids, true);
        if(std::find(subClassGuids.begin(), subClassGuids.end(), uChildGuid) != subClassGuids.end())
        {
            bRet = true;
        }
    }
    return bRet;
}

void CComponentProxyManager::RegisterComponentReference(CComponentReference* pReference)
{
    size_t uId = pReference->GetProxyId();
    if (m_referenceIdMap.find(uId) == m_referenceIdMap.end())
    {
        m_referenceIdMap[uId] = std::vector<CComponentReference*>();
    }
    m_referenceIdMap[uId].push_back(pReference);
    BEATS_ASSERT(m_referenceMap.find(pReference->GetId()) == m_referenceMap.end());
    m_referenceMap[pReference->GetId()] = pReference;
}

void CComponentProxyManager::UnregisterComponentReference(CComponentReference* pReference)
{
    size_t uId = pReference->GetProxyId();
    BEATS_ASSERT(m_referenceIdMap.find(uId) != m_referenceIdMap.end());
    std::vector<CComponentReference*>& referenceList = m_referenceIdMap[uId];
    for (auto iter = referenceList.begin(); iter != referenceList.end(); ++iter)
    {
        if (*iter == pReference)
        {
            referenceList.erase(iter);
            break;
        }
    }
    if (referenceList.size() == 0)
    {
        m_referenceIdMap.erase(uId);
    }
    BEATS_ASSERT(m_referenceMap.find(pReference->GetId()) != m_referenceMap.end());
    m_referenceMap.erase(pReference->GetId());
}

const std::map<size_t, std::vector<CComponentReference*>>& CComponentProxyManager::GetReferenceIdMap() const
{
    return m_referenceIdMap;
}

const std::map<size_t, CComponentReference*>& CComponentProxyManager::GetReferenceMap() const
{
    return m_referenceMap;
}

CComponentReference* CComponentProxyManager::CreateReference(size_t uProxyId, size_t uReferenceGuid, size_t uId)
{
    CComponentProxy* pTemplate = static_cast<CComponentProxy*>(GetComponentTemplate(uReferenceGuid));
    BEATS_ASSERT(pTemplate != NULL);
    CComponentReference* pRet = new CComponentReference(uProxyId, uReferenceGuid, pTemplate->GetGraphics()->Clone());
    pRet->SetId(uId == 0xFFFFFFFF ? m_pIdManager->GenerateId() : uId);
    RegisterComponentReference(pRet);
    RegisterInstance(pRet);
    return pRet;
}

const std::map<size_t, CComponentProxy*>& CComponentProxyManager::GetComponentsInCurScene() const
{
    return m_proxyInCurScene;
}

void CComponentProxyManager::OnCreateComponentInScene(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) == m_proxyInCurScene.end());
    m_proxyInCurScene[pProxy->GetId()] = pProxy;
}

void CComponentProxyManager::OnDeleteComponentInScene(CComponentProxy* pProxy)
{
    BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) != m_proxyInCurScene.end());
    bool bIsReference = pProxy->GetId() != pProxy->GetProxyId();
    if (bIsReference)
    {
        CComponentReference* pReference = dynamic_cast<CComponentReference*>(pProxy);
        BEATS_ASSERT(pReference != NULL);
        m_proxyInCurScene.erase(pProxy->GetId());
        pReference->Uninitialize();
        BEATS_SAFE_DELETE(pReference);
    }
    else
    {
        size_t uProxyId = pProxy->GetId();
        std::vector<size_t> allReferenceId;
        std::map<size_t, std::vector<size_t>>* pIdToReferenceMap = m_pProject->GetIdToReferenceMap();
        std::map<size_t, std::vector<size_t>>::iterator idToRefIter = pIdToReferenceMap->find(uProxyId);
        if (idToRefIter != pIdToReferenceMap->end())
        {
            allReferenceId = idToRefIter->second;
        }
        std::map<size_t, std::vector<CComponentReference*>>::iterator iter = m_referenceIdMap.find(uProxyId);
        if (iter != m_referenceIdMap.end())
        {
            for (size_t i = 0; i < iter->second.size(); ++i)
            {
                allReferenceId.push_back(iter->second.at(i)->GetId());
            }
        }
        if (allReferenceId.size() > 0)
        {
            TCHAR szBuffer[10240];
            _stprintf(szBuffer, _T("无法删除该组件，请先删除位于以下位置的%d个引用：\n"), allReferenceId.size());
            TString strInfo;
            strInfo.append(szBuffer);
            for (size_t i = 0; i < allReferenceId.size(); ++i)
            {
                size_t uRefId = allReferenceId[i];
                size_t uFileId = m_pProject->QueryFileId(uRefId, false);
                const TString& strFilePath = m_pProject->GetComponentFileName(uFileId);
                _stprintf(szBuffer, _T("%d. %s\n"), i, strFilePath.c_str());
                strInfo.append(szBuffer);
            }
            MessageBox(NULL, strInfo.c_str(), _T("无法删除"), MB_OK);
        }
        else
        {
            CComponentInstance* pHostComponent = pProxy->GetHostComponent();
            if (pHostComponent != NULL)
            {
                BEATS_ASSERT(m_proxyInCurScene.find(pProxy->GetId()) != m_proxyInCurScene.end())
                    m_proxyInCurScene.erase(pProxy->GetId());
                pHostComponent->Uninitialize();
                BEATS_SAFE_DELETE(pHostComponent);
            }
        }
    }
}

bool CComponentProxyManager::IsExporting() const
{
    return m_bExportingPhase;
}

const std::set<size_t>& CComponentProxyManager::GetLoadedFiles() const
{
    return m_loadedFiles;
}

bool CComponentProxyManager::IsEnableCreateInstanceWithProxy() const
{
    return m_bCreateInstanceWithProxy;
}

void CComponentProxyManager::SetEnableCreateInstanceWithProxy(bool bFlag)
{
    m_bCreateInstanceWithProxy = bFlag;
}

void CComponentProxyManager::LoadTemplateDataFromXML(const TCHAR* pszPath)
{
    char tmp[MAX_PATH] = {0};
    CStringHelper::GetInstance()->ConvertToCHAR(pszPath, tmp, MAX_PATH);
    TiXmlDocument document(tmp);
    bool loadSuccess = document.LoadFile();
    if (loadSuccess)
    {
        TiXmlElement* pRootElement = document.RootElement();
        TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
        if (pComponentListNode != NULL )
        {
            TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
            while (pComponentElement != NULL)
            {
                const char* pGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement("Instance");
                while (pInstanceElement != NULL)
                {
                    int id = -1;
                    pInstanceElement->Attribute("Id", &id);
                    CComponentProxy* pComponent = static_cast<CComponentProxy*>(GetComponentTemplate(guid));
                    if (pComponent != NULL)
                    {
                        pComponent->LoadFromXML(pInstanceElement);
                    }
                    else
                    {
                        BEATS_PRINT(_T("Component (guid 0x%x) doesn't exist, have you deleted it recently?\n"), guid);
                    }
                    pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                }
                pComponentElement = pComponentElement->NextSiblingElement("Component");
            }
        }
    }
}

void CComponentProxyManager::LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc)
{
    size_t version = 0;
    serializer >> version;
    BEATS_ASSERT(version <= COMPONENT_SYSTEM_VERSION);
    size_t componentCount = 0;
    serializer >> componentCount;
    for (size_t i = 0; i < componentCount; ++i)
    {
        bool bIsAbstractClass = false;
        serializer >> bIsAbstractClass;
        size_t curReadPos = serializer.GetReadPos();
        curReadPos;
        size_t totalSize = 0;
        serializer >> totalSize;
        size_t guid = 0;
        serializer >> guid;
        size_t parentGuid = 0;
        serializer >> parentGuid;
        BEATS_ASSERT(guid != 0);
        TCHAR* pStrHolder = NULL;
        TCHAR** ppStrHolder = &pStrHolder;
        serializer.Read(ppStrHolder);
        RegisterClassInheritInfo(guid, parentGuid);
        if (!bIsAbstractClass)
        {
            CComponentProxy* pComponentEditorProxy = func(pGraphicFunc(), guid, parentGuid, pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetDisplayName(pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetCatalogName(pStrHolder);
            BEATS_ASSERT(m_pComponentTemplateMap->find(guid) == m_pComponentTemplateMap->end(), _T("Template component proxy already exists!GUID:0x%x, id:%d"), guid, pComponentEditorProxy->GetId());
            RegisterTemplate(pComponentEditorProxy);
            CComponentInstance* pInstance = (CComponentInstance*)CComponentInstanceManager::GetInstance()->GetComponentTemplate(guid);
            BEATS_ASSERT(pInstance != NULL, _T("Cant find a template instance for a proxy to be its host!GUID:0x%x, id:%d"), guid, pComponentEditorProxy->GetId());
            pComponentEditorProxy->SetTemplateFlag(true);
            pComponentEditorProxy->SetHostComponent(pInstance);
            pComponentEditorProxy->Deserialize(serializer);
        }
        else
        {
            BEATS_ASSERT(pStrHolder != NULL);
            m_abstractComponentNameMap[guid] = TString(pStrHolder);
        }
        BEATS_ASSERT(serializer.GetReadPos() == curReadPos + totalSize);
    }
}
