#ifndef GUI_WINDOW_COMBOBOX_H__INCLUDE
#define GUI_WINDOW_COMBOBOX_H__INCLUDE

#include "Window.h"

namespace FCGUI
{
    class Button;
    class TextLabel;
    class ListBox;
    class ComboBox : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_COMBOBOX);
    public:
        enum EventType
        {
            EVENT_COMBOBOX = FCGUI::EVENT_COMBOBOX,
            EVENT_DROPDOWN,
            EVENT_SELECTOK,
        };
        ComboBox( const TString &name );
        ComboBox( CSerializer serializer );
        ~ComboBox();
        
        int GetCurrSel( ) const;
        int SetCurrSel( int select );
        size_t AddString( const TString &strText);
        void DeleteString( size_t index );
        Button* GetDropDownButtton() const;
        ListBox* GetListBox() const;
        TextLabel* GetTextLabel() const;

        void OnDropDown(BaseEvent *baseEvent);
        void OnSelectOk(BaseEvent *baseEvent);
    protected:
        void IntiChild();
        void Init();
        void ReSizeChild();
    protected:
        Button   *_pDropDwonButton;
        TextLabel *_pSelectItem;
        ListBox *_pListBox;
        kmScalar _fDropDownButtonWidth;
        kmScalar _fListBoxHeight;
    };

}

#endif