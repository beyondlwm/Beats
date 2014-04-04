#ifndef GUI_FONT_FONT_MANAGER_H__INCLUDE
#define GUI_FONT_FONT_MANAGER_H__INCLUDE

namespace FCGUI
{
    //forward declaration
    class FontFace;

    class FontManager
    {
        BEATS_DECLARE_SINGLETON(FontManager);
    public:
        FontFace *CreateFace(const TString &name, const std::string &file, int sizeInPx);
        FontFace *GetFace(const TString &name);
        void DrawText(const TString &text, kmScalar x, kmScalar y, const TString &faceName);

    private:
        std::map<TString, FontFace *> _faces;
    };
}

#endif