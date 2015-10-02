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
#include <algorithm>

CComponentInstanceManager* CComponentInstanceManager::m_pInstance = NULL;
CComponentInstanceManager::CComponentInstanceManager()
    : m_bInClonePhase(false)
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
    uint32_t uVersion = 0;
    serializer >> uVersion;
    if (uVersion != COMPONENT_SYSTEM_VERSION)
    {
        TCHAR szInfo[256];
        _stprintf(szInfo, _T("Data version is %d while our program version is %d"), uVersion, COMPONENT_SYSTEM_VERSION);
        MessageBox(NULL, szInfo, _T("Verion Not Match!"), MB_OK | MB_ICONERROR);
    }
    else
    {
        uint32_t uComponentTemplateCount = 0;
        serializer >> uComponentTemplateCount;
        CComponentProjectDirectory* pRootDirectory = new CComponentProjectDirectory(NULL, _T("Root"));
        m_pProject->SetRootDirectory(pRootDirectory);
        pRootDirectory->Deserialize(serializer);
        uint32_t uStartFile;
        serializer >> uStartFile;
        m_pProject->SetStartFile(uStartFile);
        // 1. Load binarize data and file structure.
        uint32_t uFileCount = 0;
        serializer >> uFileCount;
        std::map<uint32_t, std::vector<uint32_t> > conflictMap;
        for (uint32_t i = 0; i < uFileCount; ++i)
        {
            TString strFileName;
            serializer >> strFileName;
            // This is a hack way here!
            const_cast<std::vector<TString>*>(m_pProject->GetFileList())->push_back(strFileName);
            BEATS_ASSERT(conflictMap.size() == 0);

            uint32_t uStartPos = 0;
            serializer >> uStartPos;
            BEATS_ASSERT(uStartPos == (serializer.GetReadPos() - sizeof(uStartPos)), _T("File start pos not match!"));
            uint32_t uFileSize = 0;
            serializer >> uFileSize;
            m_pProject->RegisterFileLayoutInfo(i, uStartPos, uFileSize);
            uint32_t uComponentCount = 0;
            serializer >> uComponentCount;

            for (uint32_t j = 0; j < uComponentCount; ++j)
            {
                uint32_t uComponentDataSize, uGuid, uId;
                uint32_t uComponentStartPos = m_pSerializer->GetReadPos();
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

void CComponentInstanceManager::SwitchFile(uint32_t uFileId)
{
    BEATS_ASSERT(uFileId != 0xFFFFFFFF);
    std::vector<uint32_t> loadFiles, unloadFiles;
    bool bNewAddFile = false; // Useless here. Only useful when CComponentProxyManager call CalcSwitchFile.
    CalcSwitchFile(uFileId, false, loadFiles, unloadFiles, bNewAddFile);
    BEATS_ASSERT(!bNewAddFile, _T("This can't happen in CComponentInstanceManager, it can be true only in CComponentProxyManager"));
    bool bLoadThisFile = loadFiles.size() > 0 && loadFiles.back() == uFileId;
    if (bLoadThisFile)
    {
        m_uCurLoadFileId = uFileId;
    }
    for (uint32_t i = 0; i < unloadFiles.size(); ++i)
    {
        CloseFile(unloadFiles[i]);
    }
    std::vector<CComponentBase*> loadedComponents;
    for (uint32_t i = 0; i < loadFiles.size(); ++i)
    {
        LoadFile(loadFiles[i], &loadedComponents);
    }
    ResolveDependency();

    for (uint32_t i = 0; i < loadedComponents.size(); ++i)
    {
        loadedComponents[i]->Initialize();
    }
}

void CComponentInstanceManager::LoadFile(uint32_t uFileId, std::vector<CComponentBase*>* pLoadComponents)
{
    uint32_t uFileStartPos = 0;
    uint32_t uFileDataLength = 0;
    bool bRet = m_pProject->QueryFileLayoutInfo(uFileId, uFileStartPos, uFileDataLength);
    BEATS_ASSERT(bRet, _T("Query file layout info failed! file id %d"), uFileId);
    if (bRet)
    {
        m_pSerializer->SetReadPos(uFileStartPos);
        uint32_t uFileStartPosRead, uFileDataLengthRead;
        *m_pSerializer >> uFileStartPosRead >> uFileDataLengthRead;
        BEATS_ASSERT(uFileStartPosRead == uFileStartPos && uFileDataLengthRead == uFileDataLength);
        uint32_t uComponentCount = 0;
        *m_pSerializer >> uComponentCount;
        for (uint32_t j = 0; j < uComponentCount; ++j)
        {
            uint32_t uComponentDataSize, uGuid, uId;
            uint32_t uComponentStartPos = m_pSerializer->GetReadPos();
            *m_pSerializer >> uComponentDataSize >> uGuid >> uId;
            CComponentInstance* pComponent = (CComponentInstance*)(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, m_pSerializer, false));
            pComponent->SetDataPos(uComponentStartPos);
            pComponent->SetDataSize(uComponentDataSize);
            BEATS_ASSERT(pComponent != NULL);
            if (pLoadComponents != nullptr)
            {
                pLoadComponents->push_back(pComponent);
            }
            BEATS_ASSERT(uComponentStartPos + uComponentDataSize == m_pSerializer->GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uComponentDataSize, m_pSerializer->GetReadPos() - uComponentStartPos);
            m_pSerializer->SetReadPos(uComponentStartPos + uComponentDataSize);
        }
        ResolveDependency();
        m_loadedFiles.push_back(uFileId);
        BEATS_ASSERT(m_pSerializer->GetReadPos() - uFileStartPos == uFileDataLength, _T("File Data NOT Match!\nGot an error when import data for file %d Required size:%d Actual size %d"), uFileId, uFileDataLength, m_pSerializer->GetReadPos() - uFileStartPos);
    }
}

void CComponentInstanceManager::CloseFile(uint32_t uFileId)
{
    std::vector<uint32_t>::iterator iterFile = std::find(m_loadedFiles.begin(), m_loadedFiles.end(), uFileId);
    BEATS_WARNING(iterFile != m_loadedFiles.end(), "Close an unopened file %d, this may be right if we are exiting the program.", uFileId);
    if (iterFile != m_loadedFiles.end())
    {
        std::vector<CComponentBase*> componentToDelete;
        std::map<uint32_t, std::vector<uint32_t> >* pFileToComponentMap = m_pProject->GetFileToComponentMap();
        auto iter = pFileToComponentMap->find(uFileId);
        if (iter != pFileToComponentMap->end())
        {
            for (uint32_t i = 0; i < iter->second.size(); ++i)
            {
                uint32_t uComponentId = iter->second.at(i);
                BEATS_ASSERT(uComponentId != 0xFFFFFFFF);
                CComponentBase* pComponentBase = CComponentInstanceManager::GetInstance()->GetComponentInstance(uComponentId);
                // This may be null, because some components can be uninitialized by other component's uninitialize.
                if (pComponentBase != NULL)
                {
                    if (pComponentBase->IsInitialized())
                    {
                        pComponentBase->Uninitialize();
                    }
                    else
                    {
                        // Component may not be initialized this moment, for example:
                        // Close file right after load file, just to fetch some component data, so we don't need to initialize it.
                        // Even it is not initialized, we still need to unregister the component since register is done in creating component procession.
                        UnregisterInstance(pComponentBase);
                        GetIdManager()->RecycleId(uComponentId);
                    }
                    componentToDelete.push_back(pComponentBase);
                }
            }
        }
        for (uint32_t i = 0; i < componentToDelete.size(); ++i)
        {
            BEATS_SAFE_DELETE(componentToDelete[i]);
        }
        m_loadedFiles.erase(iterFile);
    }
}

CSerializer* CComponentInstanceManager::GetFileSerializer() const
{
    return m_pSerializer;
}

void CComponentInstanceManager::SetCurLoadFileId(uint32_t uFileId)
{
    m_uCurLoadFileId = uFileId;
}

uint32_t CComponentInstanceManager::GetCurLoadFileId() const
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
    for (uint32_t i = 0; i < m_pDependencyResolver->size(); ++i)
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
                *(ptrdiff_t*)pDependencyResolver->pVariableAddress = (ptrdiff_t)pComponentToBeLink;
            }
        }
    }
    BEATS_SAFE_DELETE_VECTOR(*m_pDependencyResolver);
}

uint32_t CComponentInstanceManager::GetVersion()
{
    return COMPONENT_SYSTEM_VERSION;
}

void CComponentInstanceManager::LoadDirectoryFiles(CComponentProjectDirectory* pDirectory, std::vector<CComponentBase*>& loadComponents)
{
    size_t uFileCount = pDirectory->GetFileList().size();
    for (size_t i = 0; i < uFileCount; ++i)
    {
        uint32_t uFileId = pDirectory->GetFileList().at(i);
        LoadFile(uFileId, &loadComponents);
    }
}