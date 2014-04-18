#include "stdafx.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderManager.h"
#include "SpriteFrameBatchManager.h"
#include "TextureFragManager.h"
#include "TextureFrag.h"

CSprite::CSprite(CTextureFrag *textureFrag)
    : m_textureFrag(textureFrag)
{
    kmVec2 origin;
    kmVec2Fill(&origin, 0.f, 0.f);
    SetOriginSize(origin, m_textureFrag->Size());
}

CSprite::CSprite(const TString &textureFragName)
    : m_textureFrag(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName))
{
    kmVec2 origin;
    kmVec2Fill(&origin, 0.f, 0.f);
    SetOriginSize(origin, m_textureFrag->Size());
}

CSprite::CSprite(const TString &textureFragName, const kmVec2 &size)
    : m_textureFrag(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName))
{
    kmVec2 origin;
    kmVec2Fill(&origin, 0.f, 0.f);
    SetOriginSize(origin, size);
}

CSprite::CSprite(const TString &textureFragName, const kmVec2 &size, const kmVec2 &origin)
    : m_textureFrag(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName))
{
    SetOriginSize(origin, size);
}

CSprite::~CSprite()
{
}

void CSprite::PreRender()
{
}

void CSprite::Render()
{
    kmMat4 transform;
    kmMat4Identity(&transform);

    BEATS_ASSERT(m_textureFrag);
    CSpriteFrameBatchManager *batchMgr = CSpriteFrameBatchManager::GetInstance();
    if(m_textureFrag)
    {
        batchMgr->AddQuad(m_quad, m_textureFrag, transform);
    }
}

void CSprite::PostRender()
{
}

const CQuadP &CSprite::QuadP() const
{
    return m_quad;
}

const CQuadT &CSprite::QuadT() const
{
    return m_textureFrag->Quad();
}

SharePtr<CTexture> CSprite::Texture() const
{
    return m_textureFrag->Texture();
}

void CSprite::SetQuad( const CQuadP &quad )
{
    m_quad = quad;
}

void CSprite::SetOriginSize( const kmVec2 &origin, const kmVec2 &size )
{
    //set vertex
    m_quad.tl.x = 0 - origin.x;
    m_quad.tl.y = 0 - origin.y;
    m_quad.tr.x = size.x - origin.x;
    m_quad.tr.y = m_quad.tl.y;
    m_quad.bl.x = m_quad.tl.x;
    m_quad.bl.y = size.y - origin.y;
    m_quad.br.x = m_quad.tr.x;
    m_quad.br.y = m_quad.bl.y;
}