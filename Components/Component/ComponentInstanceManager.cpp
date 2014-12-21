#include "stdafx.h"
#include "ComponentInstanceManager.h"
#include "ComponentBase.h"
#include "ComponentProxy.h"
#include "Utility/Serializer/Serializer.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/UtilityManager.h"
#include "Utility/IdManager/IdManager.h"
#include "ComponentProjectDirectory.h"
#include "ComponentProject.h"

CComponentInstanceManager* CComponentInstanceManager::m_pInstance = NULL;

CComponentInstanceManager::CComponentInstanceManager()
    : m_bInClonePhase(false)
    , m_uCurLoadFileId(0xFFFFFFFF)
    , m_uCurWorkingFileId(0xFFFFFFFF)
{
    m_pSerializer = new CSerializer;
}

CComponentInstanceManager::~CComponentInstanceManager()
{
    Release();
    // Don't remove GetInstance(), it is force to create an instance of CUtilityManager, so we can visit the member of it.
    CUtilityManager::GetInstance();
    CUtilityManager::Destroy();
    BEATS_SAFE_DELETE(m_pSerializer);
}

CSerializer* CComponentInstanceManager::Import(const TCHAR* pszFilePath)
{
    m_pSerializer->Serialize(pszFilePath);
    CSerializer& serializer = *m_pSerializer;
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
        size_t uComponentTemplateCount = 0;
        serializer >> uComponentTemplateCount;
        CComponentProjectDirectory* pRootDirectory = new CComponentProjectDirectory(NULL, _T("Root"));
        m_pProject->SetRootDirectory(pRootDirectory);
        pRootDirectory->Deserialize(serializer);
        size_t uStartFile;
        serializer >> uStartFile;
        m_pProject->SetStartFile(uStartFile);
        // 1. Load binarize data and file sturcture.
        size_t uFileCount = 0;
        serializer >> uFileCount;
        for (size_t i = 0; i < uFileCount; ++i)
        {
            size_t uStartPos = 0;
            serializer >> uStartPos;
            BEATS_ASSERT(uStartPos == (serializer.GetReadPos() - sizeof(uStartPos)), _T("File start pos not match!"));
            size_t uFileSize = 0;
            serializer >> uFileSize;
            m_pProject->RegisterFileLayoutInfo(i, uStartPos, uFileSize);

            size_t uComponentCount = 0;
            serializer >> uComponentCount;
            for (size_t j = 0; j < uComponentCount; ++j)
            {
                size_t uComponentDataSize, uGuid, uId;
                size_t uComponentStartPos = m_pSerializer->GetReadPos();
                serializer >> uComponentDataSize >> uGuid >> uId;
                m_pProject->RegisterComponent(i, uGuid, uId);
                serializer.SetReadPos(uComponentStartPos + uComponentDataSize);
            }

            BEATS_ASSERT(uStartPos + uFileSize <= serializer.GetWritePos(), _T("Data overflow!"));
            serializer.SetReadPos(uStartPos + uFileSize);
        }
        BEATS_ASSERT(serializer.GetReadPos() == serializer.GetWritePos(), _T("Some data are not loaded completly. loaded data size %d, all data size %d"), serializer.GetReadPos(), serializer.GetWritePos());
        
        // 2. Load start up file.
        SwitchFile(uStartFile);
    }
    return m_pSerializer;
}

void CComponentInstanceManager::SwitchFile(size_t uFileId)
{
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    std::vector<size_t> loadFiles, unloadFiles;
    bool bLoadThisFile= CalcSwitchFile(uFileId, loadFiles, unloadFiles);
    if (bLoadThisFile)
    {
        m_uCurLoadFileId = uFileId;
    }
    std::vector<CComponentBase*> loadedComponents;
    for (size_t i = 0; i < loadFiles.size(); ++i)
    {
        LoadFile(loadFiles[i], loadedComponents);
    }
    for (size_t i = 0; i < unloadFiles.size(); ++i)
    {
        CloseFile(unloadFiles[i]);
    }
    ResolveDependency();

    for (size_t i = 0; i < loadedComponents.size(); ++i)
    {
        loadedComponents[i]->Initialize();
    }
    m_uCurWorkingFileId = uFileId;
}

