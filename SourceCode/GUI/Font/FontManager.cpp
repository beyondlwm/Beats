#include "stdafx.h"
#include "FontManager.h"
#include "FontFace.h"
#include "Render/Texture.h"

using namespace FCGUI;

FontManager *FontManager::m_pInstance = nullptr;

FontManager::FontManager()
{

}

FontManager::~FontManager()
{
    for(auto face : _faces)
    {
        BEATS_SAFE_DELETE(face.second);
    }
}

FontFace *FontManager::CreateFace( const TString &name, const TString &file, int sizeInPt, int dpi )
{
    auto itr = _faces.find(name);
    BEATS_ASSERT(itr == _faces.end(), _T("Font face:%s already existed"), name.c_str());

    FontFace *face = new FontFace(name, file, sizeInPt, dpi);
    _faces[name] = face;
    return face;
}

FontFace *FontManager::GetFace( const TString &name )
{
    auto itr = _faces.find(name);
    return itr != _faces.end() ? itr->second : nullptr;
}

void FontManager::RenderText( const TString &text, kmScalar x, kmScalar y, const TString &faceName )
{
    FontFace *face = GetFace(faceName);
    BEATS_ASSERT(face);

    face->RenderText(text, x, y);
}
