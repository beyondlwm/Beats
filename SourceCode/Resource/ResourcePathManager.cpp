#include "stdafx.h"
#include "ResourcePathManager.h"

CResourcePathManager* CResourcePathManager::m_pInstance = NULL;

CResourcePathManager::CResourcePathManager()
{
    Init();
}

CResourcePathManager::~CResourcePathManager()
{

}

void CResourcePathManager::Init()
{
    for (auto i = 0; i < eRPT_Count; i++)
    {
        if (i == 0)
        {
            TCHAR szPath[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, szPath);
            m_szPath[eRPT_Work] = szPath;
        }
        else
        {
            m_szPath[i] = m_szPath[eRPT_Work] + _T("/../") + pszResourcePathName[i];
        }
    }
}

const TString& CResourcePathManager::GetResourcePath(EResourcePathType type) const
{
    return m_szPath[type];
}

EResourcePathType CResourcePathManager::GetResourcePathType(EResourceType type) const
{
    EResourcePathType ret = eRPT_Count;
    switch (type)
    {
    case eRT_Texture:
        ret = eRPT_Texture;
        break;
    case eRT_Skeleton:
        ret = eRPT_Skeleton;
        break;
    case eRT_Skin:
        ret = eRPT_Skin;
        break;
    case eRT_Material:
        ret = eRPT_Material;
        break;
    case eRT_Shader:
        ret = eRPT_Shader;
        break;
    case eRT_SpriteAnimation:
        ret = eRPT_SpriteAnimation;
        break;
    case eRT_Animation:
        ret = eRPT_Animation;
        break;

    default:
        break;
    }
    BEATS_ASSERT(ret != eRPT_Count, _T("Unknown type of resource %d"), type);
    return ret;
}
