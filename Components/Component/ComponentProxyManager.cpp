#include "stdafx.h"
#include "ComponentProxyManager.h"
#include "ComponentInstanceManager.h"
#include "ComponentProject.h"
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
#include "ComponentProjectDirectory.h"
#include "ComponentReference.h"
#include <algorithm>

CComponentProxyManager* CComponentProxyManager::m_pInstance = NULL;

CComponentProxyManager::CComponentProxyManager()
    : m_bCreateInstanceWithProxy(true)
    , m_bExportingPhase(false)
    , m_uOperateProgress(0)
    , m_uCurrViewFileId(0xFFFFFFFF)
    , m_pCurrReflectProperty(NULL)
    , m_pCurrReflectDependency(NULL)
    , m_pCurrUpdateProxy(NULL)
    , m_pRemoveChildInfo(nullptr)
    , m_reflectOperateType(EReflectOperationType::ChangeValue)
{
    m_pPropertyCreatorMap = new std::map<uint32_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<uint32_t, std::vector<uint32_t> >();
    m_pComponentBaseClassMap = new std::map<uint32_t, uint32_t>();
    m_pRemoveChildInfo = new CSerializer;
}

CComponentProxyManager::~CComponentProxyManager()
{
    BEATS_SAFE_DELETE(m_pIdManager);
    BEATS_ASSERT(m_pComponentInstanceMap->size() == 0, _T("All proxy should be deleted by instance"));
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);
    BEATS_SAFE_DELETE(m_pDependencyResolver);
    BEATS_SAFE_DELETE(m_pPropertyCreatorMap);
    BEATS_SAFE_DELETE(m_pComponentInheritMap);
    BEATS_SAFE_DELETE(m_pComponentBaseClassMap);
    BEATS_SAFE_DELETE(m_pRemoveChildInfo);
}

void CComponentProxyManager::InitializeAllTemplate()
{
    std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin();
    for (; iter != m_pComponentTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(iter->second != NULL);
        BEATS_ASSERT(dynamic_cast<CComponentProxy*>(iter->second) != NULL, _T("Only proxy template can be initialized!"));
        BEATS_ASSERT(iter->second->IsInitialized() == false, _T("Can't initialize component twice!"));
        iter->second->Initialize();
        BEATS_ASSERT(iter->second->IsInitialized(),
            _T("The initialize flag of component %s is not set after initialize func!"),
            iter->second->GetClassStr());
    }
}

void CComponentProxyManager::UninitializeAllTemplate()
{
    for (std::map<uint32_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin(); iter != m_pComponentTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(iter->second != NULL);
        BEATS_ASSERT(iter->second->IsInitialized() || iter->second->GetGuid() != 1);
        iter->second->Uninitialize();
    }
}

void CComponentProxyManager::OpenFile(const TCHAR* pFilePath, bool bCloseLoadedFile/*= false*/)
{
    m_uOperateProgress = 0;
    uint32_t uFileId = m_pProject->GetComponentFileId(pFilePath);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    if (uFileId != 0xFFFFFFFF)
    {
        // Save cur file.
        if (m_uCurrViewFileId != uFileId)
        {
            SaveCurFile();
        }
        bool bNewAddThisFile = false;
        std::vector<uint32_t> loadFiles;
        std::vector<uint32_t> unloadFiles;
        CalcSwitchFile(uFileId, loadFiles, unloadFiles, bNewAddThisFile);
        if (bCloseLoadedFile)
        {
            // use 10% for close file.
            for (uint32_t i = 0; i < unloadFiles.size(); ++i)
            {
                m_strCurrOperateFile = m_pProject->GetComponentFileName(unloadFiles[i]);
                CloseFile(unloadFiles[i]);
                m_uOperateProgress += (uint32_t)(10.f / unloadFiles.size());
            }
        }
        m_uOperateProgress = 10;
        // use 80% for load file.
        std::vector<CComponentBase*> loadedComponents;
        for (uint32_t i = 0; i < loadFiles.size(); ++i)
        {
            m_strCurrOperateFile = m_pProject->GetComponentFileName(loadFiles[i]);
            LoadFile(loadFiles[i], &loadedComponents);
            m_uOperateProgress += (uint32_t)(80.f / loadFiles.size());
        }
        m_uOperateProgress = 90;
        bool bLoadThisFile = loadFiles.size() > 0 && loadFiles.back() == uFileId;
        if (bLoadThisFile)
        {
            if (!bNewAddThisFile)
            {
                m_uCurrLoadFileId = uFileId;
            }
        }
        if (bCloseLoadedFile && loadFiles.size() == 0) // It means we have load uFileId before, and we return to it with closing other loaded file.
        {
            m_uCurrLoadFileId = uFileId;
        }
        RebuildCurrSceneComponents(uFileId);
        // use 10% for initialize all loaded components.
        for (uint32_t i = 0; i < loadedComponents.size(); ++i)
        {
            uint32_t uComponentId = loadedComponents[i]->GetId();
            bool bIsReference = m_referenceMap.find(uComponentId) != m_referenceMap.end();
            if (!bIsReference)
            {
                static_cast<CComponentProxy*>(loadedComponents[i])->GetHostComponent()->Initialize();
            }
            m_uOperateProgress += (uint32_t)(10.f / loadedComponents.size());
        }
        m_uOperateProgress = 100;
        m_strCurrOperateFile.clear();
        ReSaveFreshFile();
    }
}

