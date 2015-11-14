#include "stdafx.h"
#include "ComponentProject.h"
#include "ComponentProjectDirectory.h"
#include "ComponentProxyManager.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/IdManager/IdManager.h"
#include "Utility/UtilityManager.h"
#include "FilePath/FilePathTool.h"

CComponentProject::CComponentProject()
    : m_pProjectDirectory(NULL)
    , m_uLoadedFileCount(0)
    , m_uTotalFileCount(0)
    , m_uStartFileId(0)
    , m_pComponentFiles(new std::vector<TString>)
    , m_pComponentToTypeMap(new std::map<uint32_t, uint32_t>)
    , m_pComponentToFileMap(new std::map<uint32_t, uint32_t>)
    , m_pFileToComponentMap(new std::map<uint32_t, std::vector<uint32_t> >)
    , m_pFileToDirectoryMap(new std::map<uint32_t, CComponentProjectDirectory*>)
    , m_pTypeToComponentMap(new std::map<uint32_t, std::vector<uint32_t> >)
    , m_pPropertyMaintainMap(new std::map<uint32_t, std::map<TString, TString> >)
    , m_pFileDataLayout(new std::map<uint32_t, SFileDataLayout>)
    , m_pIdToReferenceMap(new std::map<uint32_t, std::set<uint32_t> >)
    , m_pReferenceToIdMap(new std::map<uint32_t, uint32_t>)
{

}

CComponentProject::~CComponentProject()
{
    BEATS_SAFE_DELETE(m_pProjectDirectory);
    BEATS_SAFE_DELETE(m_pComponentFiles);
    BEATS_SAFE_DELETE(m_pComponentToTypeMap);
    BEATS_SAFE_DELETE(m_pComponentToFileMap);
    BEATS_SAFE_DELETE(m_pFileToComponentMap);
    BEATS_SAFE_DELETE(m_pFileToDirectoryMap);
    BEATS_SAFE_DELETE(m_pTypeToComponentMap);
    BEATS_SAFE_DELETE(m_pPropertyMaintainMap);
    BEATS_SAFE_DELETE(m_pFileDataLayout);
    BEATS_SAFE_DELETE(m_pIdToReferenceMap);
    BEATS_SAFE_DELETE(m_pReferenceToIdMap);
}

CComponentProjectDirectory* CComponentProject::LoadProject(const TCHAR* pszProjectFile, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap)
{
    CloseProject();
    m_uTotalFileCount = 0;
    m_uLoadedFileCount = 0;
    m_strCurrLoadingFile.clear();
    // NOTICE: because in editor mode we focus on CComponentProxyManager's id manager while
    // we focus on CComponentInstanceManager's id manager in game mode.
    // So we don't care and never exam CComponentInstanceManager's id manager in editor's mode. We lock it avoid chaos.
    if (!CComponentInstanceManager::GetInstance()->GetIdManager()->IsLocked())
    {
        CComponentInstanceManager::GetInstance()->GetIdManager()->Lock();
    }
    BEATS_ASSERT(CFilePathTool::GetInstance()->IsAbsolute(pszProjectFile), _T("Only accept absolute path!"));
    if (pszProjectFile != NULL && pszProjectFile[0] != 0)
    {
        m_strProjectFilePath = CFilePathTool::GetInstance()->ParentPath(pszProjectFile);
        m_strProjectFileName = CFilePathTool::GetInstance()->FileName(pszProjectFile);

        TiXmlDocument document;
        bool bLoadSuccess = document.LoadFile(pszProjectFile, TIXML_ENCODING_UTF8);
        BEATS_ASSERT(bLoadSuccess, _T("Load file %s failed!"), pszProjectFile);
        if (bLoadSuccess)
        {
            TiXmlElement* pRootElement = document.RootElement();
            m_uTotalFileCount = GetTotalFileCount(pRootElement);
            m_pProjectDirectory = new CComponentProjectDirectory(NULL, pRootElement->Value());
            TString strStartFile;
            LoadXMLProject(pRootElement, m_pProjectDirectory, strStartFile, conflictIdMap);
            m_uStartFileId = GetComponentFileId(strStartFile);
            BEATS_ASSERT(m_uStartFileId != 0xFFFFFFFF);
        }
        else
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load File :%s Failed! Row:%d Col:%d Reason:%s"), pszProjectFile, document.ErrorRow(), document.ErrorCol(), document.ErrorDesc());
            MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
    }
    m_strCurrLoadingFile.clear();
    BEATS_ASSERT(m_uLoadedFileCount == m_uTotalFileCount);
    return m_pProjectDirectory;
}