bool CComponentInstanceManager::CalcSwitchFile(size_t uFileId, std::vector<size_t>& loadFiles, std::vector<size_t>& unloadFiles)
{
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    std::vector<CComponentBase*> loadedComponents;
    bool bLoadThisFile = true;
    // 1. File is in the parent directory (loaded before): just change the content of m_proxyInCurScene
    if (m_loadedFiles.find(uFileId) != m_loadedFiles.end())
    {
        // Change content will be done at last. So do nothing here.
        bLoadThisFile = false;
    }
    else
    {
        CComponentProjectDirectory* pDirectory = m_pProject->FindProjectDirectoryById(uFileId);
        BEATS_ASSERT(pDirectory != NULL);
        TString strLogicPath;
        // new open a file.
        if (m_uCurLoadFileId == 0xFFFFFFFF)
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
                size_t uFileCount = directories.back()->GetFileList().size();
                for (size_t i = 0; i < uFileCount; ++i)
                {
                    size_t uFileId = directories.back()->GetFileList().at(i);
                    loadFiles.push_back(uFileId);
                }
                directories.pop_back();
            }
        }
        else
        {
            CComponentProjectDirectory* pCurDirectory = m_pProject->FindProjectDirectoryById(m_uCurLoadFileId);
            BEATS_ASSERT(pCurDirectory != NULL);
            strLogicPath = pDirectory->MakeRelativeLogicPath(pCurDirectory);

            // 2. File is at the same directory: close current file and open it.
            if (strLogicPath.empty())
            {
                unloadFiles.push_back(m_uCurLoadFileId);
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
                    for (size_t i = 0;i < fileList.size(); ++i)
                    {
                        if (fileList[i] != m_uCurLoadFileId)
                        {
                            loadFiles.push_back(fileList[i]);
                        }
                    }
                    // Load sub-directory to target, but don't load the last sub-directory, because we only need one file in it, the target file.
                    CComponentProjectDirectory* pCurLoopDirectory = pDirectory;
                    for (int i = 1; i < (int)logicPaths.size() - 1; ++i)
                    {
                        pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                        size_t uFileCount = pCurLoopDirectory->GetFileList().size();
                        for (size_t i = 0; i < uFileCount; ++i)
                        {
                            size_t uFileId = pCurLoopDirectory->GetFileList().at(i);
                            loadFiles.push_back(uFileId);
                        }
                    }
                }
                else// 4. File is in other different directory: close current and change directory.
                {
                    // This means the file should be already loaded (since it's in the parent directory)
                    // But it is not in the m_loadedFiles, so this file must be a new added one
                    // So we do some nothing as if it is already loaded.
                    BEATS_ASSERT(logicPaths.back().compare(_T("..")) != 0);
                    unloadFiles.push_back(m_uCurLoadFileId);
                    CComponentProjectDirectory* pCurLoopDirectory = pCurDirectory->GetParent();
                    for (int i = 1; i < (int)logicPaths.size() - 1; ++i)
                    {
                        if (logicPaths[i].compare(_T("..")) == 0)
                        {
                            const std::vector<size_t>& fileList = pCurLoopDirectory->GetFileList();
                            for (size_t i = 0; i < fileList.size(); ++i)
                            {
                                unloadFiles.push_back(fileList[i]);
                            }
                            pCurLoopDirectory = pCurLoopDirectory->GetParent();
                        }
                        else
                        {
                            pCurLoopDirectory = pCurLoopDirectory->FindChild(logicPaths[i].c_str());
                            BEATS_ASSERT(pCurLoopDirectory != NULL);
                            size_t uFileCount = pCurLoopDirectory->GetFileList().size();
                            for (size_t i = 0; i < uFileCount; ++i)
                            {
                                size_t uFileId = pCurLoopDirectory->GetFileList().at(i);
                                loadFiles.push_back(uFileId);
                            }
                        }
                    }
                }
            }
        }
    }

    if (bLoadThisFile)
    {
        loadFiles.push_back(uFileId);
    }
    return bLoadThisFile;
}