void CComponentProxyManager::LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pComponentContainer)
{
    const TString& strFilePath = m_pProject->GetComponentFileName(uFileId);
    BEATS_ASSERT(strFilePath.length() > 0);
    if (CFilePathTool::GetInstance()->Exists(strFilePath.c_str()))
    {
        rapidxml::file<> fdoc(strFilePath.c_str());
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<rapidxml::parse_default>(fdoc.data());
            doc.m_pszFilePath = strFilePath.c_str();
        }
        catch (rapidxml::parse_error &e)
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load file :%s Failed! error :%s"), strFilePath.c_str(), e.what());
            MessageBox(BEYONDENGINE_HWND, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
        BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId) == m_loadedFiles.end());
        m_loadedFiles.push_back(uFileId);

        rapidxml::xml_node<>* pRootElement = doc.first_node("Root");
        rapidxml::xml_node<>* pComponentListNode = pRootElement->first_node("Components");
        if (pComponentListNode != NULL)
        {
            bool bRestoreLoadingPhase = IsInLoadingPhase();
            SetLoadPhaseFlag(true);
            CComponentInstanceManager::GetInstance()->SetLoadPhaseFlag(true);
            std::vector<CComponentProxy*> loadedProxyList;
            std::vector<int> reservedId;
            rapidxml::xml_node<>* pComponentElement = pComponentListNode->first_node("Component");
            while (pComponentElement != NULL)
            {
                const char* pGuidStr = pComponentElement->first_attribute("GUID")->value();
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pGuidStr, strFilePath.c_str());
                if (GetComponentTemplate(guid) == NULL)
                {
                    CComponentProxyManager::GetInstance()->GetRefreshFileList().insert(uFileId);
                    BEATS_ASSERT(false, _T("Can't create component with \nGUID 0x%x\nName %s\nHave you removed this component class?"), guid, pComponentElement->first_attribute("Name")->value());
                }
                else
                {
                    rapidxml::xml_node<>* pInstanceElement = pComponentElement->first_node();
                    while (pInstanceElement != NULL)
                    {
                        int id = -1;
                        id = atoi(pInstanceElement->first_attribute("Id")->value());
                        BEATS_ASSERT(id != -1);
                        CComponentProxy* pComponentProxy = NULL;
                        if (strcmp(pInstanceElement->name(), "Instance") == 0)
                        {
                            pComponentProxy = (CComponentProxy*)(CreateComponent(guid, false, false, id, false, NULL, false));
                        }
                        else if (strcmp(pInstanceElement->name(), "Reference") == 0)
                        {
                            int nReferenceId = -1;
                            nReferenceId = atoi(pInstanceElement->first_attribute("ReferenceId")->value());
                            pComponentProxy = CreateReference(nReferenceId, guid, id);
                        }
                        pComponentProxy->LoadFromXML(pInstanceElement);
                        loadedProxyList.push_back(pComponentProxy);
                        if (pComponentContainer != nullptr)
                        {
                            pComponentContainer->push_back(pComponentProxy);
                        }
                        pInstanceElement = pInstanceElement->next_sibling();
                    }
                }
                pComponentElement = pComponentElement->next_sibling("Component");
            }
            ResolveDependency();
            SetLoadPhaseFlag(bRestoreLoadingPhase);
            CComponentInstanceManager::GetInstance()->SetLoadPhaseFlag(bRestoreLoadingPhase);
            // Call component proxy's initialize means we have sync all value to host component, so we call host component's load function.
            for (size_t i = 0; i < loadedProxyList.size(); ++i)
            {
                loadedProxyList[i]->Initialize();
            }
            for (size_t i = 0; i < loadedProxyList.size(); ++i)
            {
                bool bIsReference = m_referenceMap.find(loadedProxyList[i]->GetId()) != m_referenceMap.end();
                if (!bIsReference)
                {
                    CComponentInstance* pHostComponent = static_cast<CComponentProxy*>(loadedProxyList[i])->GetHostComponent();
                    if (pHostComponent != nullptr)
                    {
                        pHostComponent->Load();
                    }
                    else
                    {
                        BEATS_ASSERT(m_bCreateInstanceWithProxy == false, "Only when m_bCreateInstanceWithProxy is set to false, we can't get the host component");
                    }
                }
            }
        }
    }
}

void CComponentProxyManager::UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents)
{
    BEATS_ASSERT(pUnloadComponents != nullptr);
    std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId);
    BEATS_WARNING(iterFile != m_loadedFiles.end(), "Close an unopened file %d, this may be right if we are exiting the program.", uFileId);
    if (iterFile != m_loadedFiles.end())
    {
        // query id from the static data: m_pProject.
        std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
            for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
            {
                for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                {
                    uint32_t uComponentId = *idIter;
                    CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                    BEATS_ASSERT(pComponentProxy != NULL && pComponentProxy->IsLoaded());
                    pComponentProxy->Unload();
                    if (pComponentProxy->GetProxyId() == pComponentProxy->GetId()) // it is not a reference
                    {
                        pComponentProxy->GetHostComponent()->Unload();
                    }
                    pUnloadComponents->push_back(pComponentProxy);
                }
            }
        }
        m_loadedFiles.erase(iterFile);
    }
}