uint32_t CComponentProject::GetLoadProjectProgress(TString& strCurrLoadingFile) const
{
    uint32_t uRet = 0;
    if (m_uTotalFileCount > 0)
    {
        uRet = (uint32_t)(m_uLoadedFileCount * 100.f / m_uTotalFileCount);
        strCurrLoadingFile = m_strCurrLoadingFile;
    }
    return uRet;
}

bool CComponentProject::CloseProject()
{
    bool bRet = false;
    uint32_t uCurrLoadFileId = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
    if (uCurrLoadFileId != 0xFFFFFFFF)
    {
        CComponentProxyManager::GetInstance()->CloseFile(uCurrLoadFileId);
    }
    CComponentProxyManager::GetInstance()->GetIdManager()->Reset();
    if (m_pProjectDirectory != NULL)
    {
        bRet = true;
        BEATS_SAFE_DELETE(m_pProjectDirectory);
    }
    m_pComponentFiles->clear();

    m_pComponentToTypeMap->clear();
    m_pComponentToFileMap->clear();
    m_pFileToComponentMap->clear();
    m_pTypeToComponentMap->clear();
    m_pPropertyMaintainMap->clear();
    m_strProjectFilePath.clear();
    m_strProjectFileName.clear();
    return bRet;
}

void CComponentProject::SaveProject()
{
    TiXmlDocument document;
    TiXmlDeclaration* pDeclaration = new TiXmlDeclaration("1.0","","");
    document.LinkEndChild(pDeclaration);
    TiXmlElement* pRootElement = new TiXmlElement("Root");
    document.LinkEndChild(pRootElement);
    TiXmlElement* pStartDirectoryNode = new TiXmlElement("StartFile");
    TString strRelativePath = CFilePathTool::GetInstance()->MakeRelative(m_strProjectFilePath.c_str(), GetComponentFileName(m_uStartFileId).c_str());
    pStartDirectoryNode->SetAttribute("FilePath", strRelativePath.c_str());
    pRootElement->LinkEndChild(pStartDirectoryNode);

    SaveProjectFile(pRootElement, m_pProjectDirectory);
    TString strFullPath = m_strProjectFilePath;
    strFullPath.append(_T("/")).append(m_strProjectFileName);
    document.SaveFile(strFullPath.c_str());
}

void CComponentProject::SaveProjectFile( TiXmlElement* pParentNode, const CComponentProjectDirectory* p)
{
    //Root doesn't need to save/load.
    TiXmlElement* pNewDirectoryElement = pParentNode;
    if (p != m_pProjectDirectory)
    {
        pNewDirectoryElement = new TiXmlElement("Directory");
        pNewDirectoryElement->SetAttribute("Name", p->GetName().c_str());
        pParentNode->LinkEndChild(pNewDirectoryElement);
    }

    const std::vector<CComponentProjectDirectory*>& children = p->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        SaveProjectFile(pNewDirectoryElement, *iter);
    }

    const std::vector<uint32_t>& files = p->GetFileList();

    for (std::vector<uint32_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TiXmlElement* pNewFileElement = new TiXmlElement("File");
        uint32_t uFileNameId = *iter;

        TString strRelativePath = CFilePathTool::GetInstance()->MakeRelative(m_strProjectFilePath.c_str(), GetComponentFileName(uFileNameId).c_str());
        pNewFileElement->SetAttribute("Path", strRelativePath.c_str());
        pNewDirectoryElement->LinkEndChild(pNewFileElement);
    }
}

