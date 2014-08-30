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
        CComponentProjectDirectory* pRootProject = new CComponentProjectDirectory(NULL, _T("Root"));
        m_pProject->SetRootDirectory(pRootProject);
        pRootProject->Deserialize(serializer);
        size_t uStartFile;
        serializer >> uStartFile;
        m_pProject->SetStartFile(uStartFile);
        // 1. Load binarize data and file sturcture.
        size_t uFileCount = 0;
        serializer >> uFileCount;
        for (size_t j = 0; j < uFileCount; ++j)
        {
            size_t uStartPos = 0;
            serializer >> uStartPos;
            BEATS_ASSERT(uStartPos == (serializer.GetReadPos() - sizeof(uStartPos)), _T("File start pos not match!"));
            size_t uFileSize = 0;
            serializer >> uFileSize;
            m_pProject->RegisterFileLayoutInfo(j, uStartPos, uFileSize);
            BEATS_ASSERT(uStartPos + uFileSize <= serializer.GetWritePos(), _T("Data overflow!"));
            serializer.SetReadPos(uStartPos + uFileSize);
        }
        BEATS_ASSERT(serializer.GetReadPos() == serializer.GetWritePos(), _T("Some data are not loaded completly. loaded data size %d, all data size %d"), serializer.GetReadPos(), serializer.GetWritePos());
        
//#error:TODO
        // 2. Load start up file.
        //LoadDirectory(pStartDirectory);
        
        // 2. Resolve dependency.
        CComponentInstanceManager::GetInstance()->ResolveDependency();
        
        // 3. Call Initialize.
        InitializeAllInstance();
    }
    return m_pSerializer;
}

void CComponentInstanceManager::LoadDirectory(CComponentProjectDirectory* pDirectory)
{
    std::vector<CComponentProjectDirectory*> loadingDirectories;
    CComponentProjectDirectory* pCurDirectory = pDirectory;
    while (pCurDirectory != NULL)
    {
        loadingDirectories.push_back(pCurDirectory);
        pCurDirectory = pCurDirectory->GetParent();
    }
    while (loadingDirectories.size() > 0)
    {
        pCurDirectory = loadingDirectories.back();
        LoadDirectoryFiles(pCurDirectory);
        loadingDirectories.pop_back();
    }
}

CSerializer* CComponentInstanceManager::GetFileSerializer() const
{
    return m_pSerializer;
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

void CComponentInstanceManager::LoadDirectoryFiles(CComponentProjectDirectory* pDirectory)
{
    size_t uFileCount = pDirectory->GetFileList().size();
    for (size_t i = 0; i < uFileCount; ++i)
    {
        size_t uFileId = pDirectory->GetFileList().at(i);
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
                CComponentBase* pComponent = CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, false, uId, true, m_pSerializer, false);
                pComponent;
                BEATS_ASSERT(uComponentStartPos + uComponentDataSize == m_pSerializer->GetReadPos(), _T("Component Data Not Match!\nGot an error when import data for component %x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, pComponent->GetClassStr(), uId, uComponentDataSize, m_pSerializer->GetReadPos() - uComponentStartPos);
                m_pSerializer->SetReadPos(uComponentStartPos + uComponentDataSize);
            }
            BEATS_ASSERT(m_pSerializer->GetReadPos() - uFileStartPos == uFileDataLength, _T("File Data NOt Match!\nGot an error when import data for file %d Required size:%d Actual size %d"), uFileId, uFileDataLength, m_pSerializer->GetReadPos() - uFileStartPos);
        }
    }
}