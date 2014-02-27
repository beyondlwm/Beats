#include "stdafx.h"
#include "ComponentProjectData.h"
#include "ComponentProject.h"
#include "ComponentManager.h"

CComponentProjectDirectory::CComponentProjectDirectory(CComponentProjectDirectory* pParent, const TString& strName)
: m_pParent(pParent)
, m_strName(strName)
{
    if (pParent != NULL)
    {
        pParent->m_pChildrenVec->push_back(this);
    }
    m_pFilesSet = new std::set<size_t>();
    m_pChildrenVec = new std::vector<CComponentProjectDirectory*>();
}

CComponentProjectDirectory::~CComponentProjectDirectory()
{
    DeleteAll(false);
    BEATS_SAFE_DELETE(m_pFilesSet);
    BEATS_SAFE_DELETE(m_pChildrenVec);
}

bool CComponentProjectDirectory::AddFile(const TString& strFileName, std::map<size_t, std::vector<size_t>>& conflictMap)
{
    CComponentProject* pProject = CComponentManager::GetInstance()->GetProject();
    size_t uFileId = pProject->RegisterFile(strFileName, conflictMap);
    BEATS_ASSERT(m_pFilesSet->find(uFileId) == m_pFilesSet->end());
    m_pFilesSet->insert(uFileId);
    return true;
}

CComponentProjectDirectory* CComponentProjectDirectory::AddDirectory(const TString& strDirectoryName)
{
    CComponentProjectDirectory* pRet = NULL;
    bool bAlreadyExists = false;
    std::vector<CComponentProjectDirectory*>::iterator iter = m_pChildrenVec->begin();
    for (; iter != m_pChildrenVec->end(); ++iter)
    {
        if ((*iter)->m_strName.compare(strDirectoryName) == 0)
        {
            bAlreadyExists = true;
            break;
        }
    }
    if (!bAlreadyExists)
    {
        pRet = new CComponentProjectDirectory(this, strDirectoryName);
    }
    return pRet;
}

bool CComponentProjectDirectory::DeleteFile(size_t uFileId)
{
    bool bRet = false;
    if (m_pFilesSet->find(uFileId) != m_pFilesSet->end())
    {
        m_pFilesSet->erase(uFileId);

        CComponentManager* pComopnentManager = CComponentManager::GetInstance();
        CComponentProject* pProject = pComopnentManager->GetProject();
        TString strFileName = pProject->GetComponentFileName(uFileId);
        bool bDeleteCurWorkingFile = strFileName.compare(pComopnentManager->GetCurrentWorkingFilePath()) == 0;
        if (bDeleteCurWorkingFile)
        {
            pComopnentManager->CloseFile();
        }
        pProject->UnregisterFile(uFileId);
        bRet = true;
    }
    else
    {
        for (std::vector<CComponentProjectDirectory*>::iterator iter = m_pChildrenVec->begin(); iter != m_pChildrenVec->end(); ++iter)
        {
            bRet = (*iter)->DeleteFile(uFileId);
            if (bRet)
            {
                break;
            }
        }
    }

    return bRet;
}

bool CComponentProjectDirectory::DeleteDirectory(CComponentProjectDirectory* pDirectory)
{
    bool bRet = false;
    for (std::vector<CComponentProjectDirectory*>::iterator iter = m_pChildrenVec->begin(); iter != m_pChildrenVec->end(); ++iter)
    {
        if ((*iter) == pDirectory)
        {
            m_pChildrenVec->erase(iter);
            BEATS_SAFE_DELETE(pDirectory);
            bRet = true;
            break;
        }
    }
    return bRet;
}


const TString& CComponentProjectDirectory::GetName() const
{
    return m_strName;
}

const std::vector<CComponentProjectDirectory*>& CComponentProjectDirectory::GetChildren() const
{
    return *m_pChildrenVec;
}

const std::set<size_t>& CComponentProjectDirectory::GetFileList() const
{
    return *m_pFilesSet;
}

CComponentProjectDirectory* CComponentProjectDirectory::GetParent() const
{
    return m_pParent;
}

bool CComponentProjectDirectory::DeleteAll(bool bUpdateProject)
{
    for (std::vector<CComponentProjectDirectory*>::iterator iter = m_pChildrenVec->begin(); iter != m_pChildrenVec->end(); ++iter)
    {
        BEATS_SAFE_DELETE(*iter);
    }
    m_pChildrenVec->clear();
    if (bUpdateProject)
    {
        while (m_pFilesSet->size() != 0)
        {
            DeleteFile(*m_pFilesSet->begin());
        }
    }
    else
    {
        m_pFilesSet->clear();
    }
    return true;
}

bool CComponentProjectDirectory::IsDirectory()
{
    return m_pFilesSet != NULL && m_pChildrenVec != NULL;
}
