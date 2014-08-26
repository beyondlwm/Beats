#include "stdafx.h"
#include "ComponentProject.h"
#include "ComponentProjectDirectory.h"
#include "ComponentProxyManager.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/IdManager/IdManager.h"
#include <string>
#include "Utility/UtilityManager.h"
#include "FilePath/FilePathTool.h"

CComponentProject::CComponentProject()
: m_pProjectDirectory(NULL)
, m_pStartDirectory(NULL)
, m_pComponentFiles(new std::vector<TString>)
, m_pComponentToTypeMap(new std::map<size_t, size_t>)
, m_pComponentToFileMap(new std::map<size_t, size_t>)
, m_pFileToComponentMap(new std::map<size_t, std::vector<size_t> >)
, m_pTypeToComponentMap(new std::map<size_t, std::vector<size_t> >)
, m_pPropertyMaintainMap(new std::map<size_t, std::map<TString, TString> >)
, m_pFileDataLayout(new std::map<size_t, SFileDataLayout>)
{

}

CComponentProject::~CComponentProject()
{
    BEATS_SAFE_DELETE(m_pProjectDirectory);
    BEATS_SAFE_DELETE(m_pComponentFiles);
    BEATS_SAFE_DELETE(m_pComponentToTypeMap);
    BEATS_SAFE_DELETE(m_pComponentToFileMap);
    BEATS_SAFE_DELETE(m_pFileToComponentMap);
    BEATS_SAFE_DELETE(m_pTypeToComponentMap);
    BEATS_SAFE_DELETE(m_pPropertyMaintainMap);
    BEATS_SAFE_DELETE(m_pFileDataLayout);
}

