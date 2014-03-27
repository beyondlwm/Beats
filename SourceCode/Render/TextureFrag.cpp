#include "stdafx.h"
#include "TextureFrag.h"
#include "Texture.h"


CTextureFrag::CTextureFrag( SharePtr<CTexture> texture, CPoint origin, CSize size )
    : m_texture(texture)
    , m_origin(origin)
    , m_size(size)
{
    m_quad.tl.u = m_origin.x / m_texture->Width();
    m_quad.tl.v = m_origin.y / m_texture->Height();
    m_quad.tr.u = (m_origin.x + m_size.width) / m_texture->Width();
    m_quad.tr.v = m_quad.tl.v;
    m_quad.bl.u = m_quad.tl.u;
    m_quad.bl.v = (m_origin.y + m_size.height) / m_texture->Height();
    m_quad.br.u = m_quad.tr.u;
    m_quad.br.v = m_quad.bl.v;
}

const CQuadT & CTextureFrag::Quad() const
{
    return m_quad;
}

SharePtr<CTexture> CTextureFrag::Texture() const
{
    return m_texture;
}
