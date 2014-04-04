#include "stdafx.h"
#include "TextureFrag.h"
#include "Texture.h"


CTextureFrag::CTextureFrag(const TString &name, SharePtr<CTexture> texture, kmVec2 origin, kmVec2 size )
    : m_name(name)
    , m_texture(texture)
    , m_origin(origin)
    , m_size(size)
{
    m_quad.tl.u = m_origin.x / m_texture->Width();
    m_quad.tl.v = m_origin.y / m_texture->Height();
    m_quad.tr.u = (m_origin.x + m_size.x) / m_texture->Width();
    m_quad.tr.v = m_quad.tl.v;
    m_quad.bl.u = m_quad.tl.u;
    m_quad.bl.v = (m_origin.y + m_size.y) / m_texture->Height();
    m_quad.br.u = m_quad.tr.u;
    m_quad.br.v = m_quad.bl.v;
}

TString CTextureFrag::Name() const
{
    return m_name;
}

const CQuadT & CTextureFrag::Quad() const
{
    return m_quad;
}

SharePtr<CTexture> CTextureFrag::Texture() const
{
    return m_texture;
}

kmVec2 CTextureFrag::Origin() const
{
    return m_origin;
}

kmVec2 CTextureFrag::Size() const
{
    return m_size;
}
