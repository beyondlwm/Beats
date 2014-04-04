#include "stdafx.h"
#include "FontFace.h"
#include "Render/Texture.h"

//next multiple of 8
inline int nextMOE(int x)
{
    return (x + 7) & 0xFFFFFFF8;
}

using namespace FCGUI;

FT_Library FontFace::_library = nullptr;
int FontFace::_libRefCount = 0;

FontFace::FontFace(const TString &name, const std::string &file, int sizeInPt, int dpi)
{
    init(name, file);

    FT_Error err = 0;
    FT_F26Dot6 size = sizeInPt << 6;
    err = FT_Set_Char_Size(_face, size, size, dpi, dpi);
    BEATS_ASSERT(!err);
}

FontFace::FontFace(const TString &name, const std::string &file, int sizeInPx)
{
    init(name, file);

    FT_Error err = 0;
    err = FT_Set_Pixel_Sizes(_face, sizeInPx, sizeInPx);
    BEATS_ASSERT(!err);
}

void FontFace::init(const TString &name, const std::string &file)
{
    FT_Error err = 0;
    if(_libRefCount++ == 0)
    {
        err = FT_Init_FreeType(&_library);
        BEATS_ASSERT(!err);
    }

    _name = name;
    _currPage = -1;
    _currX = 0;
    _currY = 0;
    _lineHeight = 0;
    _ascender = 0;

    err = FT_New_Face(_library, file.c_str(), 0, &_face);
    BEATS_ASSERT(!err);

    err = FT_Select_Charmap(_face, FT_ENCODING_UNICODE);
    BEATS_ASSERT(!err);

    newPage();
}

FontFace::~FontFace()
{
    for(auto glyph : _glyphs)
    {
        BEATS_SAFE_DELETE(glyph.second);
    }

    FT_Error err = 0;
    err = FT_Done_Face(_face);
    BEATS_ASSERT(!err);

    if(--_libRefCount == 0)
    {
        err = FT_Done_FreeType(_library);
        BEATS_ASSERT(!err);
    }
}

void FCGUI::FontFace::newPage()
{
    SharePtr<CTexture> texture = new CTexture;
    texture->InitWithData(nullptr, 0, PixelFormat::A8, PAGE_WIDTH, PAGE_HEIGHT);
    ++_currPage;
    _textures.push_back(texture);
}

void FontFace::PrepareCharacters( const TString &chars )
{
    FT_Error err = 0;
#ifndef _UNICODE
    //TODO: translate chars to unicode
#pragma error("non-unicode unsupported")
#endif

    for(size_t i = 0; i < chars.size(); ++i)
    {
        wchar_t character = chars[i];

        if(_glyphs.find(character) != _glyphs.end())
            continue;

        err = FT_Load_Char(_face, character, FT_LOAD_RENDER);
        BEATS_ASSERT(!err);

        BEATS_ASSERT(_ascender == 0 || _ascender == _face->size->metrics.ascender >> 6);
        _ascender = _face->size->metrics.ascender >> 6;
        long descender = _face->size->metrics.descender >> 6;
        BEATS_ASSERT(_lineHeight == 0 || _lineHeight == _ascender - descender);
        _lineHeight = _ascender - descender;

        FT_Bitmap &bitmap = _face->glyph->bitmap;
        FT_Glyph_Metrics &metrics = _face->glyph->metrics;
        long x = metrics.horiBearingX >> 6;
        long y = metrics.horiBearingY >> 6;
        long xAdvance = metrics.horiAdvance >> 6;
        long width = metrics.width >> 6;
        long height = metrics.height >> 6;
        width, height;

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