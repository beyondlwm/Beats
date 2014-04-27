#ifndef GUI_FONT_FONT_MANAGER_H__INCLUDE
#define GUI_FONT_FONT_MANAGER_H__INCLUDE

namespace FCGUI
{
    //forward declaration
    class FontFace;
    class FontGlyph;

    class FontManager
    {
        BEATS_DECLARE_SINGLETON(FontManager);
    public:
        FontFace *CreateFace(const TString &name, const TString &file, int sizeInPt, int dpi = -1);
        FontFace *GetFace(const TString &name);
        void RenderText(const TString &text, kmScalar x, kmScalar y, const TString &faceName);

    private:
        std::map<TString, FontFace *> _faces;
    };
}

#endif