void CComponentProxyManager::LoadFileFromDirectory(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>* pComponentContainer)
{
    const std::vector<uint32_t>& fileList = pDirectory->GetFileList();
    for (uint32_t i = 0; i < fileList.size(); ++i)
    {
        LoadFile(fileList[i], pComponentContainer);
    }
}

void CComponentProxyManager::CloseFile(uint32_t uFileId)
{
    ReSaveFreshFile(); // the file we are about to close may be in the fresh file list, so we always try to save it before it is closed.
    // TODO: Lock id manager, since we will recycle id in component's un-initialize function.
    // However, we don't want that, so we lock the id manager for now.
    m_pIdManager->Lock();
    std::vector<CComponentBase*> unloadProxyList;
    UnloadFile(uFileId, &unloadProxyList);
    // unloadProxyList contain two types of proxy: CComponentProxy and CComponentReference
    // 1. If it is a CComponentProxy, we need to un-initialize and delete its host component. because CComponentInstance will un-initialize and delete its proxyComponent.
    // 2. If it is a ComponentReference, we need to un-initialize and delete it directly, because it should not bind to any CComponentInstance.
    std::vector<CComponentBase*> componentToDelete;
    for (size_t i = 0; i < unloadProxyList.size(); ++i)
    {
        CComponentProxy* pProxy = static_cast<CComponentProxy*>(unloadProxyList[i]);
        if (pProxy->GetProxyId() == pProxy->GetId())//for those not reference
        {
            componentToDelete.push_back(pProxy->GetHostComponent());
        }
        else
        {
            componentToDelete.push_back(pProxy);
        }
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        CComponentBase* pComponentBase = componentToDelete[i];
        BEATS_ASSERT(pComponentBase->IsInitialized() && !pComponentBase->IsLoaded());
        pComponentBase->Uninitialize();
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        CComponentBase* pComponentBase = componentToDelete[i];
        BEATS_ASSERT(!pComponentBase->IsInitialized());
        BEATS_SAFE_DELETE(pComponentBase);
    }
    m_pIdManager->UnLock();
}

const uint32_t CComponentProxyManager::GetCurrentViewFileId() const
{
    return m_uCurrViewFileId;
}

void CComponentProxyManager::RebuildCurrSceneComponents(uint32_t uCurViewFileId)
{
    // Rebuild the m_proxyInCurScene
    m_proxyInCurScene.clear();
    std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
    auto fileToComponentIter = pFileToComponentMap->find(uCurViewFileId);
    if (fileToComponentIter != pFileToComponentMap->end())
    {
        const std::map<uint32_t, std::set<uint32_t> >& componentList = fileToComponentIter->second;
        for (auto subIter = fileToComponentIter->second.begin(); subIter != fileToComponentIter->second.end(); ++subIter)
        {
            for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
            {
                uint32_t uComponentId = *idIter;
                BEATS_ASSERT(m_proxyInCurScene.find(uComponentId) == m_proxyInCurScene.end());
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                // If the component is deleted in code, it is possible that proxy is null.
                BEATS_ASSERT(pProxy != NULL || GetComponentTemplate(m_pProject->QueryComponentGuid(uComponentId)) == NULL);
                if (pProxy != NULL)
                {
                    m_proxyInCurScene[uComponentId] = pProxy;
                }
            }
        }
    }
    m_uCurrViewFileId = uCurViewFileId;
}

