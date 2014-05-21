#include "stdafx.h"
#include "ComponentProxyManager.h"
#include "ComponentManager.h"
#include "ComponentProject.h"
#include "TinyXML/tinyxml.h"
#include "Serializer/Serializer.h"
#include "StringHelper/StringHelper.h"
#include "UtilityManager.h"
#include "IdManager/IdManager.h"
#include "ComponentGraphic.h"
#include "DependencyDescriptionLine.h"
#include "DependencyDescription.h"

CComponentProxyManager* CComponentProxyManager::m_pInstance = NULL;

CComponentProxyManager::CComponentProxyManager()
    : m_pCurrReflectPropertyDescription(NULL)
    , m_pCurrReflectDependency(NULL)
    , m_bReflectCheckFlag(false)
{
    m_pProject = new CComponentProject;
    m_pPropertyCreatorMap = new std::map<size_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<size_t, std::vector<size_t>>();
}

CComponentProxyManager::~CComponentProxyManager()
{
    BEATS_SAFE_DELETE(m_pProject);
    BEATS_SAFE_DELETE(m_pPropertyCreatorMap);
    BEATS_SAFE_DELETE(m_pComponentInheritMap);
}

void CComponentProxyManager::OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy /*= false */)
{
    if (!bOpenAsCopy || !m_currentWorkingFilePath.empty()) // We must first open a file,then we can copy from the pFilePath.
    {
        char tmp[MAX_PATH] = {0};
        CStringHelper::GetInstance()->ConvertToCHAR(pFilePath, tmp, MAX_PATH);
        TiXmlDocument document(tmp);
        bool loadSuccess = document.LoadFile(TIXML_ENCODING_LEGACY);
        if (loadSuccess)
        {
            TiXmlElement* pRootElement = document.RootElement();
            TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
            std::vector<CComponentEditorProxy*> copyOpenComponents;
            if (pComponentListNode != NULL )
            {
                std::vector<int> reservedId; 
                TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
                while (pComponentElement != NULL && loadSuccess)
                {
                    const char* pGuidStr = pComponentElement->Attribute("GUID");
                    char* pStopPos = NULL;
                    int guid = strtoul(pGuidStr, &pStopPos, 16);
                    TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement("Instance");
                    while (pInstanceElement != NULL && loadSuccess)
                    {
                        int id = -1;
                        pInstanceElement->Attribute("Id", &id);
                        BEATS_ASSERT(id != -1);
                        CComponentEditorProxy* pComopnent = static_cast<CComponentEditorProxy*>(CreateComponent(guid, false, false, id, false, NULL));
                        pComopnent->LoadFromXML(pInstanceElement);
                        pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                    }
                    pComponentElement = pComponentElement->NextSiblingElement("Component");
                }
            }
            ResolveDependency();
            const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = GetComponentInstanceMap();
            for (std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin(); iter != pInstanceMap->end(); ++iter)
            {
                for (std::map<size_t, CComponentBase*>::const_iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
                {
                    CComponentEditorProxy* pProxyInstance = dynamic_cast<CComponentEditorProxy*>(subIter->second);
                    BEATS_ASSERT(pProxyInstance != NULL);
                    // We need to update the host component after the proxy's dependencies have been resolved.
                    pProxyInstance->UpdateHostComponent();
                    if (pProxyInstance->GetHostComponent() != NULL)
                    {
                        pProxyInstance->GetHostComponent()->Initialize();
                    }
                }
            }
            if (bOpenAsCopy)
            {
                std::vector<CComponentBase*> allInstance;
                std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
                for (; iter != m_pComponentInstanceMap->end(); ++iter)
                {
                    std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin();
                    for (; subIter != iter->second->end(); ++subIter)
                    {
                        allInstance.push_back(subIter->second);
                    }
                }
                for (size_t i = 0; i < allInstance.size(); ++i)
                {
                    UnregisterInstance(allInstance[i]);
                    allInstance[i]->SetId(m_pIdManager->GenerateId());
                    RegisterInstance(allInstance[i]);
                }
            }
            else
            {
                m_currentWorkingFilePath.assign(pFilePath);
            }
        }
        else
        {
            TCHAR info[MAX_PATH];
            TCHAR reason[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(document.ErrorDesc(), reason, MAX_PATH);
            _stprintf(info, _T("文件 :%s 读取失败！原因：%s"), pFilePath, reason);
            MessageBox(NULL, info, _T("打开文件失败"), MB_OK | MB_ICONERROR);
        }
    }
}

void CComponentProxyManager::CloseFile(bool bRefreshProjectData)
{
    if (m_currentWorkingFilePath.length() > 0)
    {
        for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
        {
            for (std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                BEATS_SAFE_DELETE(subIter->second);
                m_pIdManager->RecycleId(subIter->first);
            }
            BEATS_SAFE_DELETE(iter->second);
        }
        m_pComponentInstanceMap->clear();

        if (bRefreshProjectData)
        {
            // When we close a file, we may have saved it or reverted the change
            // However, no matter which way we have chosen, we just reload the whole file to keep the id manager is working in the right way.
            size_t uFileId = m_pProject->GetComponentFileId(m_currentWorkingFilePath);
            BEATS_ASSERT(uFileId != 0xFFFFFFFF);
            m_pProject->ReloadFile(uFileId);
        }
        m_currentWorkingFilePath.clear();
    }
}

const TString& CComponentProxyManager::GetCurrentWorkingFilePath() const
{
    return m_currentWorkingFilePath;
}

void CComponentProxyManager::Export(const std::vector<TString>& fileList, const TCHAR* pSavePath)
{
    BEATS_ASSERT(pSavePath != NULL);
    CSerializer serializer;
    serializer << COMPONENT_SYSTEM_VERSION;
    serializer << fileList.size();
    TString workingFileCache = m_currentWorkingFilePath;
    if (m_currentWorkingFilePath.length() > 0)
    {
        CloseFile();
    }
    for (size_t i = 0; i < fileList.size(); ++i)
    {
        OpenFile(fileList[i].c_str());
        size_t uComponentCount = 0;
        size_t uWritePos = serializer.GetWritePos();
        serializer << uComponentCount;
        for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
        {
            for (std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                ++uComponentCount;
                CComponentEditorProxy* pProxy = static_cast<CComponentEditorProxy*>(subIter->second);
                pProxy->Serialize(serializer, eVT_SavedValue);
            }
        }
        size_t uCurWritePos = serializer.GetWritePos();
        serializer.SetWritePos(uWritePos);
        serializer << uComponentCount;
        serializer.SetWritePos(uCurWritePos);
        // We just open the file to do export operation, it's not possible to change the file, so we don't refresh the project data to save time.
        SaveToFile(fileList[i].c_str());
        CloseFile(false);
    }
    serializer.Deserialize(pSavePath);
    //Restore the file which was opened before export.
    if (workingFileCache.length() > 0)
    {
        OpenFile(workingFileCache.c_str());
    }
}

void CComponentProxyManager::QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse )
{
    result.clear();
    std::map<size_t, std::vector<size_t>>::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
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
    std::map<size_t, std::vector<size_t>>::iterator iter = m_pComponentInheritMap->find(uBaseClassGuid);
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
        BEATS_ASSERT(pComponent != NULL, _T("Can't Find Component with GUID: 0x%x"), uGuid);
        strRet = pComponent->GetClassStr();
    }
    return strRet;
}

