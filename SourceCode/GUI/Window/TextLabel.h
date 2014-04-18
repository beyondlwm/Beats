#ifndef GUI_WINDOW_TEXTLABEL_H__INCLUDE
#define GUI_WINDOW_TEXTLABEL_H__INCLUDE

#include "Window.h"
#include "GUI/Font/FontFace.h"

namespace FCGUI
{
    class TextLabel : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_TEXTLABEL);
    public:
        enum EventType
        {
            EVENT_TEXTLABEL = FCGUI::EVENT_TEXTLABEL,
            EVENT_TEXTLABEL_PRESSED,
        };
        TextLabel(const TString &name);
        TextLabel(CSerializer serializer);
        virtual bool OnMouseEvent( MouseEvent *event ) override;
        const TString& GetDisplayText() const;
        void SetText(const TString &text);
        const TString& GetFontFileName() const;
        void SetFontFileName(const TString &fontFileName);
        int GetFontSize() const;
        void SetFontSize(int sizeInPt);
        kmScalar GetPadding() const;
        kmVec2 GetWorldPos();
    public:
         FontFace    *_pFontFace;
    protected:
        void Init();
    private:
        TString _displayText;
        kmScalar _padding;
    };
}

#endif
