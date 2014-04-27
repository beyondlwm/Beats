#include "stdafx.h"
#include "Resource.h"

CResource::CResource()
    : m_bIsLoaded(false)
{

}

CResource::~CResource()
{
}

const TString& CResource::GetFilePath() const
{
    return m_strPath;
}

void CResource::SetFilePath(const TString& str)
{
    m_strPath = str;
}

void CResource::SetLoadedFlag(bool bFlag)
{
    m_bIsLoaded = bFlag;
}

bool CResource::IsLoaded() const
{
    return m_bIsLoaded;
}