CComponentProjectDirectory* CComponentProject::LoadProject(const TCHAR* pszProjectFile, std::map<size_t, std::vector<size_t> >& conflictIdMap)
{
    CloseProject();
    BEATS_ASSERT(CFilePathTool::GetInstance()->IsAbsolute(pszProjectFile), _T("Only accept absolute path!"));
    if (pszProjectFile != NULL && pszProjectFile[0] != 0)
    {
        m_strProjectFilePath = CFilePathTool::GetInstance()->ParentPath(pszProjectFile);
        m_strProjectFileName = CFilePathTool::GetInstance()->FileName(pszProjectFile);

        char szProjectFileChar[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToCHAR(pszProjectFile, szProjectFileChar, MAX_PATH);

        TiXmlDocument document;
        bool bLoadSuccess = document.LoadFile(szProjectFileChar);
        if (bLoadSuccess)
        {
            TiXmlElement* pRootElement = document.RootElement();
            TCHAR szTName[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pRootElement->Value(), szTName, MAX_PATH);
            m_pProjectDirectory = new CComponentProjectDirectory(NULL, szTName);
            TString strStartLogicPath;
            LoadXMLProject(pRootElement, m_pProjectDirectory, strStartLogicPath, conflictIdMap);
            m_pStartDirectory = FindProjectDirectory(strStartLogicPath);
            BEATS_ASSERT(m_pStartDirectory);
        }
        else
        {
            TCHAR info[MAX_PATH];
            TCHAR reason[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(document.ErrorDesc(), reason, MAX_PATH);
            _stprintf(info, _T("Load File :%s Failed!Reason:%s"), pszProjectFile, reason);
            MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
    }
    return m_pProjectDirectory;
}

bool CComponentProject::CloseProject()
{
    bool bRet = false;
    CComponentProxyManager::GetInstance()->CloseFile();
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
    TiXmlElement* pStartDirectoryNode = new TiXmlElement("StartDirectory");
    TString strStartLogicPath = m_pStartDirectory->GenerateLogicPath();
    char szBuffer[10240];
    CStringHelper::GetInstance()->ConvertToCHAR(strStartLogicPath.c_str(), szBuffer, MAX_PATH);
    pStartDirectoryNode->SetAttribute("Path", szBuffer);
    pRootElement->LinkEndChild(pStartDirectoryNode);

    SaveProjectFile(pRootElement, m_pProjectDirectory);
    TString strFullPath = m_strProjectFilePath;
    strFullPath.append(_T("/")).append(m_strProjectFileName);
    CStringHelper::GetInstance()->ConvertToCHAR(strFullPath.c_str(), szBuffer, MAX_PATH);
    document.SaveFile(szBuffer);
}

void CComponentProject::SaveProjectFile( TiXmlElement* pParentNode, const CComponentProjectDirectory* p)
{
    //Root doesn't need to save/load.
    TiXmlElement* pNewDirectoryElement = pParentNode;
    if (p != m_pProjectDirectory)
    {
        pNewDirectoryElement = new TiXmlElement("Directory");
        char szName[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToCHAR(p->GetName().c_str(), szName, MAX_PATH);
        pNewDirectoryElement->SetAttribute("Name", szName);
        pParentNode->LinkEndChild(pNewDirectoryElement);
    }

    const std::vector<CComponentProjectDirectory*>& children = p->GetChildren();
    for (std::vector<CComponentProjectDirectory*>::const_iterator iter = children.begin(); iter != children.end(); ++iter)
    {
        SaveProjectFile(pNewDirectoryElement, *iter);
    }

    const std::vector<size_t>& files = p->GetFileList();

    for (std::vector<size_t>::const_iterator iter = files.begin(); iter != files.end(); ++iter)
    {
        TiXmlElement* pNewFileElement = new TiXmlElement("File");
        size_t uFileNameId = *iter;

        TString strRelativePath = CFilePathTool::GetInstance()->MakeRelative(m_strProjectFilePath.c_str(), GetComponentFileName(uFileNameId).c_str());
        char szCharPath[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToCHAR(strRelativePath.c_str(), szCharPath, MAX_PATH);
        pNewFileElement->SetAttribute("Path", szCharPath);
        pNewDirectoryElement->LinkEndChild(pNewFileElement);
    }
}

void CComponentProject::ResolveIdForFile(size_t uFileId, size_t idToResolve, bool bKeepId)
{
    if (bKeepId)
    {
        BEATS_ASSERT(m_pComponentToFileMap->find(idToResolve) != m_pComponentToFileMap->end());
        size_t uExistInFileId = (*m_pComponentToFileMap)[idToResolve];
        if (uExistInFileId != uFileId)
        {
            size_t uGuid = (*m_pComponentToTypeMap)[idToResolve];
            UnregisterComponent(idToResolve);
            RegisterComponent(uFileId, uGuid, idToResolve);
        }
    }
    else
    {
        char tmp[MAX_PATH] = {0};
        TString strFileName = GetComponentFileName(uFileId);
        const TCHAR* pszFilePath = strFileName.c_str();
        CStringHelper::GetInstance()->ConvertToCHAR(pszFilePath, tmp, MAX_PATH);
        TiXmlDocument document(tmp);
        bool loadSuccess = document.LoadFile(TIXML_ENCODING_LEGACY);
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
                size_t uComponentGuid = strtoul(pszGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == NULL, _T("Guid value %s is not a 0x value at file %s."), pszGuidStr, strFileName.c_str());
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
            TCHAR reason[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(document.ErrorDesc(), reason, MAX_PATH);
            _stprintf(info, _T("Load File :%s Failed!Reason: %s"), pszFilePath, reason);
            MessageBox(NULL, info, _T("Load File Failed"), MB_OK | MB_ICONERROR);
        }
    }
}

void CComponentProject::RegisterPropertyMaintainInfo(size_t uComponentGuid, const TString& strOriginPropertyName, const TString& strReplacePropertyName)
{
    std::map<size_t, std::map<TString, TString> >::iterator iter = m_pPropertyMaintainMap->find(uComponentGuid);
    if (iter == m_pPropertyMaintainMap->end())
    {
        (*m_pPropertyMaintainMap)[uComponentGuid] = std::map<TString, TString>();
        iter = m_pPropertyMaintainMap->find(uComponentGuid);
    }
    BEATS_ASSERT(iter->second.find(strOriginPropertyName) == iter->second.end(), _T("The property %s has already been registered for maintain!"), strOriginPropertyName.c_str());
    (iter->second)[strOriginPropertyName] = strReplacePropertyName;
}

bool CComponentProject::GetReplacePropertyName(size_t uComponentGuid, const TString& strOriginPropertyName, TString& strResult)
{
    bool bRet = false;
    std::map<size_t, std::map<TString, TString> >::iterator iter = m_pPropertyMaintainMap->find(uComponentGuid);
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

void CComponentProject::RegisterFileLayoutInfo(size_t uFileId, size_t uStartPos, size_t uDataLength)
{
    BEATS_ASSERT(m_pFileDataLayout->find(uFileId) == m_pFileDataLayout->end(), _T("File layout info already exists! id %d"), uFileId);
    (*m_pFileDataLayout)[uFileId] = SFileDataLayout(uStartPos, uDataLength);
}

CComponentProjectDirectory* CComponentProject::GetLaunchStartDirectory() const
{
    return m_pStartDirectory;
}

void CComponentProject::SetLaunchStartDirectory(CComponentProjectDirectory* pDirectory)
{
    m_pStartDirectory = pDirectory;
}

CComponentProjectDirectory* CComponentProject::FindProjectDirectory(const TString& strLogicPath) const
{
    std::vector<TString> vecDirectories;
    CStringHelper::GetInstance()->SplitString(strLogicPath.c_str(), _T("/"), vecDirectories);
    CComponentProjectDirectory* pCurDirectory = m_pProjectDirectory;
    //Ignore the root, so start from 1.
    for (size_t i = 1; i < vecDirectories.size(); ++i)
    {
        pCurDirectory = pCurDirectory->FindChild(vecDirectories[i].c_str());
        BEATS_ASSERT(pCurDirectory != NULL);
    }
    return pCurDirectory;
}

size_t CComponentProject::RegisterFile(const TString& strFileName, std::map<size_t, std::vector<size_t> >& failedId, size_t uSpecifyFileId/* = 0xFFFFFFFF*/)
{
    size_t uFileID = 0xFFFFFFFF;
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
        uFileID = m_pComponentFiles->size() - 1;
    }

    std::map<size_t, std::vector<size_t> > result;
    AnalyseFile(strFileName, result);

    CIdManager* pIdManager = CComponentProxyManager::GetInstance()->GetIdManager();

    for (std::map<size_t, std::vector<size_t> >::const_iterator subIter = result.begin(); subIter != result.end(); ++subIter)
    {
        for (size_t i = 0; i < subIter->second.size(); ++i)
        {
            size_t id = subIter->second[i];
            if (!pIdManager->ReserveId(id, false))
            {
                if (failedId.find(id) == failedId.end())
                {
                    failedId[id] = std::vector<size_t>();
                    size_t uOriginalPosOfFile = 0xFFFFFFFF;
                    // If we can't find the conflict id in the static records, it must be in the dynamic records.
                    if (m_pComponentToFileMap->find(id) == m_pComponentToFileMap->end())
                    {
                        BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetComponentInstance(id) != NULL, _T("It's impossible that can't find the component id in neither static and dynamic records."));
                        uOriginalPosOfFile = GetComponentFileId(CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath());
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
    return uFileID;
}

bool CComponentProject::AnalyseFile(const TString& strFileName, std::map<size_t, std::vector<size_t> >& outResult)
{
    outResult.clear();
    char szFileName[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToCHAR(strFileName.c_str(), szFileName, MAX_PATH);
    TiXmlDocument document(szFileName);
    bool bLoadSuccess = document.LoadFile(TIXML_ENCODING_LEGACY);
    if (bLoadSuccess)
    {
        TiXmlElement* pRootElement = document.RootElement();
        TiXmlElement* pComponentListNode = pRootElement->FirstChildElement("Components");
        if (pComponentListNode != NULL )
        {
            TiXmlElement* pComponentElement = pComponentListNode->FirstChildElement("Component");
            if (pComponentElement != NULL)
            {
                const char* pszGuidStr = pComponentElement->Attribute("GUID");
                char* pStopPos = NULL;
                size_t uComponentGuid = strtoul(pszGuidStr, &pStopPos, 16);
                BEATS_ASSERT(*pStopPos == NULL, _T("Guid value %s is not a 0x value at file %s."), pszGuidStr, strFileName.c_str());
                if (outResult.find(uComponentGuid) == outResult.end())
                {
                    outResult[uComponentGuid] = std::vector<size_t>();
                }
                std::vector<size_t>& idList = outResult[uComponentGuid];
                while (pComponentElement != NULL)
                {
                    TiXmlElement* pInstanceElement = pComponentElement->FirstChildElement("Instance");
                    while (pInstanceElement != NULL)
                    {
                        int id = -1;
                        pInstanceElement->Attribute("Id", &id);
                        BEATS_ASSERT(id != -1);
                        idList.push_back(id);
                        pInstanceElement = pInstanceElement->NextSiblingElement("Instance");
                    }
                    pComponentElement = pComponentElement->NextSiblingElement("Component");
                }
            }
        }
    }
    else
    {
        TCHAR reason[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(document.ErrorDesc(), reason, MAX_PATH);
        BEATS_ASSERT(false, _T("Load File :%s Failed!Reason: %s"), strFileName.c_str(), reason);
    }
    return true;
}

bool CComponentProject::UnregisterFile(size_t uFileId)
{
    bool bRet = true;
    (*m_pComponentFiles)[uFileId].clear(); // Don't erase it, we must keep the order un-change.
    std::vector<size_t>& componentIds = (*m_pFileToComponentMap)[uFileId];
    CIdManager* pIdManager = CComponentProxyManager::GetInstance()->GetIdManager();
    for (size_t i = 0; i < componentIds.size(); ++i)
    {
        size_t uComponentId = componentIds[i];
        if (!pIdManager->IsIdFree(uComponentId))
        {
            pIdManager->RecycleId(uComponentId);
        }
        size_t uGuid = (*m_pComponentToTypeMap)[uComponentId];
        std::vector<size_t>& componentIdsOfType = (*m_pTypeToComponentMap)[uGuid];
        m_pComponentToFileMap->erase(uComponentId);
        m_pComponentToTypeMap->erase(uComponentId);
        for (size_t i = 0; i < componentIdsOfType.size(); ++i)
        {
            if (componentIdsOfType[i] == uComponentId)
            {
                componentIdsOfType[i] = componentIdsOfType.back();
                componentIdsOfType.pop_back();
                break;
            }
        }
    }
    m_pFileToComponentMap->erase(uFileId);
    return bRet;
}

void CComponentProject::ReloadFile(size_t uFileID)
{
    TString strFileName = GetComponentFileName(uFileID);
    bool bRet = UnregisterFile(uFileID);
    BEATS_ASSERT(bRet && strFileName.length() > 0);
    if (bRet && strFileName.length() > 0)
    {
        std::map<size_t, std::vector<size_t> > failedId;
        RegisterFile(strFileName, failedId, uFileID);
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

const TString& CComponentProject::GetComponentFileName(size_t id) const
{
    BEATS_ASSERT(id < m_pComponentFiles->size(), _T("Invalid id %d in CComponentProject::GetComponentFileName"), id);
    return (*m_pComponentFiles)[id];
}

size_t CComponentProject::GetComponentFileId(const TString& strFileName) const
{
    size_t uRet = 0xFFFFFFFF;
    if (strFileName.length() > 0)
    {
        for (size_t i = 0; i < m_pComponentFiles->size(); ++i)
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

size_t CComponentProject::QueryFileId(size_t uComponentId, bool bOnlyInProjectFile)
{
    size_t uRet = 0xFFFFFFFF;
    
    // 1. Get the info from static records (in project).
    std::map<size_t, size_t>::iterator iter = m_pComponentToFileMap->find(uComponentId);
    if (iter != m_pComponentToFileMap->end())
    {
        uRet = iter->second;
    }
    if (!bOnlyInProjectFile)
    {
        // 2. If the component is in the file which we are working on, research it since its dynamic records.
        size_t uCurrentWorkingFileID = GetComponentFileId(CComponentProxyManager::GetInstance()->GetCurrentWorkingFilePath());
        if (uCurrentWorkingFileID != 0xFFFFFFFF)
        {
            if (uRet == 0xFFFFFFFF || uRet == uCurrentWorkingFileID)
            {
                CComponentBase* pComponent = CComponentProxyManager::GetInstance()->GetComponentInstance(uComponentId);
                // Can't find the data in static records since we may add the new component dynamically OR
                // Find the data in static records but we have delete it dynamically.
                uRet = pComponent != NULL ? uCurrentWorkingFileID : 0xFFFFFFFF;
            }
        }
    }

    return uRet;
}

void CComponentProject::LoadXMLProject(TiXmlElement* pNode, CComponentProjectDirectory* pProjectDirectory, TString& startLogicPath, std::map<size_t, std::vector<size_t> >& conflictIdMap)
{
    BEATS_ASSERT(pProjectDirectory != NULL && pNode != NULL);
    TiXmlElement* pElement = pNode->FirstChildElement();
    while (pElement != NULL)
    {
        const char* pText = pElement->Value();
        if (strcmp(pText, "Directory") == 0)
        {
            const char* pName = pElement->Attribute("Name");
            TCHAR szTName[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pName, szTName, MAX_PATH);
            CComponentProjectDirectory* pNewProjectFile = new CComponentProjectDirectory(pProjectDirectory, szTName);
            LoadXMLProject(pElement, pNewProjectFile, startLogicPath, conflictIdMap);
        }
        else if (strcmp(pText, "File") == 0)
        {
            const char* pPath = pElement->Attribute("Path");
            TCHAR szTPath[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pPath, szTPath, MAX_PATH);
            TString strFilePath = CFilePathTool::GetInstance()->MakeAbsolute(m_strProjectFilePath.c_str(), szTPath);
            pProjectDirectory->AddFile(strFilePath.c_str(), conflictIdMap);
        }
        else if (strcmp(pText, "StartDirectory") == 0)
        {
            const char* pPath = pElement->Attribute("Path");
            TCHAR szTPath[MAX_PATH];
            CStringHelper::GetInstance()->ConvertToTCHAR(pPath, szTPath, MAX_PATH);
            startLogicPath = szTPath;
        }
        else
        {
            BEATS_ASSERT(false);
        }
        pElement = pElement->NextSiblingElement();
    }
}

void CComponentProject::RegisterComponent(size_t uFileID, size_t uComponentGuid, size_t uComponentId)
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
        (*m_pTypeToComponentMap)[uComponentGuid] = std::vector<size_t>();
    }
    (*m_pTypeToComponentMap)[uComponentGuid].push_back(uComponentId);

    // 4. Add in file - > component map.
    if (m_pFileToComponentMap->find(uFileID) == m_pFileToComponentMap->end())
    {
        (*m_pFileToComponentMap)[uFileID] = std::vector<size_t>();
    }
    (*m_pFileToComponentMap)[uFileID].push_back(uComponentId);
}

void CComponentProject::UnregisterComponent(size_t uComponentId)
{
    std::map<size_t, size_t>::iterator iter = m_pComponentToFileMap->find(uComponentId);
    bool bFindFileRecord = iter != m_pComponentToFileMap->end();
    BEATS_ASSERT(bFindFileRecord, _T("Can't find the component by id %d"), uComponentId);

    if (bFindFileRecord)
    {
        size_t uFileId = iter->second;
        // 1. Erase in component -> file map.
        m_pComponentToFileMap->erase(iter);

        // 2. Erase in File -> component map.
        std::vector<size_t>& idInFile = (*m_pFileToComponentMap)[uFileId];
        for (size_t i = 0; i < idInFile.size(); ++i)
        {
            if (idInFile[i] == uComponentId)
            {
                idInFile[i] = idInFile.back();
                idInFile.pop_back();
                break;
            }
        }

        // 3. Erase in component -> type map.
        std::map<size_t, size_t>::iterator componentToTypeiter = m_pComponentToTypeMap->find(uComponentId);
        size_t uGuid = componentToTypeiter->second;
        BEATS_ASSERT(componentToTypeiter != m_pComponentToTypeMap->end(), _T("Can't find component id %d in Component To type map!"), uComponentId);
        m_pComponentToTypeMap->erase(componentToTypeiter);

        // 4. Erase in Type -> Component map.
        std::vector<size_t>& idOfGuid = (*m_pTypeToComponentMap)[uGuid];
        for (size_t i = 0; i < idOfGuid.size(); ++i)
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
