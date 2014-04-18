#ifndef GUI_WINDOW_TEXTEDIT_H__INCLUDE
#define GUI_WINDOW_TEXTEDIT_H__INCLUDE

#include "Window.h"
#include "GUI/Font/FontFace.h"

namespace FCGUI
{
    class EditBoxImpl;
    class  TextEdit : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_TEXTEDIT);
    public:
        enum EventType
        {
            EVENT_TEXTEDIT = FCGUI::EVENT_TEXTEDIT,
            EVENT_TEXTEDIT_BEGINEDIT,
            EVENT_TEXTEDIT_VALUECHANGE,
            EVENT_TEXTEDIT_ENDEDIT,
        };

        enum Mode
        {
            MODE_TEXTEDIT,
            MODE_TEXTLABEL,
        };

        TextEdit( const TString &name );
        TextEdit( CSerializer serializer);
        ~ TextEdit();

        void  SetText( const TString &text );
        const TString& GetText() const;
        int   GetFontSize()const;
        void  SetFontSize(int sizeInPt);
        const TString& GetFontFileName()const;
        void  SetFontFileName(const TString& name);
        int   GetTextSize()const;
        virtual int CurrState() const override;
        virtual bool OnMouseEvent( MouseEvent *event ) override;
        virtual bool OnKeyboardEvent(KeyboardEvent *event);

        void InsertText(const TString &text, int nPos);
        void PositionTextPos(const kmVec3 &localPos);
        void DeleteBackward();
        
        int GetLineCount() const;
        kmScalar GetPadding() const;
        kmScalar GetLineMaxHeight(size_t indexLine) const;
        const std::vector<TString>& GetTextOfLines();
        void SetMode(const Mode &mode);
        const Mode& GetMode() const;

        FontFace    *_pFontFace;
    protected:
        void Init();
        void CalcLineLimit();
        void ProcessLineBreak(TString strAdd);
        
        bool IsPassedRowLimit();
        bool IsContainPos(const kmVec3 &pos);
        void CalcStringOfLine();
        void AddText(wchar_t wchar);
    private:
        Mode _mode;
        std::vector<TString>  _textOfLine;
        TString   _text;
        bool      _bEdit;
        bool      _bInsert;

        int  _maxLength;
        int  _currCharPos;
        size_t _lineCount;
        kmScalar _padding;

        kmVec2 _worldPos;
    };
}

#endif