void CComponentProject::ResolveIdForFile(uint32_t uFileId, uint32_t idToResolve, bool bKeepId)
{
    if (bKeepId)
    {
        BEATS_ASSERT(m_pComponentToFileMap->find(idToResolve) != m_pComponentToFileMap->end());
        uint32_t uExistInFileId = (*m_pComponentToFileMap)[idToResolve];
        if (uExistInFileId != uFileId)
        {
            uint32_t uGuid = (*m_pComponentToTypeMap)[idToResolve];
            UnregisterComponent(idToResolve);
            RegisterComponent(uFileId, uGuid, idToResolve);
        }
    }
    else
    {
        TString strFileName = GetComponentFileName(uFileId);
        TiXmlDocument document(strFileName.c_str());
        bool loadSuccess = document.LoadFile(TIXML_ENCODING_UTF8);
        if (loadSuccess)
        {
            int iNewID = CComponentProxyManager::GetInstance()->GetIdManager()->GenerateId();
            TiXmlElement* pRootElement = document.RootElement();
            TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
            std::vector<CComponentProxy*> copyOpenComponents;
            if (pComponentListNode != NULL )
            {
                TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
                const char* pszGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                uint32_t uComponentGuid = strtoul(pszGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pszGuidStr, strFileName.c_str());
                RegisterComponent(uFileId, uComponentGuid, iNewID);
                while (pComponentElement != NULL)
                {
                    TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement("Instance");
                    while (pInstanceElement != NULL)
                    {
                        int id = -1;
                        pInstanceElement->Attribute("Id", &id);
                        BEATS_ASSERT(id != -1);
                        if (id  == (int)idToResolve)
                        {
                            pInstanceElement->SetAttribute("Id", iNewID);
                        }
                        TiXmlElement* pDependency = pInstanceElement->FirstChildElement("Dependency");
                        while (pDependency != NULL)
                        {
                            TiXmlElement* pDependencyNode = pDependency->FirstChildElement("DependencyNode");
                            while (pDependencyNode != NULL)
                            {
                                pDependencyNode->Attribute("Id", &id);
                                if (id == (int)idToResolve)
                                {
                                    pDependencyNode->SetAttribute("Id", iNewID);
                                }
                                pDependencyNode = pDependencyNode->NextSiblingElement("DependencyNode");
                            }
                            pDependency = pDependency->NextSiblingElement("Dependency");
                        }
                        pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                    }
                    pComponentElement = pComponentElement->NextSiblingElement("Component");
                }
            }
            document.SaveFile();
        }
        else
        {
            TCHAR info[MAX_PATH];
            _stprintf(info, _T("Load File :%s Failed!Row: %d Col: %d Reason: %s"), strFileName.c_str(), document.ErrorRow(), document.ErrorCol(), document.ErrorDesc());
            MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
    }
}

void CComponentProject::RegisterPropertyMaintainInfo(uint32_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName)
{
    std::map<uint32_t, std::map<TString, TString> >::iterator iter = m_pPropertyMaintainMap->find(uComponentGuid);
    if (iter == m_pPropertyMaintainMap->end())
    {
        (*m_pPropertyMaintainMap)[uComponentGuid] = std::map<TString, TString>();
        iter = m_pPropertyMaintainMap->find(uComponentGuid);
    }
    BEATS_ASSERT(iter->second.find(strOriginPropertyName) == iter->second.end(), _T("The property %s has already been registered for maintain!"), strOriginPropertyName.c_str());
    (iter->second)[strOriginPropertyName] = strReplacePropertyName;
}

bool CComponentProject::GetReplacePropertyName(uint32_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult)
{
    bool bRet = false;
    std::map<uint32_t, std::map<TString, TString> >::iterator iter = m_pPropertyMaintainMap->find(uComponentGuid);
    if (iter != m_pPropertyMaintainMap->end())
    {
        std::map<TString, TString>::const_iterator subIter = iter->second.find(strOriginPropertyName);
        if (subIter != iter->second.end())
        {
            strResult = subIter->second;
            bRet = true;
        }
    }
    return bRet;
}

const std::vector<TString>* CComponentProject::GetFileList() const
{
    return m_pComponentFiles;
}

void CComponentProject::RegisterFileLayoutInfo(uint32_t uFileId, uint32_t uStartPos, uint32_t uDataLength)
{
    BEATS_ASSERT(m_pFileDataLayout->find(uFileId) == m_pFileDataLayout->end(), _T("File layout info already exists! id %d"), uFileId);
    (*m_pFileDataLayout)[uFileId] = SFileDataLayout(uStartPos, uDataLength);
}

bool CComponentProject::QueryFileLayoutInfo(uint32_t uFileId, uint32_t& uStartPos, uint32_t& uDataLength) const
{
    bool bRet = false;
    auto iter = m_pFileDataLayout->find(uFileId);
    if (iter != m_pFileDataLayout->end())
    {
        uStartPos = iter->second.m_uStartPos;
        uDataLength = iter->second.m_uDataSize;
        bRet = true;
    }
    return bRet;
}

void CComponentProject::SetStartFile(uint32_t uFileId)
{
    m_uStartFileId = uFileId;
}

uint32_t CComponentProject::GetStartFile() const
{
    return m_uStartFileId;
}

CComponentProjectDirectory* CComponentProject::FindProjectDirectory(const TString& strPath, bool bAbsoluteOrLogicPath) const
{
    CComponentProjectDirectory* pCurDirectory = NULL;
    if (bAbsoluteOrLogicPath)
    {
        uint32_t uFileId = CComponentProxyManager::GetInstance()->GetProject()->GetComponentFileId(strPath);
        BEATS_ASSERT(uFileId != 0xFFFFFFFF);
        pCurDirectory = (*m_pFileToDirectoryMap)[uFileId];
    }
    else
    {
        std::vector<TString> vecDirectories;
        CStringHelper::GetInstance()->SplitString(strPath.c_str(), _T("/"), vecDirectories);
        pCurDirectory = m_pProjectDirectory;
        //Ignore the root, so start from 1.
        for (uint32_t i = 1; i < vecDirectories.size(); ++i)
        {
            pCurDirectory = pCurDirectory->FindChild(vecDirectories[i].c_str());
            BEATS_ASSERT(pCurDirectory != NULL);
        }
    }

    return pCurDirectory;
}

CComponentProjectDirectory* CComponentProject::FindProjectDirectoryById(uint32_t uFileId)
{
    CComponentProjectDirectory* pRet = NULL;
    auto iter = m_pFileToDirectoryMap->find(uFileId);
    if (iter != m_pFileToDirectoryMap->end())
    {
        pRet = iter->second;
    }
    return pRet;
}

uint32_t CComponentProject::QueryComponentGuid(uint32_t uId)
{
    auto iter = m_pComponentToTypeMap->find(uId);
    BEATS_ASSERT(iter != m_pComponentToTypeMap->end(), _T("Query component guid failed of id %d"), uId);
    return iter->second;
}

std::map<uint32_t, std::vector<uint32_t> >* CComponentProject::GetFileToComponentMap() const
{
    return m_pFileToComponentMap;
}

std::map<uint32_t, std::set<uint32_t>>* CComponentProject::GetIdToReferenceMap() const
{
    return m_pIdToReferenceMap;
}

std::map<uint32_t, uint32_t>* CComponentProject::GetReferenceMap()
{
    return m_pReferenceToIdMap;
}

std::map<uint32_t, CComponentProjectDirectory*>* CComponentProject::GetFileToDirectoryMap() const
{
    return m_pFileToDirectoryMap;
}

std::map<uint32_t, uint32_t>* CComponentProject::GetComponentToFileMap() const
{
    return m_pComponentToFileMap;
}

std::map<uint32_t, std::vector<uint32_t> >* CComponentProject::GetTypeToComponentMap() const
{
    return m_pTypeToComponentMap;
}

uint32_t CComponentProject::RegisterFile(CComponentProjectDirectory* pDirectory, const TString& strFileName, std::map<uint32_t, std::vector<uint32_t> >& failedId, uint32_t uSpecifyFileId/* = 0xFFFFFFFF*/)
{
    uint32_t uFileID = 0xFFFFFFFF;
    if (uSpecifyFileId != 0xFFFFFFFF)
    {
        TString strExistsFileName = GetComponentFileName(uSpecifyFileId);
        bool bValidId = strExistsFileName.length() == 0;
        BEATS_ASSERT(bValidId, _T("Can't Specify a file pos %d for file %s, there is alreay a file %s"), 
            uSpecifyFileId, strFileName.c_str(), strExistsFileName.c_str());
        if (bValidId)
        {
            (*m_pComponentFiles)[uSpecifyFileId].assign(strFileName);
            uFileID = uSpecifyFileId;
        }
    }
    else
    {
        m_pComponentFiles->push_back(strFileName);
        uFileID = (uint32_t)m_pComponentFiles->size() - 1;
    }

    std::map<uint32_t, std::vector<uint32_t> > result;
    AnalyseFile(strFileName, result);

    CIdManager* pIdManager = CComponentProxyManager::GetInstance()->GetIdManager();

    for (std::map<uint32_t, std::vector<uint32_t> >::const_iterator subIter = result.begin(); subIter != result.end(); ++subIter)
    {
        for (uint32_t i = 0; i < subIter->second.size(); ++i)
        {
            uint32_t id = subIter->second[i];
            if (!pIdManager->ReserveId(id, false))
            {
                if (failedId.find(id) == failedId.end())
                {
                    failedId[id] = std::vector<uint32_t>();
                    uint32_t uOriginalPosOfFile = 0xFFFFFFFF;
                    // If we can't find the conflict id in the static records, it must be in the dynamic records.
                    if (m_pComponentToFileMap->find(id) == m_pComponentToFileMap->end())
                    {
                        BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentInstance(id) != NULL, _T("It's impossible that can't find the component id in neither static and dynamic records."));
                        uOriginalPosOfFile = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
                    }
                    else
                    {
                        uOriginalPosOfFile = (*m_pComponentToFileMap)[id];
                    }
                    BEATS_ASSERT(uOriginalPosOfFile != 0xFFFFFFFF, _T("Can't find which file is the component in with id %d"), id);
                    failedId[id].push_back(uOriginalPosOfFile);
                }
                failedId[id].push_back(uFileID);
            }
            else
            {
                RegisterComponent(uFileID, subIter->first, id);
            }
        }
    }
    BEATS_ASSERT(m_pFileToDirectoryMap->find(uFileID) == m_pFileToDirectoryMap->end());
    (*m_pFileToDirectoryMap)[uFileID] = pDirectory;
    return uFileID;
}

