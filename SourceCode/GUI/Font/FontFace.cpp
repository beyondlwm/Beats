#include "stdafx.h"
#include "FontFace.h"
#include "Font.h"
#include "Render/Texture.h"
#include "Resource/ResourceManager.h"
#include "Render/SpriteFrameBatchManager.h"

#define FT_SHIFT_NUM 6;

//next multiple of 8
inline int nextMOE(int x)
{
    return (x + 7) & 0xFFFFFFF8;
}

using namespace FCGUI;

FontFace::FontFace(const TString &name, const TString &file, int size, int dpi/*= -1*/)
    : _name(name)
    , _currPage(-1)
    , _currX(0)
    , _currY(0)
    , _lineHeight(0)
    , _ascender(0)
    , _size(size)
    , _dpi(dpi)
    , _font(CResourceManager::GetInstance()->GetResource<Font>(file, false))
{
    newPage();
}

FontFace::~FontFace()
{
    for(auto glyph : _glyphs)
    {
        BEATS_SAFE_DELETE(glyph.second);
    }
}

void FontFace::newPage()
{
    SharePtr<CTexture> texture = new CTexture;
    texture->InitWithData(nullptr, 0, PixelFormat::A8, PAGE_WIDTH, PAGE_HEIGHT);
    ++_currPage;
    _textures.push_back(texture);
}

void FontFace::setSize()
{
    FT_Error err = 0;
    if (_dpi == -1)
    {
        err = FT_Set_Pixel_Sizes(_font->Face(), _size, _size);
    }
    else
    {
        FT_F26Dot6 sizeTrans = _size << FT_SHIFT_NUM;
        err = FT_Set_Char_Size(_font->Face(), sizeTrans, sizeTrans, _dpi, _dpi);
    }

    _ascender = _font->Face()->size->metrics.ascender >> FT_SHIFT_NUM;
    long descender = _font->Face()->size->metrics.descender >> FT_SHIFT_NUM;
    _lineHeight = _ascender - descender;

    BEATS_ASSERT(!err);
}

void FontFace::PrepareCharacters( const TString &chars )
{
    setSize();

#ifndef _UNICODE
    //TODO: translate chars to unicode
#pragma error("non-unicode unsupported")
#endif

    FT_Error err = 0;

    for(size_t i = 0; i < chars.size(); ++i)
    {
        wchar_t character = chars[i];

        if(_glyphs.find(character) != _glyphs.end())
            continue;

        err = FT_Load_Char(_font->Face(), character, FT_LOAD_RENDER);
        BEATS_ASSERT(!err);

        FT_Bitmap &bitmap = _font->Face()->glyph->bitmap;
        FT_Glyph_Metrics &metrics = _font->Face()->glyph->metrics;
        long x = metrics.horiBearingX >> FT_SHIFT_NUM;
        long y = metrics.horiBearingY >> FT_SHIFT_NUM;
        long xAdvance = metrics.horiAdvance >> FT_SHIFT_NUM;
        long width = metrics.width >> FT_SHIFT_NUM;
        long height = metrics.height >> FT_SHIFT_NUM;
        width, height;

        if(x < 0)
            x =  0;
        if(xAdvance < x + width)
            xAdvance = x + width;

        long xOffset = x;
        long yOffset = _ascender - y;

        long destWidth = nextMOE(bitmap.width);
        long destHeight = nextMOE(bitmap.rows);

        if(_currX + xOffset + destWidth > PAGE_WIDTH)
        {
            _currX = 0;
            _currY += _lineHeight;
            if(_currY + yOffset + destHeight > PAGE_HEIGHT)
            {
                _currY = 0;
                newPage();
            }
        }

        unsigned char *data = bitmap.buffer;
        bool needRealloc = bitmap.width != bitmap.pitch
            || destWidth != bitmap.width || destHeight != bitmap.rows;
        if(needRealloc)
        {
            data = new unsigned char[destWidth * destHeight];
            for(long i = 0; i < destHeight; ++i)
            {
                for(long j = 0; j < destWidth; ++j)
                {
                    data[i * destWidth + j] = (i < bitmap.rows && j < bitmap.width) ?
                        bitmap.buffer[i * bitmap.pitch + j] : 0;
                }
            }
        }

        SharePtr<CTexture> texture = _textures[_currPage];
        texture->UpdateSubImage(_currX + xOffset, _currY + yOffset, destWidth, destHeight, data);

        if(needRealloc)
        {
            BEATS_SAFE_DELETE_ARRAY(data);
        }

        FontGlyph *glyph = new FontGlyph;
        glyph->width = (float)xAdvance;
        glyph->height = (float)_lineHeight;
        glyph->u = (float)_currX / PAGE_WIDTH;
        glyph->v = (float)_currY / PAGE_HEIGHT;
        glyph->texture = _textures[_currPage];
        _glyphs[character] = glyph;

        _currX += xAdvance;
    }
}

void FontFace::RenderText(const TString &text, kmScalar x, kmScalar y)
{
    auto glyphs = GetGlyphs(text);
    for(auto glyph : glyphs)
    {
        drawGlyph(glyph, x, y);
        x += glyph->width;
    }
}

void FontFace::drawGlyph(FontGlyph *glyph, kmScalar x, kmScalar y)
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
}

FontGlyph *FontFace::GetGlyph( unsigned long character )
{
    auto itr = _glyphs.find(character);
    return itr != _glyphs.end() ? itr->second : nullptr;
}

std::vector<FontGlyph *> FontFace::GetGlyphs(const TString &text)
{
    PrepareCharacters(text);
    std::vector<FontGlyph *> glyphs;
    for(auto character : text)
    {
        FontGlyph *glyph = GetGlyph(character);
        if(glyph)
            glyphs.push_back(glyph);
    }
    return glyphs;
}

const TString& FontFace::GetName()const
{
    return _name;
}
