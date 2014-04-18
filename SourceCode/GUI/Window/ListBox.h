#ifndef GUI_WINDOW_LISTBOX_H__INCLUDE
#define GUI_WINDOW_LISTBOX_H__INCLUDE

#include "Window.h"
#include "GUI/Font/FontFace.h"
namespace FCGUI
{
    class  ListBox : public Window
    {
        typedef std::vector<TString> StringList;
        DEFINE_WINDOW_TYPE(WINDOW_LISTBOX);

    public:
        enum EventType
        {
            EVENT_LISTBOX = FCGUI::EVENT_LISTBOX,
            EVENT_SELCHANGE,
        };

        ListBox(const TString &name);

         virtual bool OnMouseEvent( MouseEvent *event ) override;

        size_t GetCount( ) const;
        kmScalar SetItemHeight(kmScalar  itemHeight);
        kmScalar GetItemHeight( ) const;
        int GetCurrSel( ) const;
        int SetCurrSel( int select );
        size_t AddString( const TString &strText);
        void DeleteString( size_t index );
        void InsertString( int index, const TString &strText );
        const StringList& GetStringList() const;
        kmScalar GetPadding() const;
        Window*  GetSubSelWin();
        TString GetString(int index) const; 
    protected:
        void Init();
        bool IsContainPos(const kmVec3 &pos);
        void ShowSelBackGround();
        void CaclItemHeight();
    public:
         FontFace    *_pFontFace;
    private:
        Window   *_pSubSel;
        kmScalar _itemHeight;
        int _currSel;
        StringList _strList;

        kmScalar _padding;
    };
}
#endif