bool CComponentProject::AnalyseFile(const TString& strFileName, std::map<uint32_t, std::vector<uint32_t> >& outResult)
{
    outResult.clear();
    TiXmlDocument document(strFileName.c_str());
    bool bLoadSuccess = document.LoadFile(TIXML_ENCODING_UTF8);
    if (bLoadSuccess)
    {
        TiXmlElement* pRootElement = document.RootElement();
        TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
        if (pComponentListNode != NULL )
        {
            TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
            while (pComponentElement != NULL)
            {
                const char* pszGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                uint32_t uComponentGuid = strtoul(pszGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == 0, _T("Guid value %s is not a 0x value at file %s."), pszGuidStr, strFileName.c_str());
                if (outResult.find(uComponentGuid) == outResult.end())
                {
                    outResult[uComponentGuid] = std::vector<uint32_t>();
                }
                std::vector<uint32_t>& idList = outResult[uComponentGuid];
                TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement();
                while (pInstanceElement != NULL)
                {
                    bool bIsReference = strcmp(pInstanceElement->Value(), "Reference") == 0;
                    bool bFindProxy = strcmp(pInstanceElement->Value(), "Instance") == 0 || bIsReference;
                    BEATS_ASSERT(bFindProxy, _T("Read invalid data!"));
                    if (bFindProxy)
                    {
                        int id = -1;
                        pInstanceElement->Attribute("Id", &id);
                        BEATS_ASSERT(id != -1);
                        idList.push_back(id);
                        if (bIsReference)
                        {
                            int proxyId = -1;
                            pInstanceElement->Attribute("ReferenceId", &proxyId);
                            BEATS_ASSERT(proxyId != -1);
                            if (m_pIdToReferenceMap->find(proxyId) == m_pIdToReferenceMap->end())
                            {
                                (*m_pIdToReferenceMap)[proxyId] = std::set<uint32_t>();
                            }
                            BEATS_ASSERT((*m_pIdToReferenceMap)[proxyId].find(id) == (*m_pIdToReferenceMap)[proxyId].end(), _T("Get repeat reference id %d"), id);
                            (*m_pIdToReferenceMap)[proxyId].insert(id);
                            BEATS_ASSERT(m_pReferenceToIdMap->find(id) == m_pReferenceToIdMap->end(), _T("Get repeat reference id %d"), id);
                            (*m_pReferenceToIdMap)[id] = proxyId;
                        }
                    }
                    pInstanceElement = pInstanceElement->NextSiblingElement();
                }
                pComponentElement = pComponentElement->NextSiblingElement("Component");
            }
        }
    }
    else
    {
        TCHAR reason[MAX_PATH];
        _stprintf(reason, _T("%s %s row : %d col : %d"), document.ErrorDesc(), strFileName.c_str(), document.ErrorRow(), document.ErrorCol());
        BEATS_ASSERT(false, _T("Load File :%s Failed!Reason: %s "), strFileName.c_str(), reason);
    }
    return true;
}

