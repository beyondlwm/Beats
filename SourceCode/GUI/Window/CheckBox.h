#ifndef GUI_WINDOW_CHECKBOX_H__INCLUDE
#define GUI_WINDOW_CHECKBOX_H__INCLUDE

#include "Window.h"

namespace FCGUI
{
    class CheckBox : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_CHECKBOX);
    public:
        enum EventType
        {
            EVENT_BUTTON = FCGUI::EVENT_CHECKBOX,
            EVENT_CHECK,
        };

        enum State
        {
            STATE_NORMAL_UNCHECKED,
            STATE_NORMAL_CHECKED,
            STATE_DISABLED_UNCHECKED,
            STATE_DISABLED_CHECKED,
            STATE_COUNT,
        };

    public:
        CheckBox( const TString &name );
        CheckBox( CSerializer serializer);
        ~CheckBox();
        
        virtual bool OnMouseEvent( MouseEvent *event ) override;
        bool         IsCheck() const;
        void         SetCheck( bool bCheck );
        virtual int  CurrState() const override;
        Window*      GetSubWindow();
    protected:
        void InitChild();
        void Init();
    protected:
        bool         _bCheck;
        State        _state;
        Window*      _pSubContent;

        kmScalar _fContentWidth;
    };
}

#endif
