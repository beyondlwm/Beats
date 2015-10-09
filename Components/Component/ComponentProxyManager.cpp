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
#include "ComponentProjectDirectory.h"
#include "ComponentReference.h"
#include <algorithm>

CComponentProxyManager* CComponentProxyManager::m_pInstance = NULL;

CComponentProxyManager::CComponentProxyManager()
    : m_bCreateInstanceWithProxy(true)
    , m_bLoadingFilePhase(false)
    , m_bExportingPhase(false)
    , m_bReflectCheckFlag(false)
    , m_uCurrViewFileId(0xFFFFFFFF)
    , m_pCurrReflectPropertyDescription(NULL)
    , m_pCurrReflectDependency(NULL)
    , m_pCurrUpdateProxy(NULL)
{
    m_pPropertyCreatorMap = new std::map<uint32_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<uint32_t, std::vector<uint32_t> >();
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
    bool bRestoreLoadingPhase = m_bLoadingFilePhase;
    m_bLoadingFilePhase = true;
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    uint32_t uFileId = pProject->GetComponentFileId(pFilePath);
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
        CalcSwitchFile(uFileId, bCloseLoadedFile, loadFiles, unloadFiles, bNewAddThisFile);
        for (uint32_t i = 0; i < unloadFiles.size(); ++i)
        {
            CloseFile(unloadFiles[i]);
        }
        std::vector<CComponentBase*> loadedComponents;
        for (uint32_t i = 0; i < loadFiles.size(); ++i)
        {
            LoadFile(loadFiles[i], &loadedComponents);
        }
        bool bLoadThisFile = loadFiles.size() > 0 && loadFiles.back() == uFileId;
        if (bLoadThisFile)
        {
            if (!bNewAddThisFile)
            {
                m_uCurLoadFileId = uFileId;
            }
        }
        if (bCloseLoadedFile && loadFiles.size() == 0) // It means we have load uFileId before, and we return to it with closing other loaded file.
        {
            m_uCurLoadFileId = uFileId;
        }
        // Rebuild the m_proxyInCurScene
        m_proxyInCurScene.clear();
        uint32_t uCurViewFileId = m_pProject->GetComponentFileId(pFilePath);
        std::map<uint32_t, std::vector<uint32_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto fileToComponentIter = pFileToComponentMap->find(uCurViewFileId);
        if (fileToComponentIter != pFileToComponentMap->end())
        {
            std::vector<uint32_t>& componentList = fileToComponentIter->second;
            for (uint32_t i = 0; i < componentList.size(); ++i)
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
        for (uint32_t i = 0; i < loadedComponents.size(); ++i)
        {
            uint32_t uComponentId = loadedComponents[i]->GetId();
            bool bIsReference = m_referenceMap.find(uComponentId) != m_referenceMap.end();
            if (!bIsReference)
            {
                static_cast<CComponentProxy*>(loadedComponents[i])->GetHostComponent()->Initialize();
            }
        }
        ReSaveFreshFile();
        m_bLoadingFilePhase = bRestoreLoadingPhase;
        m_uCurrViewFileId = uFileId;
    }
}