bool CComponentProject::UnregisterFile(uint32_t uFileId)
{
    bool bRet = true;
    (*m_pComponentFiles)[uFileId].clear(); // Don't erase it, we must keep the order un-change.
    std::vector<uint32_t>& componentIds = (*m_pFileToComponentMap)[uFileId];
    CIdManager* pIdManager = CComponentProxyManager::GetInstance()->GetIdManager();
    for (uint32_t i = 0; i < componentIds.size(); ++i)
    {
        uint32_t uComponentId = componentIds[i];
        if (!pIdManager->IsIdFree(uComponentId))
        {
            pIdManager->RecycleId(uComponentId);
        }
        uint32_t uGuid = (*m_pComponentToTypeMap)[uComponentId];
        std::vector<uint32_t>& componentIdsOfType = (*m_pTypeToComponentMap)[uGuid];
        m_pComponentToFileMap->erase(uComponentId);
        m_pComponentToTypeMap->erase(uComponentId);
        for (uint32_t i = 0; i < componentIdsOfType.size(); ++i)
        {
            if (componentIdsOfType[i] == uComponentId)
            {
                componentIdsOfType[i] = componentIdsOfType.back();
                componentIdsOfType.pop_back();
                break;
            }
        }
        auto refIter = m_pReferenceToIdMap->find(uComponentId);
        bool bIsReferenceComponent = refIter != m_pReferenceToIdMap->end();
        if (bIsReferenceComponent)
        {
            uint32_t uProxyId = refIter->second;
            auto refListIter = m_pIdToReferenceMap->find(uProxyId);
            BEATS_ASSERT(refListIter != m_pIdToReferenceMap->end() && refListIter->second.find(uComponentId) != refListIter->second.end());
            refListIter->second.erase(uComponentId);
            if (refListIter->second.size() == 0)
            {
                m_pIdToReferenceMap->erase(uProxyId);
            }
            m_pReferenceToIdMap->erase(uComponentId);
        }
    }
    m_pFileToComponentMap->erase(uFileId);
    BEATS_ASSERT(m_pFileToDirectoryMap->find(uFileId) != m_pFileToDirectoryMap->end());
    m_pFileToDirectoryMap->erase(uFileId);
    return bRet;
}