void CComponentInstanceManager::LoadFile(size_t uFileId, std::vector<CComponentBase*>& loadComponents)
{
    size_t uFileStartPos = 0;
    size_t uFileDataLength = 0;
    bool bRet = m_pProject->QueryFileLayoutInfo(uFileId, uFileStartPos, uFileDataLength);
    BEATS_ASSERT(bRet, _T("Query file layout info failed! file id %d"), uFileId);
    if (bRet)
    {
        m_pSerializer->SetReadPos(uFileStartPos);
        size_t uFileStartPosRead, uFileDataLengthRead;
        *m_pSerializer >> uFileStartPosRead >> uFileDataLengthRead;
        BEATS_ASSERT(uFileStartPosRead == uFileStartPos && uFileDataLengthRead == uFileDataLength);
        size_t uComponentCount = 0;
        *m_pSerializer >> uComponentCount;
        for (size_t j = 0; j < uComponentCount; ++j)
        {
            size_t uComponentDataSize, uGuid, uId;
            size_t uComponentStartPos = m_pSerializer->GetReadPos();
            *m_pSerializer >> uComponentDataSize >> uGuid >> uId;
            CComponentInstance* pComponent = (CComponentInstance*)(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, m_pSerializer, false));
            pComponent->SetDataPos(uComponentStartPos);
            pComponent->SetDataSize(uComponentDataSize);
            BEATS_ASSERT(pComponent != NULL);
            loadComponents.push_back(pComponent);
            BEATS_ASSERT(uComponentStartPos + uComponentDataSize == m_pSerializer->GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uComponentDataSize, m_pSerializer->GetReadPos() - uComponentStartPos);
            m_pSerializer->SetReadPos(uComponentStartPos + uComponentDataSize);
        }
        m_loadedFiles.insert(uFileId);
        BEATS_ASSERT(m_pSerializer->GetReadPos() - uFileStartPos == uFileDataLength, _T("File Data NOT Match!\nGot an error when import data for file %d Required size:%d Actual size %d"), uFileId, uFileDataLength, m_pSerializer->GetReadPos() - uFileStartPos);
    }
}

void CComponentInstanceManager::CloseFile(size_t uFileId)
{
    BEATS_ASSERT(m_loadedFiles.find(uFileId) != m_loadedFiles.end(), _T("Can't close file which is not opened!"));
    std::vector<CComponentBase*> componentToDelete;
    std::map<size_t, std::vector<size_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
    auto iter = pFileToComponentMap->find(uFileId);
    if (iter != pFileToComponentMap->end())
    {
#ifdef _DEBUG
        // All components must exists before they call uninitialize.
        for (size_t i = 0;i < iter->second.size(); ++i)
        {
            size_t uComponentId = iter->second.at(i);
            CComponentBase* pComponent = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
            BEATS_ASSERT(pComponent != NULL && pComponent->IsInitialized());
        }
#endif
        for (size_t i = 0;i < iter->second.size(); ++i)
        {
            size_t uComponentId = iter->second.at(i);
            CComponentBase* pComponentBase = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
            // This may be null, because some components can be uninitialized by other component's uninitialize.
            if (pComponentBase != NULL && pComponentBase->IsInitialized())
            {
                pComponentBase->Uninitialize();
                componentToDelete.push_back(pComponentBase);
            }
        }
    }
    for (size_t i = 0; i < componentToDelete.size(); ++i)
    {
        BEATS_SAFE_DELETE(componentToDelete[i]);
    }
    m_loadedFiles.erase(uFileId);
}

CSerializer* CComponentInstanceManager::GetFileSerializer() const
{
    return m_pSerializer;
}

const std::set<size_t>& CComponentInstanceManager::GetLoadedFiles() const
{
    return m_loadedFiles;
}

void CComponentInstanceManager::SetCurLoadFileId(size_t uFileId)
{
    m_uCurLoadFileId = uFileId;
}

size_t CComponentInstanceManager::GetCurLoadFileId() const
{
    return m_uCurLoadFileId;
}

bool CComponentInstanceManager::IsInClonePhase() const
{
    return m_bInClonePhase;
}

void CComponentInstanceManager::SetClonePhaseFlag(bool bInClonePhase)
{
    m_bInClonePhase = bInClonePhase;
}

void CComponentInstanceManager::ResolveDependency()
{
    for (size_t i = 0; i < m_pDependencyResolver->size(); ++i)
    {
        SDependencyResolver* pDependencyResolver = (*m_pDependencyResolver)[i];
        CComponentBase* pComponentToBeLink = GetComponentInstance(pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pComponentToBeLink != NULL, _T("Resolve dependency failed, Comopnent id %d guid 0x%x doesn't exist!"), pDependencyResolver->uInstanceId, pDependencyResolver->uGuid);
        BEATS_ASSERT(pDependencyResolver->pDescription == NULL);
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
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

size_t CComponentInstanceManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}

void CComponentInstanceManager::LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents)
{
    size_t uFileCount = pDirectory->GetFileList().size();
    for (size_t i = 0; i < uFileCount; ++i)
    {
        size_t uFileId = pDirectory->GetFileList().at(i);
        LoadFile(uFileId, loadComponents);
    }
}