void CComponentProxyManager::Export(const TCHAR* pSavePath)
{
    m_bExportingPhase = true;
    BEATS_ASSERT(pSavePath != NULL);
    static CSerializer serializer;
    serializer.Reset();
    serializer << COMPONENT_SYSTEM_VERSION;
    serializer << GetComponentTemplateMap()->size();
    CComponentProjectDirectory* pRootProject = m_pProject->GetRootDirectory();
    pRootProject->Serialize(serializer);
    serializer << m_pProject->GetStartFile();

    m_uOperateProgress = 0;
    uint32_t uFileCount = (uint32_t)(m_pProject->GetFileList()->size());
    serializer << uFileCount;
    for (uint32_t i = 0; i < uFileCount; ++i)
    {
        const TString strFileName = CFilePathTool::GetInstance()->FileName(m_pProject->GetComponentFileName(i).c_str());
        m_strCurrOperateFile = strFileName;
        serializer << strFileName;
        uint32_t uComponentCount = 0;
        uint32_t uWritePos = serializer.GetWritePos();
        serializer << uWritePos;// File Start pos.
        serializer << 12;// File size placeholder.
        serializer << uComponentCount; // component count place holder
        std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponent = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponent->find(i);
        BEATS_ASSERT(iter != pFileToComponent->end(), _T("File: %s\ndoes not have a component!"), strFileName.c_str());
        if (iter != pFileToComponent->end())
        {
            std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
            if (iterFile != m_loadedFiles.end())
            {
                for (auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
                {
                    for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                    {
                        uint32_t uComponentId = *idIter;
                        CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                        if (pProxy)
                        {
                            // Don't export component reference
                            if (pProxy->GetProxyId() == pProxy->GetId())
                            {
                                pProxy->ExportDataToHost(serializer, eVT_SavedValue);
                                ++uComponentCount;
                            }
                        }
                        else
                        {
                            uint32_t uGuid = m_pProject->QueryComponentGuid(uComponentId);
                            BEATS_ASSERT(false, _T("Can't find proxy with GUID 0x%x id %d, have you removed that class in code?"), uGuid, uComponentId);
                        }
                    }
                }
            }
            else
            {
                std::vector<CComponentBase*> vecComponents;
                CIdManager* pInstanceIdManager = CComponentInstanceManager::GetInstance()->GetIdManager();
                // Don't create instance in LoadFile when exporting.
                BEATS_ASSERT(m_bCreateInstanceWithProxy);
                m_bCreateInstanceWithProxy = false;
                m_pIdManager->Lock();
                LoadFile(i, &vecComponents);
                iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
                BEATS_ASSERT(iterFile != m_loadedFiles.end(), _T("Load file index %d failed!"), i);
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(vecComponents[j]);
                    BEATS_ASSERT(pProxy->IsInitialized());
                    if (pProxy->GetProxyId() == pProxy->GetId())
                    {
                        pProxy->ExportDataToHost(serializer, eVT_SavedValue);
                        ++uComponentCount;
                    }
                }
                ReSaveFreshFile();
                // Don't call CloseFile, because we have nothing to do with proxy's host component.
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    vecComponents[j]->Uninitialize();
                }
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    BEATS_SAFE_DELETE(vecComponents[j]);
                }
                m_loadedFiles.erase(iterFile);
                m_pIdManager->UnLock();
                m_bCreateInstanceWithProxy = true;// Restore.
            }
            uint32_t uCurWritePos = serializer.GetWritePos();
            uint32_t uFileDataSize = uCurWritePos - uWritePos;
            serializer.SetWritePos(uWritePos + sizeof(uint32_t));// Skip file start pos.
            serializer << uFileDataSize;
            serializer << uComponentCount;
            serializer.SetWritePos(uCurWritePos);
        }
        m_uOperateProgress = uint32_t((i + 1) * 100.f / uFileCount);
    }
    BEATS_ASSERT(m_uOperateProgress == 100);
    m_strCurrOperateFile.clear();
    serializer.Deserialize(pSavePath);
    m_bExportingPhase = false;
}

void CComponentProxyManager::QueryDerivedClass(uint32_t uBaseClassGuid, std::vector<uint32_t>& result, bool bRecurse ) const
{
    result.clear();
    std::map<uint32_t, std::vector<uint32_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter != m_pComponentInheritMap->end())
    {
        result = iter->second;
        if (bRecurse)
        {
            for (uint32_t i = 0; i < iter->second.size(); ++i)
            {
                std::vector<uint32_t> subResult;
                QueryDerivedClass(iter->second[i], subResult, true);
                for (uint32_t j = 0; j < subResult.size(); ++j)
                {
                    result.push_back(subResult[j]);
                }
            }
        }
    }
}

uint32_t CComponentProxyManager::QueryBaseClase(uint32_t uGuid) const
{
    uint32_t uRet = 0xFFFFFFFF;
    auto iter = m_pComponentBaseClassMap->find(uGuid);
    if (iter != m_pComponentBaseClassMap->end())
    {
        uRet = iter->second;
    }
    return uRet;
}

void CComponentProxyManager::RegisterClassInheritInfo( uint32_t uDerivedClassGuid, uint32_t uBaseClassGuid )
{
    std::map<uint32_t, std::vector<uint32_t> >::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
    if (iter == m_pComponentInheritMap->end())
    {
        (*m_pComponentInheritMap)[uBaseClassGuid] = std::vector<uint32_t>();
        iter = m_pComponentInheritMap->find(uBaseClassGuid);
    }
    BEATS_ASSERT(iter != m_pComponentInheritMap->end());
    iter->second.push_back(uDerivedClassGuid);
    BEATS_ASSERT(m_pComponentBaseClassMap->find(uDerivedClassGuid) == m_pComponentBaseClassMap->end());
    (*m_pComponentBaseClassMap)[uDerivedClassGuid] = uBaseClassGuid;
}