void CComponentProject::ReloadFile(uint32_t uFileID)
{
    TString strFileName = GetComponentFileName(uFileID);
    BEATS_ASSERT(m_pFileToDirectoryMap->find(uFileID) != m_pFileToDirectoryMap->end());
    CComponentProjectDirectory* pDirectory = (*m_pFileToDirectoryMap)[uFileID];
    bool bRet = UnregisterFile(uFileID);
    BEATS_ASSERT(bRet && strFileName.length() > 0);
    if (bRet && strFileName.length() > 0)
    {
        std::map<uint32_t, std::vector<uint32_t> > failedId;
        RegisterFile(pDirectory, strFileName, failedId, uFileID);
        BEATS_ASSERT(failedId.size() == 0, _T("Impossible to have any failture here!"));
    }
}

CComponentProjectDirectory* CComponentProject::GetRootDirectory() const
{
    return m_pProjectDirectory;
}

void CComponentProject::SetRootDirectory(CComponentProjectDirectory* pDirectory)
{
    m_pProjectDirectory = pDirectory;
}

const TString& CComponentProject::GetProjectFilePath() const
{
    return m_strProjectFilePath;
}

const TString& CComponentProject::GetProjectFileName() const
{
    return m_strProjectFileName;
}

const TString& CComponentProject::GetComponentFileName(uint32_t id) const
{
    BEATS_ASSERT(id < m_pComponentFiles->size(), _T("Invalid id %d in CComponentProject::GetComponentFileName"), id);
    return (*m_pComponentFiles)[id];
}

