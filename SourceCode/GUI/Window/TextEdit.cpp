#include "stdafx.h"
#include "TextEdit.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "GUI/WindowManager.h"
#include "GUI/Font/FontManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"

inline static void SplitString(const TString &inputStr, char splitChar, std::vector<TString> &outStrVec)
{
    std::string::size_type start = inputStr.find_first_not_of(splitChar);
    std::string::size_type stop = start;
    while (stop != std::string::npos)
    {
        stop =inputStr.find_first_of(splitChar, start);
        if(stop != std::string::npos)
            outStrVec.push_back(inputStr.substr(start, stop-start));
        else
        {
            outStrVec.push_back(inputStr.substr(start));  
        }
        start = inputStr.find_first_not_of(splitChar, stop);
        stop = start;
    }
}

using namespace FCGUI;

TextEdit::TextEdit(const TString &name)
    : Window(name)
{
    Init();
}

TextEdit::TextEdit( CSerializer serializer)
    :Window( serializer )
{
    Init();
    DECLARE_PROPERTY(serializer, _padding, true, 0xFFFFFFFF, _T("±ß¿ò´óÐ¡"), NULL, NULL, NULL);
}

TextEdit::~ TextEdit()
{

}

void TextEdit::Init()
{
    _currCharPos = 0;
    _lineCount = 0;
    _bInsert = false;
    _padding = 4.f;
    _mode = MODE_TEXTEDIT;
    kmVec2Fill(&_worldPos, 0.f, 0.f );
    _pFontFace  = FontManager::GetInstance()->GetFace(_T("STFANGSO_12"));
}

int TextEdit::CurrState() const
{
    return 0;
}

int   TextEdit::GetTextSize()const
{
    return _text.size();
}

void TextEdit::SetText(const TString &text)
{
    if(!text.empty())
    {
        _text = text;
    }
}

const TString& TextEdit::GetText() const
{
    return _text;
}

void TextEdit::InsertText(const TString &text, int nPos)
{
    _text.insert(nPos,text);
}

void TextEdit::DeleteBackward()
{
    if(!_text.empty() && _currCharPos > 0)
    {
        BEATS_ASSERT( (size_t)_currCharPos <= _text.size());
        if(_text[_currCharPos-1] == ('\n'))
        {
            _text.erase(_currCharPos-1,1);
            _currCharPos--;
        }
        _text.erase(_currCharPos - 1, 1);
        _currCharPos--;
    }   
}

int TextEdit::GetLineCount()const
{
    return _lineCount;
}

kmScalar TextEdit::GetPadding() const
{
    return _padding;
}

kmScalar TextEdit::GetLineMaxHeight(size_t indexLine) const
{
   BEATS_ASSERT(indexLine < _textOfLine.size())

   TString strText = _textOfLine[indexLine];

   kmScalar fMaxHeight = 0.f;
   size_t size = strText.size();

   for( size_t i = 0; i < size; ++i)
   {
        FontGlyph* pFontglyph = _pFontFace->GetGlyph(strText[i]);
        if(pFontglyph && pFontglyph->height > fMaxHeight)
        {
            fMaxHeight = pFontglyph->height;
        }
   }
   return fMaxHeight;
}

const std::vector<TString>& TextEdit::GetTextOfLines()
{
    CalcStringOfLine();
    return _textOfLine;
}

void TextEdit::SetMode(const Mode &mode)
{
    _mode = mode;
}

const TextEdit::Mode& TextEdit::GetMode() const
{
    return _mode;
}

void TextEdit::PositionTextPos(const kmVec3 &localPos)
{
    if(!_text.empty())
    {
        kmScalar linewidth = 2 * _padding;
        kmScalar rowheight = 2 * _padding;
        kmScalar lastRowHeight = rowheight;
        int charIndex = 0;
        int rowIndex = 0;

        for(size_t i = 0; i < _text.size(); ++i)
        {
            if(_text[i] == '\n')
            {
                rowIndex++;
                linewidth = 2 * _padding;
                lastRowHeight = rowheight;
            }
            else
            {
                FontGlyph* pFontglyph = _pFontFace->GetGlyph(_text[i]);
                BEATS_ASSERT(pFontglyph != nullptr);
                linewidth += pFontglyph->width;

                rowheight =  lastRowHeight + GetLineMaxHeight(rowIndex);
            }
            charIndex++;
            if(rowheight > localPos.y && linewidth > localPos.x)
                break;
        } 
        if((size_t)charIndex < _text.size())
        {
            _bInsert = true;
        }
        else
        {
            _bInsert = false;
        }
        _currCharPos = charIndex;
    } 
}

