#include "stdafx.h"
#include "Font.h"

using namespace FCGUI;

FT_Library Font::_library = nullptr;
int Font::_libRefCount = 0;

Font::Font()
{
    if(_libRefCount++ == 0)
    {
        FT_Error err = FT_Init_FreeType(&_library);
        BEATS_ASSERT(!err);
    }
}

Font::~Font()
{
    if(--_libRefCount == 0)
    {
        FT_Error err = FT_Done_FreeType(_library);
        BEATS_ASSERT(!err);
    }
}

bool Font::Load()
{
    BEATS_ASSERT(!IsLoaded(), _T("Can't Load a font which is already loaded!"));

    size_t len = m_strPath.size() * 2 + 1;
    char *path = new char[len];
    CStringHelper::GetInstance()->ConvertToCHAR(m_strPath.c_str(), path, len);

    FT_Error err = FT_New_Face(_library, path, 0, &_face);
    BEATS_ASSERT(!err);

    err = FT_Select_Charmap(_face, FT_ENCODING_UNICODE);
    BEATS_ASSERT(!err);

    BEATS_SAFE_DELETE_ARRAY(path);

    SetLoadedFlag(!err);

    return !err;
}

bool Font::Unload()
{
    BEATS_ASSERT(IsLoaded(), _T("Can't Unload a font which haven't been loaded!"));

    FT_Error err = FT_Done_Face(_face);
    BEATS_ASSERT(!err);

    SetLoadedFlag(false);

    return !err;
}

FT_Face Font::Face() const
{
    return _face;
}
