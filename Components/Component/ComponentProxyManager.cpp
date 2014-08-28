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
    : m_bLoadingFilePhase(false)
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
    // 2. File is at the same directory: close current file and open it.
    // 3. File is in the son directory: don't close current, load rest files of directory and go on.
    // 4. File is in other different directory: close current and change directory.
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    size_t uFileId = pProject->GetComponentFileId(pFilePath);
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    // 1. File is in the parent directory (loaded before): just change the content of m_proxyInCurScene
    if (m_loadedFiles.find(uFileId) != m_loadedFiles.end())
    {
        m_proxyInCurScene.clear();
        std::map<size_t, std::vector<size_t> >* pComponentMap = pProject->GetFileToComponentMap();
        auto iter = pComponentMap->find(uFileId);
        BEATS_ASSERT(iter != pComponentMap->end());
        for (size_t i = 0; i < iter->second.size(); ++i)
        {
            size_t uId = iter->second.at(i);
            CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uId));
            BEATS_ASSERT(_T("Get Proxy failed with id %d"), uId);
            m_proxyInCurScene[pProxy->GetId()] = pProxy;
        }
    }
    else
    {
        m_bLoadingFilePhase = true;
        CComponentProjectDirectory* pDirectory = m_pProject->FindProjectDirectory(pFilePath, true);
        CComponentProjectDirectory* pCurDirectory = m_pProject->GetRootDirectory();
        BEATS_ASSERT(pDirectory != NULL);
        std::vector<TString> logicPaths;
        TString strLogicPath;
        bool bIgnoreRoot = false;
        if (m_currentWorkingFilePath.empty())
        {
            strLogicPath = pDirectory->GenerateLogicPath();
            bIgnoreRoot = true;
        }
        else
        {
            pCurDirectory = m_pProject->FindProjectDirectory(m_currentWorkingFilePath, true);
            BEATS_ASSERT(pCurDirectory != NULL);
            strLogicPath = pDirectory->MakeRelativeLogicPath(pCurDirectory);
            CloseFile(m_currentWorkingFilePath.c_str(), true);
        }
        CStringHelper::GetInstance()->SplitString(strLogicPath.c_str(), _T("/"), logicPaths);
        size_t i = bIgnoreRoot ? 1 : 0;
        std::vector<CComponentProxy*> loadedComponents;
        // Don't load last directory, because we only load one file of it.
        for (; i < logicPaths.size() - 1; ++i)
        {
            if (logicPaths[i].compare(_T("..")) == 0)
            {
                // If i == 0, the first go-up is finshed by close the current file.
                if (i != 0)
                {
                    const std::vector<size_t>& fileList = pCurDirectory->GetParent()->GetFileList();
                    for (size_t i = 0; i < fileList.size(); ++i)
                    {
                        const TString& strFileName = m_pProject->GetComponentFileName(fileList[i]);
                        CloseFile(strFileName.c_str(), false);
                    }
                    pCurDirectory = pCurDirectory->GetParent();
                }
            }
            else
            {
                CComponentProjectDirectory* pDirectory = pCurDirectory->FindChild(logicPaths[i].c_str());
                BEATS_ASSERT(pDirectory != NULL);
                LoadFileFromDirectory(pDirectory, &loadedComponents);
                pCurDirectory = pDirectory;
            }
        }
        LoadFile(pFilePath, &loadedComponents);
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

        m_currentWorkingFilePath.assign(pFilePath);
        size_t uFileId = m_pProject->GetComponentFileId(m_currentWorkingFilePath);
        std::map<size_t, std::vector<size_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto fileToComponentIter = pFileToComponentMap->find(uFileId);
        if (fileToComponentIter != pFileToComponentMap->end())
        {
            std::vector<size_t>& componentList = fileToComponentIter->second;
            for (size_t i = 0; i < componentList.size(); ++i)
            {
                BEATS_ASSERT(m_proxyInCurScene.find(componentList[i]) == m_proxyInCurScene.end());
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(componentList[i]));
                BEATS_ASSERT(pProxy != NULL);
                m_proxyInCurScene[componentList[i]] = pProxy;
            }
        }
        //TODO: bOpenAsCopy seems useless.
        bOpenAsCopy;
        m_bLoadingFilePhase = false;
    }
    m_currentViewFilePath = pFilePath;
}