void CComponentProxyManager::LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pComponentContainer)
{
    bool bRestoreLoadingPhase = m_bLoadingFilePhase;
    m_bLoadingFilePhase = true;
    const TString& strFilePath = m_pProject->GetComponentFileName(uFileId);
    BEATS_ASSERT(strFilePath.length() > 0);
    TiXmlDocument document(strFilePath.c_str());
    bool loadSuccess = document.LoadFile(TIXML_ENCODING_UTF8);
    if (loadSuccess)
    {
        BEATS_ASSERT(std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId) == m_loadedFiles.end());
        m_loadedFiles.push_back(uFileId);

        TiXmlElement* pRootElement = document.RootElement();
        TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
        if (pComponentListNode != NULL )
        {
            std::vector<CComponentProxy*> loadedProxyList;
            std::vector<int> reservedId; 
            TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
            while (pComponentElement != NULL && loadSuccess)
            {
                const char* pGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                int guid = strtoul(pGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pGuidStr, strFilePath.c_str());
                if (GetComponentTemplate(guid) == NULL)
                {
                    CComponentProxyManager::GetInstance()->GetRefreshFileList().insert(uFileId);
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
                        CComponentProxy* pComponentProxy = NULL;
                        if (strcmp(pInstanceElement->Value(), "Instance") == 0)
                        {
                            pComponentProxy = (CComponentProxy*)(CreateComponent(guid, false, false, id, false, NULL, false));
                        }
                        else if (strcmp(pInstanceElement->Value(), "Reference") == 0)
                        {
                            int nReferenceId = -1;
                            pInstanceElement->Attribute("ReferenceId", &nReferenceId);
                            pComponentProxy = CreateReference(nReferenceId, guid, id);
                        }
                        pComponentProxy->LoadFromXML(pInstanceElement);
                        loadedProxyList.push_back(pComponentProxy);
                        if (pComponentContainer != nullptr)
                        {
                            pComponentContainer->push_back(pComponentProxy);
                        }
                        pInstanceElement = pInstanceElement->NextSiblingElement();
                    }
                }
                pComponentElement = pComponentElement->NextSiblingElement("Component");
            }
            ResolveDependency();
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
                    static_cast<CComponentProxy*>(loadedProxyList[i])->GetHostComponent()->Load();
                }
            }
        }
    }
    else
    {
        TCHAR info[MAX_PATH];
        _stprintf(info, _T("Load file :%s Failed! Row: %d Col: %d Reason:%s"), strFilePath.c_str(), document.ErrorRow(), document.ErrorCol(), document.ErrorDesc());
        MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
    }
    m_bLoadingFilePhase = bRestoreLoadingPhase;
}

void CComponentProxyManager::UnloadFile(uint32_t uFileId, std::vector<CComponentBase*>* pUnloadComponents)
{
    BEATS_ASSERT(pUnloadComponents != nullptr);
    std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId);
    BEATS_WARNING(iterFile != m_loadedFiles.end(), "Close an unopened file %d, this may be right if we are exiting the program.", uFileId);
    if (iterFile != m_loadedFiles.end())
    {
        // query id from the static data: m_pProject.
        std::map<uint32_t, std::vector<uint32_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
            for (uint32_t i = 0; i < iter->second.size(); ++i)
            {
                uint32_t uComponentId = iter->second.at(i);
                CComponentProxy* pComponentProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId));
                BEATS_ASSERT(pComponentProxy != NULL && pComponentProxy->IsLoaded());
                CComponentBase* pComponent = pComponentProxy;
                if (pComponentProxy->GetProxyId() == pComponentProxy->GetId())
                {
                    pComponent = pComponentProxy->GetHostComponent();
                }
                pComponent->Unload();
                pUnloadComponents->push_back(pComponent);
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
    std::vector<CComponentBase*> componentToDelete;
    UnloadFile(uFileId, &componentToDelete);
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        BEATS_ASSERT(componentToDelete[i]->IsInitialized());
        componentToDelete[i]->Uninitialize();
    }
    for (uint32_t i = 0; i < componentToDelete.size(); ++i)
    {
        BEATS_SAFE_DELETE(componentToDelete[i]);
    }
    m_pIdManager->UnLock();
}