bool TextEdit::OnMouseEvent( MouseEvent *event )
{
    bool bRet = false;
    if( MODE_TEXTEDIT == _mode && IsEnabled() )
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_PRESSED)
            {
                kmVec3 pos;
                kmVec3Fill( &pos,event->X(), event->Y(), 0.f );
                Window::WorldToLocal( pos );

                if(IsContainPos(pos))
                {
                    PositionTextPos(pos);

                    _bEdit = true;
                    WindowEvent event(EVENT_TEXTEDIT_BEGINEDIT);
                    DispatchEvent(&event);
                }
                else
                {
                    _bEdit = false;
                } 
            }
        }
        bRet = true;
    }
    return bRet;
}

bool TextEdit::OnKeyboardEvent(KeyboardEvent *event)
{
	bool bRet = false;
    if(IsEnabled() && _bEdit)
    {
        if(event->Type() == EVENT_CHAR)
        {
            TString strPreAdd ;
            strPreAdd += event->WChar();
       
            ProcessLineBreak(strPreAdd);
            if(_bEdit)
                AddText(event->WChar());
        } 
        else if( event->Type() == EVENT_KEY_PRESSED )
        {
            if( GLFW_KEY_ENTER == event->Key())
            {
                _text += _T("\n");
                _lineCount ++;
            }
            else if( GLFW_KEY_BACKSPACE == event->Key())
            {
                DeleteBackward();
            }
        }
        else if( event->Type() == EVENT_KEY_REPEATED )
        {
            if( GLFW_KEY_BACKSPACE == event->Key())
            {
                DeleteBackward();
            }
        }
        bRet = true;
    }
    return bRet;
}

void TextEdit::ProcessLineBreak(TString strAdd)
{
    TString result;
    const char splitChar = _T('\n');
    std::string::size_type start = _text.find_first_not_of(splitChar);
    std::string::size_type stop = start;
    while (stop != std::string::npos)
    {
        stop =_text.find_first_of(splitChar, start);
        if(stop == std::string::npos)
           result = _text.substr(start);
        start = _text.find_first_not_of(splitChar, stop);
        stop = start;
    }
    result += strAdd;
    kmScalar linewidth = 0;
    _pFontFace->PrepareCharacters(result);
    for(size_t i = 0; i < result.size(); ++i)
    {
        FontGlyph* pFontglyph = _pFontFace->GetGlyph(result[i]);
        BEATS_ASSERT(pFontglyph);
        linewidth += pFontglyph->width;
    }

    linewidth += 2 * _padding;
    if( linewidth  > Size().x )
    {
        if(IsPassedRowLimit())
        {
            WindowEvent event(EVENT_TEXTEDIT_ENDEDIT);
            DispatchEvent(&event);

            _bEdit = false;
        }
        else
        {
            WindowEvent event(EVENT_TEXTEDIT_VALUECHANGE);
            DispatchEvent(&event);

            _text += _T("\n");
            _lineCount++;
            _currCharPos++;
        }
    } 
}

bool TextEdit::IsPassedRowLimit()
{   
    size_t size = _textOfLine.size();
    kmScalar fToTalHeight = 0.f;
    kmScalar fLastHeight = 0.f;
    for(size_t i = 0; i < size; ++i)
    {
        kmScalar fMaxHeightOfLine = GetLineMaxHeight(i);
        fToTalHeight += fMaxHeightOfLine;
        if( i == size - 1)
            fLastHeight = fMaxHeightOfLine;
    }
    fToTalHeight += fLastHeight;
    fToTalHeight += 2 * _padding;

    return fToTalHeight > Size().y;
}

bool TextEdit::IsContainPos(const kmVec3 &pos)
{
    kmVec2 size = RealSize();
    return (pos.x >=0 && pos.x <= size.x 
        && pos.y >=0 && pos.y <= size.y );
}

void TextEdit::CalcStringOfLine()
{
    _textOfLine.clear();
    SplitString(_text,_T('\n'), _textOfLine);
}

void TextEdit::AddText(wchar_t wchar)
{
    if( !_bInsert )
    {
        _text += wchar;
        _currCharPos++;
    }
    else
    {
        const TString string = _text.substr(_currCharPos);
        _text = _text.substr(0,_currCharPos);
        _text += wchar;
        _text += string;

        std::string::size_type pos  = _text.find_first_of(_T('\n'), _currCharPos);
        while(pos !=  std::string::npos)
        {
            _text.erase(pos, 1);
            _text.insert(pos-1,_T("\n"));
             pos  = _text.find_first_of(_T('\n'), pos);
        }
    }
}

