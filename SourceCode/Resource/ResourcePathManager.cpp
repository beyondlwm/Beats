#include "stdafx.h"
#include "ResourcePathManager.h"
#include "CCFileUtils.h"

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
    std::vector<std::string> searchPaths;
    for (auto i = 0; i < eRPT_Count; i++)
    {
        char szBuffer[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToCHAR(pszCocosResourcePathName[i], szBuffer, MAX_PATH);
        searchPaths.push_back(szBuffer);
    }
    cocos2d::FileUtils::getInstance()->setSearchPaths(searchPaths);
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
    case eRT_ParticleScript:
        ret = eRPT_Particle;
        break;
    case eRT_Font:
        ret = eRPT_Font;
        break;
    case eRT_Language:
        ret = eRPT_Language;
        break;

    default:
        break;
    }
    BEATS_ASSERT(ret != eRPT_Count, _T("Unknown type of resource %d"), type);
    return ret;
}

TString CResourcePathManager::GetFullPathForFilename( const std::string &filename )
{
    std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
    TCHAR szBuffer[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(fullPath.c_str(), szBuffer, MAX_PATH);
    return szBuffer;
}

const TString CResourcePathManager::GetResourcePath( EResourcePathType type ) const
{
    const std::vector<std::string> searchPathArray = cocos2d::FileUtils::getInstance()->getSearchPaths();
    TCHAR szBuffer[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(searchPathArray[type].c_str(), szBuffer, MAX_PATH);
    return szBuffer;
}
