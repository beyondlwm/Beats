#include "stdafx.h"
#include "ComponentManager.h"
#include "ComponentProject.h"
#include "ComponentBase.h"
#include "ComponentEditorProxy.h"
#include "../../Utility/Serializer/Serializer.h"
#include "../../Utility/TinyXML/tinyxml.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../Utility/IdManager/IdManager.h"
#include "../../Utility/UtilityManager.h"
#include "ComponentGraphic.h"
#include "../DependencyDescriptionLine.h"
#include "../DependencyDescription.h"

CComponentManager* CComponentManager::m_pInstance = NULL;

void DefaultAddDependencyFunc(void* pContainer, void* pDependency)
{
    ((std::vector<void*>*)(pContainer))->push_back((void*)pDependency);
}

CComponentManager::CComponentManager()
{
    m_pComponentTemplateMap = new std::map<size_t, CComponentBase*>;
    m_pComponentInstanceMap = new std::map<size_t, std::map<size_t, CComponentBase*>*>;
    m_pDependencyResolver = new std::vector<SDependencyResolver*>;
    m_pPropertyCreatorMap = new std::map<size_t, TCreatePropertyFunc>();
    m_pComponentInheritMap = new std::map<size_t, std::vector<size_t>>();
    m_pIdManager = new CIdManager;
    m_pProject = new CComponentProject;
}

CComponentManager::~CComponentManager()
{
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
    for (; iter != m_pComponentInstanceMap->end(); ++iter)
    {
        std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin();
        for (; subIter != iter->second->end(); ++subIter)
        {
            BEATS_SAFE_DELETE(subIter->second);
        }
        BEATS_SAFE_DELETE(iter->second);
    }
    BEATS_SAFE_DELETE(m_pComponentInstanceMap);

    typedef std::map<size_t, CComponentBase*> TComponentMap;
    BEATS_SAFE_DELETE_MAP((*m_pComponentTemplateMap), TComponentMap);
    BEATS_SAFE_DELETE(m_pComponentTemplateMap);    
    
    BEATS_SAFE_DELETE(m_pDependencyResolver);
    BEATS_SAFE_DELETE(m_pPropertyCreatorMap);
    BEATS_SAFE_DELETE(m_pComponentInheritMap);
    BEATS_SAFE_DELETE(m_pProject);
    BEATS_SAFE_DELETE(m_pIdManager);

    // Don't remove GetInstance(), it is force to create an instance of CUtilityManager, so we can visit the member of it.
    CUtilityManager::GetInstance();
    CUtilityManager::Destroy();
}

bool CComponentManager::RegisterTemplate( CComponentBase* pComponent )
{
    std::map<size_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->find(pComponent->GetGuid());
    BEATS_ASSERT(iter == m_pComponentTemplateMap->end(), _T("component %s guid 0x%x is already registered!"), pComponent->GetClassStr(), pComponent->GetGuid());
    if (iter == m_pComponentTemplateMap->end())
    {
        (*m_pComponentTemplateMap)[pComponent->GetGuid()] = pComponent;
    }
    return iter == m_pComponentTemplateMap->end();
}

bool CComponentManager::RegisterInstance(CComponentBase* pNewInstance)
{
    size_t guid = pNewInstance->GetGuid();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    if (iter == m_pComponentInstanceMap->end())
    {
        (*m_pComponentInstanceMap)[guid] = new std::map<size_t, CComponentBase*>;
        iter = m_pComponentInstanceMap->find(guid);
    }
    BEATS_ASSERT(iter->second->find(pNewInstance->GetId()) == iter->second->end(), _T("Create an existing component! GUID:0x%x, InstanceId: %d"), guid, pNewInstance->GetId());
    (*iter->second)[pNewInstance->GetId()] = pNewInstance;
    return true;
}

