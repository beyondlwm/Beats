#ifndef GUI_FONT_FONT_FACE_H__INCLUDE
#define GUI_FONT_FONT_FACE_H__INCLUDE

class CTexture;

namespace FCGUI
{
    class FontGlyph
    {
    public:
        kmScalar u, v;
        kmScalar width, height;
        SharePtr<CTexture> texture;
    };

    class FontFace
    {
    public:
        FontFace(const TString &name, const std::string &file, int sizeInPt, int dpi);
        FontFace(const TString &name, const std::string &file, int sizeInPx);
        ~FontFace();

        void PrepareCharacters(const TString &chars);
        std::vector<FontGlyph *> GetGlyphs(const TString &text);
        FontGlyph *GetGlyph(unsigned long character);

    private:
        void init(const TString &name, const std::string &file);
        void newPage();

    private:
        TString _name;
        FT_Face _face;
        std::map<unsigned long, FontGlyph *> _glyphs;
        std::vector<SharePtr<CTexture> > _textures;
        int _currPage;
        size_t _currX, _currY;
        int _lineHeight;    //maxheight of current font face
        int _ascender;  //distance from top to baseline

        static const int PAGE_WIDTH = 1024;
        static const int PAGE_HEIGHT = 1024;

        static FT_Library _library;
        static int _libRefCount;
    };
}

#endif