void CComponentProxyManager::LoadFile(const TCHAR* pszFilePath, std::vector<CComponentProxy*>* pComponentContainer)
{
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
                TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement();
                while (pInstanceElement != NULL && loadSuccess)
                {
                    int id = -1;
                    pInstanceElement->Attribute("Id", &id);
                    BEATS_ASSERT(id != -1);
                    CComponentProxy* pComponent = NULL;
                    if (_stricmp(pInstanceElement->Value(), "Instance") == 0)
                    {
                        pComponent = (CComponentProxy*)(CreateComponent(guid, false, false, id, false, NULL, false));
                    }
                    else if (_stricmp(pInstanceElement->Value(), "Reference") == 0)
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

void CComponentProxyManager::CloseFile(const TCHAR* pszFilePath, bool bRefreshProjectData)
{
    if (_tcslen(pszFilePath) > 0)
    {
        std::vector<CComponentBase*> componentToDelete;
        if (m_currentWorkingFilePath.compare(pszFilePath) == 0)
        {
            //Closing current file, just get the m_proxyInCurScene
            for (auto iter = m_proxyInCurScene.begin(); iter != m_proxyInCurScene.end(); ++iter)
            {
                iter->second->Uninitialize();
                componentToDelete.push_back(iter->second);
            }
            m_proxyInCurScene.clear();
        }
        else
        {
            // Closing file is not current scene, query id from the static data: m_pProject.
            size_t uFileId = m_pProject->GetComponentFileId(pszFilePath);
            std::map<size_t, std::vector<size_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
            auto iter = pFileToComponentMap->find(uFileId);
            BEATS_ASSERT(iter != pFileToComponentMap->end());
            for (size_t i = 0;i < iter->second.size(); ++i)
            {
                size_t uComponentId = iter->second.at(i);
                CComponentBase* pComponent = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
                BEATS_ASSERT(pComponent != NULL);
                pComponent->Uninitialize();
                componentToDelete.push_back(pComponent);
            }
        }
        for (size_t i = 0; i < componentToDelete.size(); ++i)
        {
            BEATS_SAFE_DELETE(componentToDelete[i]);
        }
        if (bRefreshProjectData)
        {
            // When we close a file, we may have saved it or reverted the change
            // However, no matter which way we have chosen, we just reload the whole file to keep the id manager is working in the right way.
            size_t uFileId = m_pProject->GetComponentFileId(pszFilePath);
            BEATS_ASSERT(uFileId != 0xFFFFFFFF);
            m_pProject->ReloadFile(uFileId);
        }
    }
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
    BEATS_ASSERT(pSavePath != NULL);
    CSerializer serializer;
    serializer << COMPONENT_SYSTEM_VERSION;
    serializer << GetComponentTemplateMap()->size();
    CComponentProjectDirectory* pRootProject = m_pProject->GetRootDirectory();
    pRootProject->Serialize(serializer);
    serializer << m_pProject->GetLaunchStartDirectory()->GenerateLogicPath();
    TString workingFileCache = m_currentWorkingFilePath;
    if (m_currentWorkingFilePath.length() > 0)
    {
        CloseFile(m_currentWorkingFilePath.c_str());
    }
    size_t uFileCount = m_pProject->GetFileList()->size();
    serializer << uFileCount;
    for (size_t i = 0; i < uFileCount; ++i)
    {
        const TString& strFileName = m_pProject->GetFileList()->at(i);
        OpenFile(strFileName.c_str());
        size_t uComponentCount = 0;
        size_t uWritePos = serializer.GetWritePos();
        serializer << uWritePos;// File Start pos.
        serializer << uWritePos;// File size placeholder.
        serializer.GetWritePos();
        serializer << uComponentCount;
        for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
        {
            for (std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                ++uComponentCount;
                CComponentProxy* pProxy = static_cast<CComponentProxy*>(subIter->second);
                pProxy->Serialize(serializer, eVT_SavedValue);
            }
        }
        size_t uCurWritePos = serializer.GetWritePos();
        size_t uFileDataSize = uCurWritePos - uWritePos;
        serializer.SetWritePos(uWritePos + sizeof(size_t));// Skip file start pos.
        serializer << uFileDataSize; // Exclude file size.
        serializer << uComponentCount;
        serializer.SetWritePos(uCurWritePos);
        // We just open the file to do export operation, it's not possible to change the file, so we don't refresh the project data to save time.
        SaveToFile(strFileName.c_str());
        CloseFile(false);
    }
    serializer.Deserialize(pSavePath);
    //Restore the file which was opened before export.
    if (workingFileCache.length() > 0)
    {
        OpenFile(workingFileCache.c_str());
    }
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

void CComponentProxyManager::SaveToFile( const TCHAR* pFileName /* = NULL*/)
{
    if (pFileName == NULL)
    {
        pFileName = m_currentWorkingFilePath.c_str();
    }
    BEATS_ASSERT(_tcslen(pFileName) > 0);
    if (_tcslen(pFileName) > 0)
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
        CStringHelper::GetInstance()->ConvertToCHAR(pFileName, pathInChar, MAX_PATH);
        document.SaveFile(pathInChar);
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
        pReference->Uninitialize();
        BEATS_SAFE_DELETE(pReference);
        m_proxyInCurScene.erase(pProxy->GetId());
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