bool CComponentManager::UnregisterInstance(CComponentBase* pNewInstance)
{
    size_t guid = pNewInstance->GetGuid();
    std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(guid);
    BEATS_ASSERT(iter != m_pComponentInstanceMap->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
                                                        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    std::map<size_t, CComponentBase*>::iterator subIter = iter->second->find(pNewInstance->GetId());
    BEATS_ASSERT(subIter != iter->second->end(), _T("Unregister instance component %s guid 0x%x id %d failed!"), 
                                                        pNewInstance->GetClassStr(), pNewInstance->GetGuid(), pNewInstance->GetId());
    iter->second->erase(subIter);
    if (iter->second->size() == 0)
    {
        m_pComponentInstanceMap->erase(iter);
    }
    return true;
}

void CComponentManager::SerializeTemplateData(CSerializer& serializer)
{
    TCHAR workingPath[MAX_PATH];
    GetModuleFileName(NULL, workingPath, MAX_PATH);
    int iStrPos = CStringHelper::GetInstance()->FindLastString(workingPath, _T("\\"), false);
    BEATS_ASSERT(iStrPos >= 0);
    workingPath[iStrPos] = 0;

    TCHAR szFilePath[MAX_PATH];
    _stprintf(szFilePath, _T("%s\\%s"), &workingPath, EXPORT_STRUCTURE_DATA_FILENAME);
    serializer.Deserialize(szFilePath);
}

void CComponentManager::DeserializeTemplateData(const TCHAR* pWorkingPath, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc)
{
    TCHAR szFilePath[MAX_PATH];
    _stprintf(szFilePath, _T("%s\\%s"), pWorkingPath, EXPORT_STRUCTURE_DATA_FILENAME);
    FILE* pComponentFile = _tfopen(szFilePath, _T("rb+"));
    if (pComponentFile == NULL)
    {
        MessageBox(NULL, _T("找不到UI文件!\n无法初始化组件系统!"), _T("Error"), MB_OK);
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

CComponentBase* CComponentManager::GetComponentTemplate( size_t guid ) const
{
    CComponentBase* pResult = NULL;
    std::map<size_t, CComponentBase*>::const_iterator iter = m_pComponentTemplateMap->find(guid);
    if (iter != m_pComponentTemplateMap->end())
    {
        pResult = iter->second;
    }
    return pResult;
}

const std::map<size_t, CComponentBase*>* CComponentManager::GetComponentTemplateMap()
{
    return m_pComponentTemplateMap;
}

CComponentBase* CComponentManager::CreateComponent( size_t guid, bool bCloneFromTemplate, bool bManualManage/* = false*/, size_t specifiedInstanceId /*=0xffffffff*/, bool bCheckRequestId/* = true*/, CSerializer* pData /*=NULL*/)
{
    CComponentBase* pNewInstance = NULL;
    __try
    {
        CComponentBase* pTemplate = GetComponentTemplate(guid);
        BEATS_ASSERT(pTemplate != NULL, _T("Create an unknown component, Guid:0x%x id: %d"), guid, specifiedInstanceId);
        pNewInstance = CreateComponentByRef(pTemplate, bCloneFromTemplate, bManualManage, specifiedInstanceId, bCheckRequestId, pData);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
    return pNewInstance;
}

CComponentBase* CComponentManager::CreateComponentByRef( CComponentBase* pComponentRef, bool bCloneValue, bool bManualManage /*= false*/, size_t specifiedInstanceId /*= 0xFFFFFFFF*/, bool bCheckRequestId /*= true*/, CSerializer* pData /*= NULL*/ )
{
    BEATS_ASSERT(pComponentRef != NULL, _T("Create an unknown component by NULL ref"));
    CComponentBase* pNewInstance = static_cast<CComponentBase*>(pComponentRef->Clone(bCloneValue, pData));
    if (!bManualManage)
    {
        size_t id = specifiedInstanceId;
        if (specifiedInstanceId != 0xFFFFFFFF)
        {
            m_pIdManager->ReserveId(specifiedInstanceId, bCheckRequestId);
        }
        else
        {
            id = m_pIdManager->GenerateId();
        }
        pNewInstance->SetId( id );
        RegisterInstance(pNewInstance);
    }
    else
    {
        BEATS_ASSERT(specifiedInstanceId == 0xFFFFFFFF, _T("If you want to manage component instance by yourself, don't require instance id!"));
    }
    return pNewInstance;
}

bool CComponentManager::DeleteComponent( CComponentBase* pComponent )
{
    UnregisterInstance(pComponent);
    m_pIdManager->RecycleId(pComponent->GetId());
    BEATS_SAFE_DELETE(pComponent);

    return true;
}

const std::map<size_t, std::map<size_t, CComponentBase*>*>* CComponentManager::GetComponentInstanceMap()
{
    return m_pComponentInstanceMap;
}

void CComponentManager::LoadTemplateDataFromSerializer(CSerializer& serializer, TCreateComponentEditorProxyFunc func, TCreateGraphicFunc pGraphicFunc)
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
        RegisterClassInheritInfo(guid, parentGuid);
        if (!bIsAbstractClass)
        {
            TCHAR* pStrHolder = NULL;
            TCHAR** ppStrHolder = &pStrHolder;
            serializer.Read(ppStrHolder);
            CComponentEditorProxy* pComponentEditorProxy = func(pGraphicFunc(), guid, parentGuid, pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetDisplayName(pStrHolder);
            serializer.Read(ppStrHolder);
            pComponentEditorProxy->SetCatalogName(pStrHolder);
            RegisterTemplate(pComponentEditorProxy);
            pComponentEditorProxy->Deserialize(serializer);
        }
        BEATS_ASSERT(serializer.GetReadPos() == curReadPos + totalSize);
    }
    std::map<size_t, CComponentBase*>::iterator iter = m_pComponentTemplateMap->begin();
    for (; iter != m_pComponentTemplateMap->end(); ++iter )
    {
        iter->second->Initialize();
    }
}

void CComponentManager::LoadTemplateDataFromXML(const TCHAR* pszPath)
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
                    CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->GetComponentTemplate(guid));
                    if (pComponent != NULL)
                    {
                        pComponent->LoadFromXML(pInstanceElement);
                    }
                    else
                    {
                        BEATS_PRINT(_T("Component (guid %d) doesn't exist, have you deleted it recently?\n"), guid);
                    }
                    pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                }
                pComponentElement = pComponentElement->NextSiblingElement("Component");
            }
        }    
    }
}