TString CComponentProxyManager::QueryComponentName(uint32_t uGuid) const
{
    TString strRet;
    std::map<uint32_t, TString>::const_iterator iter = m_abstractComponentNameMap.find(uGuid);
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
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pDecl = doc.allocate_node(rapidxml::node_declaration);
    rapidxml::xml_attribute<>* pDecl_ver = doc.allocate_attribute("version", "1.0");
    pDecl->append_attribute(pDecl_ver);
    doc.append_node(pDecl);
    rapidxml::xml_node<>* pRootElement = doc.allocate_node(rapidxml::node_element, "Root");
    doc.append_node(pRootElement);

    rapidxml::xml_node<>* pComponentListElement = doc.allocate_node(rapidxml::node_element, "Components");
    pRootElement->append_node(pComponentListElement);
    const std::map<uint32_t, CComponentBase*>* pInstanceMap = GetComponentTemplateMap();
    std::map<uint32_t, CComponentBase*>::const_iterator iter = pInstanceMap->begin();
    for (; iter != pInstanceMap->end(); ++iter)
    {
        rapidxml::xml_node<>* pComponentElement = doc.allocate_node(rapidxml::node_element, "Component");
        pComponentListElement->append_node(pComponentElement);
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%lx", iter->first);
        pComponentElement->append_attribute(doc.allocate_attribute("GUID", doc.allocate_string(szGUIDHexStr)));
        pComponentElement->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(GetComponentTemplate(iter->first)->GetClassStr())));
        static_cast<CComponentProxy*>(iter->second)->SaveToXML(pComponentElement, true);
        // No property is saved, so don't save this template.
        rapidxml::xml_node<>* pInstanceNode = pComponentElement->first_node("Instance");
        BEATS_ASSERT(pInstanceNode != NULL);
        if (pInstanceNode->first_node("VariableNode") == 0)
        {
            pComponentListElement->remove_node(pComponentElement);
        }
    }
    TString strOut;
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    rapidxml::print(std::back_inserter(strOut), doc, 0);
#endif
    std::ofstream out(pszFilePath);
    out << strOut;
    out.close();
}

void CComponentProxyManager::SaveCurFile()
{
    if (m_uCurrViewFileId != 0xFFFFFFFF)
    {
        const std::map<uint32_t, CComponentProxy*>& componentsInScene = GetComponentsInCurScene();
        std::map<uint32_t, std::vector<CComponentProxy*>> guidGroup;
        for (std::map<uint32_t, CComponentProxy*>::const_iterator iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            uint32_t uGuid = iter->second->GetGuid();
            if (guidGroup.find(uGuid) == guidGroup.end())
            {
                guidGroup[uGuid] = std::vector<CComponentProxy*>();
            }
            guidGroup[uGuid].push_back(iter->second);
        }

        const TString& strComponentFileName = m_pProject->GetComponentFileName(m_uCurrViewFileId);
        BEATS_ASSERT(!strComponentFileName.empty());
        SaveToFile(strComponentFileName.c_str(), guidGroup);

        // Recycle all id, because we will reload it in CComponentProject::ReloadFile
        for (auto iter = componentsInScene.begin(); iter != componentsInScene.end(); ++iter)
        {
            m_pIdManager->RecycleId(iter->first);
        }
        // We just reload the whole file to keep the id manager is working in the right way.
        m_pProject->ReloadFile(m_uCurrViewFileId);
    }
}

void CComponentProxyManager::SaveToFile(const TCHAR* pszFileName, std::map<uint32_t, std::vector<CComponentProxy*>>& components)
{
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pDecl = doc.allocate_node(rapidxml::node_declaration);
    rapidxml::xml_attribute<>* pDecl_ver = doc.allocate_attribute("version", "1.0");
    pDecl->append_attribute(pDecl_ver);
    doc.append_node(pDecl);
    rapidxml::xml_node<>* pRootElement = doc.allocate_node(rapidxml::node_element, "Root");
    doc.append_node(pRootElement);

    rapidxml::xml_node<>* pComponentListElement = doc.allocate_node(rapidxml::node_element, "Components");
    pRootElement->append_node(pComponentListElement);
    for (std::map<uint32_t, std::vector<CComponentProxy*>>::iterator iter = components.begin(); iter != components.end(); ++iter)
    {
        rapidxml::xml_node<>* pComponentElement = doc.allocate_node(rapidxml::node_element, "Component");
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%lx", iter->first);
        pComponentElement->append_attribute(doc.allocate_attribute("GUID", doc.allocate_string(szGUIDHexStr)));
        pComponentElement->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(GetComponentTemplate(iter->first)->GetClassStr())));
        pComponentListElement->append_node(pComponentElement);
        for (uint32_t i = 0; i < iter->second.size(); ++i)
        {
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(iter->second.at(i));
            pProxy->Save();
            pProxy->SaveToXML(pComponentElement, false);
        }
    }

    TString strOut;
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    rapidxml::print(std::back_inserter(strOut), doc, 0);
#endif
    std::ofstream out(pszFileName);
    out << strOut;
    out.close();
}

void CComponentProxyManager::RegisterPropertyCreator( uint32_t enumType, TCreatePropertyFunc func )
{
    BEATS_ASSERT(m_pPropertyCreatorMap->find(enumType) == m_pPropertyCreatorMap->end());
    (*m_pPropertyCreatorMap)[enumType] = func;
}

CPropertyDescriptionBase* CComponentProxyManager::CreateProperty( uint32_t propertyType, CSerializer* serializer )
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
    for (uint32_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription != NULL);
        BEATS_ASSERT(pDependencyResolver->pVariableAddress == NULL, "pVariableAddress should be null in proxy mode");
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

CPropertyDescriptionBase* CComponentProxyManager::GetCurrReflectProperty(EReflectOperationType* pOperateType) const
{
    if (pOperateType != nullptr)
    {
        *pOperateType = m_reflectOperateType;
    }
    // When we are in clone state or loading file state, we don't allow to reflect sync single property
    return !CComponentInstanceManager::GetInstance()->IsInClonePhase() && !IsInLoadingPhase() ? m_pCurrReflectProperty : nullptr;
}

