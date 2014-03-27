#include "stdafx.h"
#include "SpriteFrame.h"
#include "Texture.h"
#include "TextureFrag.h"

CSpriteFrame::CSpriteFrame( SharePtr<CTexture> texture, const CRect &rect)
    : m_textureFrag(new CTextureFrag(texture, rect.point, rect.size))
{
}

CSpriteFrame::CSpriteFrame( SharePtr<CTexture> texture, 
    const CRect &rect, const CSize &size, const CPoint &origin)
    : m_textureFrag(new CTextureFrag(texture, rect.point, rect.size))
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

void CSpriteFrame::SetOriginSize( const CPoint &origin, const CSize &size )
{
    //set vertex
    m_quad.tl.x = 0 - origin.x;
    m_quad.tl.y = 0 - origin.y;
    m_quad.tr.x = size.width - origin.x;
    m_quad.tr.y = m_quad.tl.y;
    m_quad.bl.x = m_quad.tl.x;
    m_quad.bl.y = size.height - origin.y;
    m_quad.br.x = m_quad.tr.x;
    m_quad.br.y = m_quad.bl.y;
}