void CComponentManager::OpenFile(const TCHAR* pFilePath, bool bOpenAsCopy /*= false */)
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
                        CComponentEditorProxy* pComopnent = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->CreateComponent(guid, false, false, id, false, NULL));
                        pComopnent->LoadFromXML(pInstanceElement);
                        pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                    }
                    pComponentElement = pComponentElement->NextSiblingElement("Component");
                }
            }
            ResolveDependency();
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


CComponentBase* CComponentManager::GetComponentInstance( size_t uId , size_t uGUID/* = 0xffffffff*/)
{
    CComponentBase* pResult = NULL;
    if (uGUID != 0xFFFFFFFF)
    {
        std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->find(uGUID);
        if (iter != m_pComponentInstanceMap->end())
        {
            std::map<size_t, CComponentBase*>::iterator subIter = iter->second->find(uId);
            if (subIter != iter->second->end())
            {
                pResult = subIter->second;
            }
        }
    }
    else
    {
        std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin();
        for (; iter != m_pComponentInstanceMap->end(); ++iter)
        {
            pResult = GetComponentInstance(uId, iter->first);
            if (pResult != NULL)
            {
                break;
            }
        }
    }
    return pResult;
}

void CComponentManager::SaveTemplate(const TCHAR* pszFilePath)
{
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);

    TiXmlElement* pComponentListElement = new TiXmlElement("Components");
    const std::map<size_t, CComponentBase*>* pInstanceMap = CComponentManager::GetInstance()->GetComponentTemplateMap();
    std::map<size_t, CComponentBase*>::const_iterator iter = pInstanceMap->begin();
    for (; iter != pInstanceMap->end(); ++iter)
    {
        TiXmlElement* pComponentElement = new TiXmlElement("Component");
        char szGUIDHexStr[32] = {0};
        sprintf_s(szGUIDHexStr, "0x%x", iter->first);
        pComponentElement->SetAttribute("GUID", szGUIDHexStr);
        char tmp[MAX_PATH] = {0};
        CStringHelper::GetInstance()->ConvertToCHAR(CComponentManager::GetInstance()->GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
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

void CComponentManager::SaveToFile( const TCHAR* pFileName /* = NULL*/)
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
        const std::map<size_t, std::map<size_t, CComponentBase*>*>* pInstanceMap = CComponentManager::GetInstance()->GetComponentInstanceMap();
        std::map<size_t, std::map<size_t, CComponentBase*>*>::const_iterator iter = pInstanceMap->begin();
        for (; iter != pInstanceMap->end(); ++iter)
        {
            TiXmlElement* pComponentElement = new TiXmlElement("Component");
            char szGUIDHexStr[32] = {0};
            sprintf_s(szGUIDHexStr, "0x%x", iter->first);
            pComponentElement->SetAttribute("GUID", szGUIDHexStr);
            char tmp[MAX_PATH] = {0};
            CStringHelper::GetInstance()->ConvertToCHAR(CComponentManager::GetInstance()->GetComponentTemplate(iter->first)->GetClassStr(), tmp, MAX_PATH);
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

void CComponentManager::Export(const std::vector<TString>& fileList, const TCHAR* pSavePath)
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
                pProxy->Serialize(serializer);
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

void CComponentManager::Import( CSerializer& serializer)
{
    size_t uVersion = 0;
    serializer >> uVersion;
    if (uVersion != COMPONENT_SYSTEM_VERSION)
    {
        TCHAR szInfo[256];
        _stprintf(szInfo, _T("Data version is %d while our program version is %d"), uVersion, COMPONENT_SYSTEM_VERSION);
        MessageBox(NULL, szInfo, _T("Verion Not Match!"), MB_OK | MB_ICONERROR);
    }
    else
    {
        // 1. Load binarize data.
        size_t uFileCount = 0;
        serializer >> uFileCount;
        for (size_t j = 0; j < uFileCount; ++j)
        {
            size_t uComponentCount = 0;
            serializer >> uComponentCount;
            for (size_t i = 0; i < uComponentCount; ++i)
            {
                size_t uDataSize, uGuid, uId;
                size_t uStartPos = serializer.GetReadPos();
                serializer >> uDataSize >> uGuid >> uId;
                CComponentBase* pComponent = CComponentManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, &serializer);
                if (uStartPos + uDataSize != serializer.GetReadPos())
                {
                    TCHAR szInfo[256];
                    _stprintf(szInfo, _T("Got an error when import data for component %x %s instance id %d"), uGuid, pComponent->GetClassStr(), uId);
                    MessageBox(NULL, szInfo, _T("Component Data Not Match!"), MB_OK | MB_ICONERROR);
                }
                serializer.SetReadPos(uStartPos + uDataSize);
            }
        }
        
        // 2. Resolve dependency.
        CComponentManager::GetInstance()->ResolveDependency();
        
        // 3. Call Initialize.
        for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
        {
            for (std::map<size_t, CComponentBase*>::iterator subIter = iter->second->begin(); subIter != iter->second->end(); ++subIter)
            {
                subIter->second->Initialize();
            }
        }
    }
}

void CComponentManager::CloseFile(bool bRefreshProjectData)
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

void CComponentManager::AddDependencyResolver( CDependencyDescription* pDescription, size_t uIndex, size_t uGuid, size_t uInstanceId , void* pVariableAddress, bool bIsList, TAddDependencyFunc pFunc /*= NULL*/)
{
    SDependencyResolver* pDependencyResovler = new SDependencyResolver;
    pDependencyResovler->pDescription = pDescription;
    pDependencyResovler->uIndex = uIndex;
    pDependencyResovler->uGuid = uGuid;
    pDependencyResovler->uInstanceId = uInstanceId;
    pDependencyResovler->pVariableAddress = pVariableAddress;
    pDependencyResovler->bIsList = bIsList;
    pDependencyResovler->pAddFunc = pFunc == NULL ? DefaultAddDependencyFunc : pFunc;
    m_pDependencyResolver->push_back(pDependencyResovler);
}

void CComponentManager::ResolveDependency()
{
    for (size_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        if (pDependencyResolver->pDescription != NULL)
        {
            BEATS_WARNING(pComponentToBeLink != NULL, 
                _T("Component %s id %d can't resolve its dependency %s to component guid %d id %d, have you deleted that component recently?"),
                pDependencyResolver->pDescription->GetOwner()->GetClassStr(), 
                pDependencyResolver->pDescription->GetOwner()->GetId(),
                pDependencyResolver->pDescription->GetDisplayName(),
                pDependencyResolver->uGuid,
                pDependencyResolver->uInstanceId);
            BEATS_ASSERT(pDependencyResolver->pDescription->GetDependencyLine(pDependencyResolver->uIndex)->GetConnectedComponent() == NULL);
            pDependencyResolver->pDescription->SetDependency(pDependencyResolver->uIndex, static_cast<CComponentEditorProxy*>(pComponentToBeLink));
        }
        else
        {
            if (pDependencyResolver->pVariableAddress != NULL)
            {
                if (pDependencyResolver->bIsList)
                {
                    pDependencyResolver->pAddFunc(pDependencyResolver->pVariableAddress, pComponentToBeLink);
                }
                else
                {
                    *(size_t*)pDependencyResolver->pVariableAddress = (size_t)pComponentToBeLink;
                }
            }
        }
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

void CComponentManager::RegisterPropertyCreator( size_t enumType, TCreatePropertyFunc func )
{
    BEATS_ASSERT(m_pPropertyCreatorMap->find(enumType) == m_pPropertyCreatorMap->end());
    (*m_pPropertyCreatorMap)[enumType] = func;
}

CPropertyDescriptionBase* CComponentManager::CreateProperty( size_t propertyType, CSerializer* serializer )
{
    return (*(*m_pPropertyCreatorMap)[propertyType])(serializer);
}

size_t CComponentManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}

void CComponentManager::QueryDerivedClass(size_t uBaseClassGuid, std::vector<size_t>& result, bool bRecurse )
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

void CComponentManager::RegisterClassInheritInfo( size_t uDerivedClassGuid, size_t uBaseClassGuid )
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

void CComponentManager::ResetComponentContainer()
{
    //Delete two container, because we may also launch the debug tool, and it will init the componentmanager again in CComponentsPage::InitComponentsPage
    for (std::map<size_t, std::map<size_t, CComponentBase*>*>::iterator iter = m_pComponentInstanceMap->begin(); iter != m_pComponentInstanceMap->end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_pComponentInstanceMap->clear();
    typedef std::map<size_t, CComponentBase*> TComponentMap;
    BEATS_SAFE_DELETE_MAP((*m_pComponentTemplateMap), TComponentMap);
    m_pIdManager->Reset();
}

const TString& CComponentManager::GetCurrentWorkingFilePath() const
{
    return m_currentWorkingFilePath;
}

CComponentProject* CComponentManager::GetProject() const
{
    return m_pProject;
}

CIdManager* CComponentManager::GetIdManager() const
{
    return m_pIdManager;
}