void CComponentProxyManager::SetCurrReflectProperty(CPropertyDescriptionBase* pPropertyDescription, EReflectOperationType operateType)
{
    BEATS_ASSERT(pPropertyDescription == nullptr || m_pCurrReflectDependency == nullptr, "Reflect property and dependency can't both be set value");
    m_pCurrReflectProperty = pPropertyDescription;
    m_reflectOperateType = operateType;
}

CSerializer& CComponentProxyManager::GetRemoveChildInfo()
{
    BEATS_ASSERT(m_pCurrReflectProperty != nullptr && m_pCurrReflectProperty->IsContainerProperty());
    return *m_pRemoveChildInfo;
}

CDependencyDescription* CComponentProxyManager::GetCurrReflectDependency() const
{
    // When we are in clone state or loading file state, we don't allow to reflect sync single dependency
    return !CComponentInstanceManager::GetInstance()->IsInClonePhase() && !IsInLoadingPhase() ? m_pCurrReflectDependency : nullptr;
}

void CComponentProxyManager::SetCurrReflectDependency(CDependencyDescription* pDependency)
{
    BEATS_ASSERT(pDependency == nullptr || m_pCurrReflectProperty == nullptr, "Reflect property and dependency can't both be set value");
    m_pCurrReflectDependency = pDependency;
}

CComponentProxy* CComponentProxyManager::GetCurrUpdateProxy() const
{
    return m_pCurrUpdateProxy;
}

void CComponentProxyManager::SetCurrUpdateProxy(CComponentProxy* pProxy)
{
    m_pCurrUpdateProxy = pProxy;
}

const std::map<uint32_t, TString>& CComponentProxyManager::GetAbstractComponentNameMap() const
{
    return m_abstractComponentNameMap;
}

