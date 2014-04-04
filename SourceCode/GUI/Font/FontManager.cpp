#include "stdafx.h"
#include "FontManager.h"
#include "FontFace.h"
#include "Render/Texture.h"
#include "Render/SpriteFrameBatchManager.h"

using namespace FCGUI;

FontManager *FontManager::m_pInstance = nullptr;

FontManager::FontManager()
{

}

FontManager::~FontManager()
{

}

FontFace *FontManager::CreateFace( const TString &name, const std::string &file, int sizeInPx )
{
    auto itr = _faces.find(name);
    BEATS_ASSERT(itr == _faces.end(), _T("Font face:%s already existed"), name.c_str());

    FontFace *face = new FontFace(name, file, sizeInPx);
    _faces[name] = face;
    return face;
}

FontFace *FontManager::GetFace( const TString &name )
{
    auto itr = _faces.find(name);
    return itr != _faces.end() ? itr->second : nullptr;
}

void FontManager::DrawText( const TString &text, kmScalar x, kmScalar y, const TString &faceName )
{
    FontFace *face = GetFace(faceName);
    BEATS_ASSERT(face);

    auto glyphs = face->GetGlyphs(text);
    for(auto glyph : glyphs)
    {
        CQuadPT quad;
        quad.tl.position.x = x;
        quad.tl.position.y = y;
        quad.tr.position.x = x + glyph->width;
        quad.tr.position.y = quad.tl.position.y;
        quad.bl.position.x = quad.tl.position.x;
        quad.bl.position.y = y + glyph->height;
        quad.br.position.x = quad.tr.position.x;
        quad.br.position.y = quad.bl.position.y;
        quad.tl.tex.u = glyph->u;
        quad.tl.tex.v = glyph->v;
        quad.tr.tex.u = glyph->u + glyph->width / glyph->texture->Width();
        quad.tr.tex.v = quad.tl.tex.v;
        quad.bl.tex.u = quad.tl.tex.u;
        quad.bl.tex.v = glyph->v + glyph->height / glyph->texture->Height();
        quad.br.tex.u = quad.tr.tex.u;
        quad.br.tex.v = quad.bl.tex.v;

        CSpriteFrameBatchManager::GetInstance()->AddQuad(quad, glyph->texture);

        x += glyph->width;
    }
}