uint32_t CComponentProject::GetComponentFileId(const TString& strFileName) const
{
    uint32_t uRet = 0xFFFFFFFF;
    if (strFileName.length() > 0)
    {
        for (uint32_t i = 0; i < m_pComponentFiles->size(); ++i)
        {
            if (m_pComponentFiles->at(i).compare(strFileName) == 0)
            {
                uRet = i;
                break;
            }
        }
    }

    return uRet;
}

uint32_t CComponentProject::QueryFileId(uint32_t uComponentId, bool bOnlyInProjectFile)
{
    uint32_t uRet = 0xFFFFFFFF;

    // 1. Get the info from static records (in project).
    std::map<uint32_t, uint32_t>::iterator iter = m_pComponentToFileMap->find(uComponentId);
    if (iter != m_pComponentToFileMap->end())
    {
        uRet = iter->second;
    }
    if (!bOnlyInProjectFile)
    {
        // 2. If the component is in the file which we are working on, research it since its dynamic records.
        const std::map<uint32_t, CComponentProxy*>& componentsInScene = CComponentProxyManager::GetInstance()->GetComponentsInCurScene();
        bool bInCurrentWorkingFile = componentsInScene.find(uComponentId) != componentsInScene.end();
        if (bInCurrentWorkingFile)
        {
            // Can't find the data in static records since we may add the new component dynamically OR
            // Find the data in static records but we have delete it dynamically.
            uRet = CComponentProxyManager::GetInstance()->GetCurrentViewFileId();
            BEATS_ASSERT(uRet != 0xFFFFFFFF);
            BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId) != NULL);
        }
    }
    return uRet;
}

uint32_t CComponentProject::GetTotalFileCount(TiXmlElement* pNode) const
{
    uint32_t uRet = 0;
    TiXmlElement* pElement = pNode->FirstChildElement();
    while (pElement != NULL)
    {
        const char* pText = pElement->Value();
        if (strcmp(pText, "Directory") == 0)
        {
            uRet += GetTotalFileCount(pElement);
        }
        else if (strcmp(pText, "File") == 0)
        {
            uRet += 1;
        }
        else if (strcmp(pText, "StartFile") == 0)
        {
        }
        else
        {
            BEATS_ASSERT(false);
        }
        pElement = pElement->NextSiblingElement();
    }
    return uRet;
}

void CComponentProject::LoadXMLProject(TiXmlElement* pNode, CComponentProjectDirectory* pProjectDirectory, TString& strStartFilePath, std::map<uint32_t, std::vector<uint32_t> >& conflictIdMap)
{
    BEATS_ASSERT(pProjectDirectory != NULL && pNode != NULL);
    TiXmlElement* pElement = pNode->FirstChildElement();
    while (pElement != NULL)
    {
        const char* pText = pElement->Value();
        if (strcmp(pText, "Directory") == 0)
        {
            const char* pName = pElement->Attribute("Name");
            CComponentProjectDirectory* pNewProjectFile = new CComponentProjectDirectory(pProjectDirectory, pName);
            LoadXMLProject(pElement, pNewProjectFile, strStartFilePath, conflictIdMap);
        }
        else if (strcmp(pText, "File") == 0)
        {
            const char* pPath = pElement->Attribute("Path");
            m_strCurrLoadingFile = CFilePathTool::GetInstance()->MakeAbsolute(m_strProjectFilePath.c_str(), pPath);
            pProjectDirectory->AddFile(m_strCurrLoadingFile.c_str(), conflictIdMap);
            ++m_uLoadedFileCount;
            (*m_pFileToDirectoryMap)[(uint32_t)m_pComponentFiles->size() - 1] = pProjectDirectory;
        }
        else if (strcmp(pText, "StartFile") == 0)
        {
            const char* pszStartFilePath = pElement->Attribute("FilePath");
            BEATS_ASSERT(pszStartFilePath != NULL);
            strStartFilePath = CFilePathTool::GetInstance()->MakeAbsolute(m_strProjectFilePath.c_str(), pszStartFilePath);
        }
        else
        {
            BEATS_ASSERT(false);
        }
        pElement = pElement->NextSiblingElement();
    }
}