bool CComponentProxyManager::IsParent(uint32_t uParentGuid, uint32_t uChildGuid) const
{
    bool bRet = false;
    if(uParentGuid == uChildGuid)
    {
        bRet = true;
    }
    else
    {
        std::vector<uint32_t> subClassGuids;
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
    uint32_t uId = pReference->GetProxyId();
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
    uint32_t uId = pReference->GetProxyId();
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

const std::map<uint32_t, std::vector<CComponentReference*>>& CComponentProxyManager::GetReferenceIdMap() const
{
    return m_referenceIdMap;
}

const std::map<uint32_t, CComponentReference*>& CComponentProxyManager::GetReferenceMap() const
{
    return m_referenceMap;
}

CComponentReference* CComponentProxyManager::CreateReference(uint32_t uProxyId, uint32_t uReferenceGuid, uint32_t uId)
{
    CComponentProxy* pTemplate = static_cast<CComponentProxy*>(GetComponentTemplate(uReferenceGuid));
    BEATS_ASSERT(pTemplate != NULL);
    CComponentReference* pRet = new CComponentReference(uProxyId, uReferenceGuid, pTemplate->GetGraphics()->Clone());
    if (uId == 0xFFFFFFFF)
    {
        uId = m_pIdManager->GenerateId();
    }
    else
    {
        m_pIdManager->ReserveId(uId, false);
    }
    pRet->SetId(uId);
    RegisterComponentReference(pRet);
    RegisterInstance(pRet);
    return pRet;
}

const std::map<uint32_t, CComponentProxy*>& CComponentProxyManager::GetComponentsInCurScene() const
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
        uint32_t uProxyId = pProxy->GetId();
        // Get the static info of reference.
        std::map<uint32_t, std::set<uint32_t>>* pIdToReferenceMap = m_pProject->GetIdToReferenceMap();
        std::map<uint32_t, std::set<uint32_t>>::iterator idToRefIter = pIdToReferenceMap->find(uProxyId);
        std::set<uint32_t> allReferenceIdSet;
        if (idToRefIter != pIdToReferenceMap->end())
        {
            allReferenceIdSet = idToRefIter->second;
        }
        // Add the dynamic info of reference.
        std::map<uint32_t, std::vector<CComponentReference*>>::iterator iter = m_referenceIdMap.find(uProxyId);
        if (iter != m_referenceIdMap.end())
        {
            for (uint32_t i = 0; i < iter->second.size(); ++i)
            {
                BEATS_ASSERT(allReferenceIdSet.find(iter->second.at(i)->GetId()) == allReferenceIdSet.end());
                allReferenceIdSet.insert(iter->second.at(i)->GetId());
            }
        }
        if (allReferenceIdSet.size() > 0)
        {
            TCHAR szBuffer[10240];
            _stprintf(szBuffer, _T("无法删除该组件，请先删除位于以下位置的%d个引用：\n"), allReferenceIdSet.size());
            TString strInfo;
            strInfo.append(szBuffer);
            int nCounter = 0;
            for (auto iter = allReferenceIdSet.begin(); iter != allReferenceIdSet.end(); ++iter)
            {
                uint32_t uRefId = *iter;
                uint32_t uFileId = m_pProject->QueryFileId(uRefId, false);
                const TString& strFilePath = m_pProject->GetComponentFileName(uFileId);
                _stprintf(szBuffer, _T("%d. %s\n"), nCounter++, strFilePath.c_str());
                strInfo.append(szBuffer);
            }
            MessageBox(BEYONDENGINE_HWND, strInfo.c_str(), _T("无法删除"), MB_OK);
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

bool CComponentProxyManager::IsEnableCreateInstanceWithProxy() const
{
    return m_bCreateInstanceWithProxy;
}

void CComponentProxyManager::SetEnableCreateInstanceWithProxy(bool bFlag)
{
    m_bCreateInstanceWithProxy = bFlag;
}

std::set<uint32_t>& CComponentProxyManager::GetRefreshFileList()
{
    return m_refreshFileList;
}

uint32_t CComponentProxyManager::GetOperateProgress(TString& strCurrOperateFile) const
{
    strCurrOperateFile = m_strCurrOperateFile;
    return m_uOperateProgress;
}

static void CollectPropertyInvokeGuid(CPropertyDescriptionBase* pPropertyDescription, std::set<uint32_t>& invokedList)
{
    if (pPropertyDescription->GetType() == eRPT_Ptr)
    {
        uint32_t uReflectGuid = pPropertyDescription->HACK_GetPtrReflectGuid();
        BEATS_ASSERT(uReflectGuid != 0);
        invokedList.insert(uReflectGuid);
    }
    for (size_t i = 0; i < pPropertyDescription->GetChildren().size(); ++i)
    {
        CollectPropertyInvokeGuid(pPropertyDescription->GetChildren().at(i), invokedList);
    }
}

static void CollectInvokeGuid(CComponentProxy* pProxy, std::set<uint32_t>& invokedGuidList)
{
    BEATS_ASSERT(pProxy != nullptr);
    if (pProxy->GetProxyId() == pProxy->GetId())//It's not a reference.
    {
        invokedGuidList.insert(pProxy->GetGuid());
        for (size_t k = 0; k < pProxy->GetPropertyPool()->size(); ++k)
        {
            CPropertyDescriptionBase* pProperty = pProxy->GetPropertyPool()->at(k);
            CollectPropertyInvokeGuid(pProperty, invokedGuidList);
        }
        for (size_t k = 0; k < pProxy->GetDependencies()->size(); ++k)
        {
            CDependencyDescription* pDependency = pProxy->GetDependency(k);
            invokedGuidList.insert(pDependency->GetDependencyGuid());
        }
    }
}

void CComponentProxyManager::CheckForUnInvokedGuid(std::set<uint32_t>& uninvokeGuidList)
{
    m_bExportingPhase = true;
    uninvokeGuidList.clear();
    std::set<uint32_t> invokedGuidList;
    uint32_t uFileCount = (uint32_t)(m_pProject->GetFileList()->size());
    for (uint32_t i = 0; i < uFileCount; ++i)
    {
        const TString strFileName = CFilePathTool::GetInstance()->FileName(m_pProject->GetComponentFileName(i).c_str());
        uint32_t uComponentCount = 0;
        std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponent = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponent->find(i);
        BEATS_ASSERT(iter != pFileToComponent->end(), _T("File: %s\ndoes not have a component!"), strFileName.c_str());
        if (iter != pFileToComponent->end())
        {
            std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
            if (iterFile != m_loadedFiles.end())
            {
                const std::map<uint32_t, std::set<uint32_t> >& componentsInFile = iter->second;
                for (auto subIter = componentsInFile.begin(); subIter != componentsInFile.end(); ++subIter)
                {
                    for (auto idIter = subIter->second.begin(); idIter != subIter->second.end(); ++idIter)
                    {
                        uint32_t uComponentId = *idIter;
                        CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                        BEATS_ASSERT(pProxy != nullptr, _T("Can't find proxy with GUID 0x%x id %d, have you removed that class in code?"), m_pProject->QueryComponentGuid(uComponentId), uComponentId);
                        if (pProxy)
                        {
                            CollectInvokeGuid(pProxy, invokedGuidList);
                        }
                    }
                }
            }
            else
            {
                std::vector<CComponentBase*> vecComponents;
                CIdManager* pInstanceIdManager = CComponentInstanceManager::GetInstance()->GetIdManager();
                // Don't create instance in LoadFile when exporting.
                BEATS_ASSERT(m_bCreateInstanceWithProxy);
                m_bCreateInstanceWithProxy = false;
                m_pIdManager->Lock();
                LoadFile(i, &vecComponents);
                BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i) != m_loadedFiles.end(), _T("Load file index %d failed!"), i);
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(vecComponents[j]);
                    BEATS_ASSERT(pProxy->IsInitialized());
                    CollectInvokeGuid(pProxy, invokedGuidList);
                }
                ReSaveFreshFile();
                // Don't call CloseFile, because we have nothing to do with proxy's host component.
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    vecComponents[j]->Uninitialize();
                }
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    BEATS_SAFE_DELETE(vecComponents[j]);
                }
                m_loadedFiles.erase(iterFile);
                m_pIdManager->UnLock();
                m_bCreateInstanceWithProxy = true;// Restore.
            }
        }
    }
    const std::map<uint32_t, CComponentBase*>* pTemplateMap = GetComponentTemplateMap();
    for (auto iter = pTemplateMap->begin(); iter != pTemplateMap->end(); ++iter)
    {
        BEATS_ASSERT(uninvokeGuidList.find(iter->first) == uninvokeGuidList.end());
        uninvokeGuidList.insert(iter->first);
    }
    for (auto iter = m_abstractComponentNameMap.begin(); iter != m_abstractComponentNameMap.end(); ++iter)
    {
        BEATS_ASSERT(uninvokeGuidList.find(iter->first) == uninvokeGuidList.end());
        uninvokeGuidList.insert(iter->first);
    }
    for (auto iter = invokedGuidList.begin(); iter != invokedGuidList.end(); ++iter)
    {
        uint32_t uGuid = *iter;
        uninvokeGuidList.erase(uGuid);
        uint32_t uBaseClassGuid = QueryBaseClase(uGuid);
        while (uBaseClassGuid != 0xFFFFFFFF)
        {
            uninvokeGuidList.erase(uBaseClassGuid);
            uBaseClassGuid = QueryBaseClase(uBaseClassGuid);
        }
    }
    m_bExportingPhase = false;
}

