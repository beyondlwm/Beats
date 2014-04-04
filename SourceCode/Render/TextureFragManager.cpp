#include "stdafx.h"
#include "TextureFragManager.h"
#include "TextureFrag.h"
#include "Texture.h"

CTextureFragManager *CTextureFragManager::m_pInstance = nullptr;

CTextureFragManager::CTextureFragManager()
{

}

CTextureFragManager::~CTextureFragManager()
{
    for(auto textureFrag : m_textureFrags)
    {
        BEATS_SAFE_DELETE(textureFrag.second);
    }
}

CTextureFrag *CTextureFragManager::CreateTextureFrag(const TString &name, 
    SharePtr<CTexture> texture, kmVec2 origin, kmVec2 size)
{
    BEATS_ASSERT(m_textureFrags.find(name) == m_textureFrags.end(),
        _T("TextureFrag:%s already existed."), name.c_str());
    CTextureFrag *textureFrag = new CTextureFrag(name, texture, origin, size);
    m_textureFrags[name] = textureFrag;
    return textureFrag;
}

CTextureFrag *CTextureFragManager::GetTextureFrag(const TString &name)
{
	CTextureFrag * pRet = NULL;
    auto itr = m_textureFrags.find(name);
    if(itr != m_textureFrags.end())
    {
        pRet = itr->second;
    }
    return pRet;
}