CComponentProject* CComponentProxyManager::GetProject() const
{
    return m_pProject;
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
        sprintf_s(szGUIDHexStr, "0x%x", iter->first);
        pComponentElement->SetAttribute("GUID", szGUIDHexStr);
        char tmp[MAX_PATH] = {0};
        CStringHelper::GetInstance()->ConvertToCHAR(GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
        pComponentElement->SetAttribute("Name", tmp);
        static_cast<CComponentEditorProxy*>(iter->second)->SaveToXML(pComponentElement, true);
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
        const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = GetComponentInstanceMap();
        std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin();
        for (; iter != pInstanceMap->end(); ++iter)
        {
            TiXmlElement* pComponentElement = new TiXmlElement("Component");
            char szGUIDHexStr[32] = {0};
            sprintf_s(szGUIDHexStr, "0x%x", iter->first);
            pComponentElement->SetAttribute("GUID", szGUIDHexStr);
            char tmp[MAX_PATH] = {0};
            CStringHelper::GetInstance()->ConvertToCHAR(GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
            pComponentElement->SetAttribute("Name", tmp);
            pComponentListElement->LinkEndChild(pComponentElement);
            std::map<size_t, CComponentBase*>::const_iterator instanceIter = iter->second->begin();
            for (;instanceIter != iter->second->end(); ++instanceIter)
            {
                CComponentEditorProxy* pProxy = static_cast<CComponentEditorProxy*>(instanceIter->second);
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
    return (*(*m_pPropertyCreatorMap)[propertyType])(serializer);
}

void CComponentProxyManager::DeserializeTemplateData(const TCHAR* pWorkingPath, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc)
{
    TCHAR szFilePath[MAX_PATH];
    _stprintf(szFilePath, _T("%s\\%s"), pWorkingPath, EXPORT_STRUCTURE_DATA_FILENAME);
    FILE* pComponentFile = _tfopen(szFilePath, _T("rb+"));
    if (pComponentFile == NULL)
    {
        MessageBox(NULL, _T("找不到导出的EDS文件!\n无法初始化组件系统!"), _T("Error"), MB_OK);
    }
    else
    {    
        // Step 1: Load Info from serialize data comes from our code.
        CSerializer componentsSerializer(szFilePath);
        fclose(pComponentFile);
        const TCHAR* pHeaderStr = COMPONENT_FILE_HEADERSTR;
        TCHAR* pbuff = NULL;
        TCHAR** buff = &pbuff;
        componentsSerializer.Read(buff);
        if (_tcsicmp(pHeaderStr, *buff) != 0)
        {
            MessageBox(NULL, _T("不是正确的文件格式,读取失败!"), _T("Error"), MB_OK | MB_ICONERROR);
        }
        else
        {
            LoadTemplateDataFromSerializer(componentsSerializer, func, pGraphicFunc);
            // Step 2: Fix the value from XML.
            _stprintf(szFilePath, _T("%s\\%s"), pWorkingPath, EXPORT_STRUCTURE_DATA_PATCH_XMLFILENAME);
            LoadTemplateDataFromXML(szFilePath);
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
        pDependencyResolver->pDescription->SetDependency(pDependencyResolver->uIndex, static_cast<CComponentEditorProxy*>(pComponentToBeLink));
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
                    CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(GetComponentTemplate(guid));
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
            CComponentEditorProxy* pComponentEditorProxy = func(pGraphicFunc(), guid, parentGuid, pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetDisplayName(pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetCatalogName(pStrHolder);
            BEATS_ASSERT(m_pComponentTemplateMap->find(guid) == m_pComponentTemplateMap->end(), _T("Template component proxy already exists!GUID:0x%x, id:%d"), guid, pComponentEditorProxy->GetId());
            RegisterTemplate(pComponentEditorProxy);
            CComponentBase* pInstance = CComponentManager::GetInstance()->GetComponentTemplate(guid);
            BEATS_ASSERT(pInstance != NULL, _T("Cant find a template instance for a proxy to be its host!GUID:0x%x, id:%d"), guid, pComponentEditorProxy->GetId());
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
    std::map<size_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin();
    for (; iter != m_pComponentTemplateMap->end(); ++iter )
    {
        iter->second->Initialize();
    }
}