const uint32_t CComponentProxyManager::GetCurrentViewFileId() const
{
    return m_uCurrViewFileId;
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

    uint32_t uFileCount = (uint32_t)(m_pProject->GetFileList()->size());
    serializer << uFileCount;
    for (uint32_t i = 0; i < uFileCount; ++i)
    {
        const TString strFileName = CFilePathTool::GetInstance()->FileName(m_pProject->GetComponentFileName(i).c_str());
        serializer << strFileName;
        uint32_t uComponentCount = 0;
        uint32_t uWritePos = serializer.GetWritePos();
        serializer << uWritePos;// File Start pos.
        serializer << 12;// File size placeholder.
        serializer << uComponentCount; // component count place holder
        std::map<uint32_t, std::vector<uint32_t> >* pFileToComponent = m_pProject->GetFileToComponentMap();
        std::map<uint32_t, std::vector<uint32_t> >::iterator iter = pFileToComponent->find(i);
        BEATS_ASSERT(iter != pFileToComponent->end(), _T("File: %s\ndoes not have a component!"), strFileName.c_str());
        if (iter != pFileToComponent->end())
        {
            std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), i);
            if (iterFile != m_loadedFiles.end())
            {
                const std::vector<uint32_t>& componentsInFile = iter->second;
                for (uint32_t j = 0; j < componentsInFile.size(); ++j)
                {
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(componentsInFile[j]));
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
                        uint32_t uGuid = m_pProject->QueryComponentGuid(componentsInFile[j]);
                        BEATS_ASSERT(false, _T("Can't find proxy with GUID 0x%x id %d, have you removed that class in code?"), uGuid, componentsInFile[j]);
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
                // Do serialize and delete operation in separate steps.
                // Because everything can be ready to serialize after initialize.
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    vecComponents[j]->Initialize();
                }
                for (uint32_t j = 0; j < vecComponents.size(); ++j)
                {
                    CComponentProxy* pProxy = static_cast<CComponentProxy*>(vecComponents[j]);
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
    }
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
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);

    TiXmlElement* pComponentListElement = new TiXmlElement("Components");
    const std::map<uint32_t, CComponentBase*>* pInstanceMap = GetComponentTemplateMap();
    std::map<uint32_t, CComponentBase*>::const_iterator iter = pInstanceMap->begin();
    for (; iter != pInstanceMap->end(); ++iter)
    {
        TiXmlElement* pComponentElement = new TiXmlElement("Component");
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%lx", iter->first);
        pComponentElement->SetAttribute("GUID", szGUIDHexStr);
        pComponentElement->SetAttribute("Name", GetComponentTemplate(iter->first)->GetClassStr());
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
    document.SaveFile(pszFilePath);
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
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);

    TiXmlElement* pComponentListElement = new TiXmlElement("Components");
    for (std::map<uint32_t, std::vector<CComponentProxy*>>::iterator iter = components.begin(); iter != components.end(); ++iter)
    {
        TiXmlElement* pComponentElement = new TiXmlElement("Component");
        char szGUIDHexStr[32] = {0};
        sprintf(szGUIDHexStr, "0x%lx", iter->first);
        pComponentElement->SetAttribute("GUID", szGUIDHexStr);
        pComponentElement->SetAttribute("Name", GetComponentTemplate(iter->first)->GetClassStr());
        pComponentListElement->LinkEndChild(pComponentElement);
        for (uint32_t i = 0; i < iter->second.size(); ++i)
        {
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(iter->second.at(i));
            pProxy->Save();
            pProxy->SaveToXML(pComponentElement, false);
        }
    }
    pRootElement->LinkEndChild(pComponentListElement);
    document.SaveFile(pszFileName);
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

bool CComponentProxyManager::IsLoadingFile() const
{
    return m_bLoadingFilePhase;
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
    pRet->SetId(uId == 0xFFFFFFFF ? m_pIdManager->GenerateId() : uId);
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

void CComponentProxyManager::LoadTemplateDataFromXML(const TCHAR* pszPath)
{
    TiXmlDocument document(pszPath);
    bool loadSuccess = document.LoadFile(TIXML_ENCODING_UTF8);
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
        std::map<uint32_t, std::vector<uint32_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto subIter = pFileToComponentMap->find(*iter);
        if (subIter != pFileToComponentMap->end())
        {
            std::map<uint32_t, std::vector<CComponentProxy*>> guidGroup;
            for (uint32_t j = 0; j < subIter->second.size(); ++j)
            {
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(subIter->second.at(j)));
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
            const TString& strFileName = m_pProject->GetComponentFileName(*iter);
            SaveToFile(strFileName.c_str(), guidGroup);
        }
    }
    m_refreshFileList.clear();
}