void CComponentProxyManager::LoadTemplateDataFromXML(const TCHAR* pszPath)
{
    if (CFilePathTool::GetInstance()->Exists(pszPath))
    {
        rapidxml::file<> fdoc(pszPath);
        rapidxml::xml_document<> doc;
        try
        {
            doc.parse<rapidxml::parse_default>(fdoc.data());
        }
        catch (rapidxml::parse_error &e)
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load File :%s Failed! error :%s"), pszPath, e.what());
            MessageBox(BEYONDENGINE_HWND, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
        rapidxml::xml_node<>* pRootElement = doc.first_node("Root");
        rapidxml::xml_node<>* pComponentListNode = pRootElement->first_node("Components");
        if (pComponentListNode != NULL)
        {
            rapidxml::xml_node<>* pComponentElement = pComponentListNode->first_node("Component");
            while (pComponentElement != NULL)
            {
                const char* pGuidStr = pComponentElement->first_attribute("GUID")->value();
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                rapidxml::xml_node<>* pInstanceElement = pComponentElement->first_node("Instance");
                while (pInstanceElement != NULL)
                {
                    int id = -1;
                    id = atoi(pInstanceElement->first_node("Id")->value());
                    CComponentProxy* pComponent = static_cast<CComponentProxy*>(GetComponentTemplate(guid));
                    if (pComponent != NULL)
                    {
                        pComponent->LoadFromXML(pInstanceElement);
                    }
                    else
                    {
                        BEATS_PRINT(_T("Component (guid 0x%x) doesn't exist, have you deleted it recently?\n"), guid);
                    }
                    pInstanceElement = pInstanceElement->next_sibling("Instance");
                }
                pComponentElement = pComponentElement->next_sibling("Component");
            }
        }
    }
}

void CComponentProxyManager::LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc)
{
    uint32_t version = 0;
    serializer >> version;
    BEATS_ASSERT(version <= COMPONENT_SYSTEM_VERSION);
    uint32_t componentCount = 0;
    serializer >> componentCount;
    for (uint32_t i = 0; i < componentCount; ++i)
    {
        bool bIsAbstractClass = false;
        serializer >> bIsAbstractClass;
        uint32_t curReadPos = serializer.GetReadPos();
        curReadPos;
        uint32_t totalSize = 0;
        serializer >> totalSize;
        uint32_t guid = 0;
        serializer >> guid;
        uint32_t parentGuid = 0;
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
            pComponentEditorProxy->ImportDataFromEDS(serializer);
        }
        else
        {
            BEATS_ASSERT(pStrHolder != NULL);
            m_abstractComponentNameMap[guid] = TString(pStrHolder);
        }
        BEATS_ASSERT(serializer.GetReadPos() == curReadPos + totalSize);
    }
}

void CComponentProxyManager::ReSaveFreshFile()
{
    for (auto iter = m_refreshFileList.begin(); iter != m_refreshFileList.end(); ++iter)
    {
        std::map<uint32_t, std::map<uint32_t, std::set<uint32_t> > >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto subIter = pFileToComponentMap->find(*iter);
        if (subIter != pFileToComponentMap->end())
        {
            std::map<uint32_t, std::vector<CComponentProxy*>> guidGroup;
            for (auto componentMapIter = subIter->second.begin(); componentMapIter != subIter->second.end(); ++componentMapIter)
            {
                for (auto idIter = componentMapIter->second.begin(); idIter != componentMapIter->second.end(); ++idIter)
                {
                    uint32_t uComponentId = *idIter;
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                    if (pProxy != NULL) // The proxy may be null when you remove some components in code.
                    {
                        std::map<uint32_t, std::vector<CComponentProxy*>>::iterator guidGroupIter = guidGroup.find(pProxy->GetGuid());
                        if (guidGroupIter == guidGroup.end())
                        {
                            guidGroup[pProxy->GetGuid()] = std::vector<CComponentProxy*>();
                            guidGroupIter = guidGroup.find(pProxy->GetGuid());
                        }
                        guidGroupIter->second.push_back(pProxy);
                    }
                }
            }
            const TString& strFileName = m_pProject->GetComponentFileName(*iter);
            SaveToFile(strFileName.c_str(), guidGroup);
        }
    }
    m_refreshFileList.clear();
}