void CComponentProject::RegisterComponent(uint32_t uFileID, uint32_t uComponentGuid, uint32_t uComponentId)
{
    if (uFileID == 0xFFFFFFFF)
    {
        uFileID = CComponentProxyManager::GetInstance()->GetCurrLoadFileId();
    }
    if (uFileID != 0xFFFFFFFF)
    {
        // 1. Add in component -> file map.
        BEATS_ASSERT(m_pComponentToFileMap->find(uComponentId) == m_pComponentToFileMap->end(), _T("An id can't be requested successfully twice!"));
        (*m_pComponentToFileMap)[uComponentId] = uFileID;

        // 2. Add in component -> type map.
        BEATS_ASSERT(m_pComponentToTypeMap->find(uComponentId) == m_pComponentToTypeMap->end(), _T("An id can't be requested successfully twice!"));
        (*m_pComponentToTypeMap)[uComponentId] = uComponentGuid;

        // 3. Add in type -> component map.
        if (m_pTypeToComponentMap->find(uComponentGuid) == m_pTypeToComponentMap->end())
        {
            (*m_pTypeToComponentMap)[uComponentGuid] = std::vector<uint32_t>();
        }
        (*m_pTypeToComponentMap)[uComponentGuid].push_back(uComponentId);

        // 4. Add in file - > component map.
        if (m_pFileToComponentMap->find(uFileID) == m_pFileToComponentMap->end())
        {
            (*m_pFileToComponentMap)[uFileID] = std::vector<uint32_t>();
        }
        (*m_pFileToComponentMap)[uFileID].push_back(uComponentId);
    }
}

void CComponentProject::UnregisterComponent(uint32_t uComponentId)
{
    std::map<uint32_t, uint32_t>::iterator iter = m_pComponentToFileMap->find(uComponentId);
    bool bFindFileRecord = iter != m_pComponentToFileMap->end();
    BEATS_ASSERT(bFindFileRecord, _T("Can't find the component by id %d"), uComponentId);

    if (bFindFileRecord)
    {
        uint32_t uFileId = iter->second;
        // 1. Erase in component -> file map.
        m_pComponentToFileMap->erase(iter);

        // 2. Erase in File -> component map.
        std::vector<uint32_t>& idInFile = (*m_pFileToComponentMap)[uFileId];
        for (uint32_t i = 0; i < idInFile.size(); ++i)
        {
            if (idInFile[i] == uComponentId)
            {
                idInFile[i] = idInFile.back();
                idInFile.pop_back();
                break;
            }
        }

        // 3. Erase in component -> type map.
        std::map<uint32_t, uint32_t>::iterator componentToTypeiter = m_pComponentToTypeMap->find(uComponentId);
        uint32_t uGuid = componentToTypeiter->second;
        BEATS_ASSERT(componentToTypeiter != m_pComponentToTypeMap->end(), _T("Can't find component id %d in Component To type map!"), uComponentId);
        m_pComponentToTypeMap->erase(componentToTypeiter);

        // 4. Erase in Type -> Component map.
        std::vector<uint32_t>& idOfGuid = (*m_pTypeToComponentMap)[uGuid];
        for (uint32_t i = 0; i < idOfGuid.size(); ++i)
        {
            if (idOfGuid[i] == uComponentId)
            {
                idOfGuid[i] = idOfGuid.back();
                idOfGuid.pop_back();
                break;
            }
        }
    }
}
