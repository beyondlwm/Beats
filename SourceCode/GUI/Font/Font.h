#ifndef GUI_FONT_FONT_H__INCLUDE
#define GUI_FONT_FONT_H__INCLUDE

#include "Resource/Resource.h"
#include "Resource/ResourcePublic.h"

namespace FCGUI
{
    class Font : public CResource
    {
        DECLARE_RESOURCE_TYPE(eRT_Font);
    public:
        Font();
        virtual ~Font();

        virtual bool Load();

        virtual bool Unload();

        FT_Face Face() const;

    private:
        FT_Face _face;

        static FT_Library _library;
        static int _libRefCount;
    };
}

#endif