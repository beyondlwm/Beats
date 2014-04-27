#include "stdafx.h"
#include "SpriteFrame.h"
#include "Texture.h"
#include "TextureFrag.h"
#include "TextureFragManager.h"

CSpriteFrame::CSpriteFrame(CTextureFrag *textureFrag)
    : m_textureFrag(textureFrag)
{
}

CSpriteFrame::CSpriteFrame(const TString &textureFragName)
    : m_textureFrag(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName))
{

}

CSpriteFrame::CSpriteFrame(const TString &textureFragName, const kmVec2 &size, const kmVec2 &origin)
    : m_textureFrag(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName))
{
    SetOriginSize(origin, size);
}

CSpriteFrame::~CSpriteFrame()
{
    BEATS_SAFE_DELETE(m_textureFrag);
}

const CQuadP &CSpriteFrame::QuadP() const
{
    return m_quad;
}

const CQuadT &CSpriteFrame::QuadT() const
{
    return m_textureFrag->Quad();
}

SharePtr<CTexture> CSpriteFrame::Texture() const
{
    return m_textureFrag->Texture();
}

void CSpriteFrame::SetQuad( const CQuadP &quad )
{
    m_quad = quad;
}

void CSpriteFrame::SetOriginSize( const kmVec2 &origin, const kmVec2 &size )
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
