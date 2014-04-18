#ifndef GUI_FONT_FONT_FACE_H__INCLUDE
#define GUI_FONT_FONT_FACE_H__INCLUDE

class CTexture;

namespace FCGUI
{
    class Font;

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
        FontFace(const TString &name, const TString &file, int size, int dpi = -1);
        ~FontFace();

        void PrepareCharacters(const TString &chars);
        void RenderText(const TString &text, kmScalar x, kmScalar y);
        std::vector<FontGlyph *> GetGlyphs(const TString &text);
        FontGlyph *GetGlyph(unsigned long character);
        const TString& GetName()const;   

    private:
        void newPage();
        void setSize();
        void drawGlyph(FontGlyph *glyph, kmScalar x, kmScalar y);

    private:
        TString _name;
        SharePtr<Font> _font;
        std::map<unsigned long, FontGlyph *> _glyphs;
        std::vector<SharePtr<CTexture> > _textures;
        int _currPage;
        size_t _currX;
		size_t _currY;
        int _size;
		int _dpi;
        int _lineHeight;    //maxheight of current font face
        int _ascender;  //distance from top to baseline

        static const int PAGE_WIDTH = 1024;
        static const int PAGE_HEIGHT = 1024;
    };
}

#endif