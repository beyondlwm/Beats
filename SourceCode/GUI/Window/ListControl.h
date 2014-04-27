#ifndef GUI_WINDOW_LISTCONTROL_H__INCLUDE
#define GUI_WINDOW_LISTCONTROL_H__INCLUDE

#include "Window.h"

namespace FCGUI
{
    class  ListControl : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_LISTCONTROL);
    public:
        enum EventType
        {
            EVENT_LISTCONTROL = FCGUI::EVENT_LISTCONTROL,
            EVENT_MOVE,
        };
         ListControl(const TString &name);
         ListControl(CSerializer serializer);
        ~ ListControl();
        virtual bool OnMouseEvent( MouseEvent *event ) override;
        void AddItem(Window *item);
        void RemoveItem(Window *item);
        Window* GetItem(size_t row, size_t col);

        void SetRowHeight(kmScalar rowHeight);
        void SetColWidth(kmScalar colWidth);
    protected:
        void Init();
        void InitLayout();
        void CalcScrollOffsetRange();
        void CheckScrollOffset(kmVec2 &offset);
    private:
        bool _bPressed;
        kmVec2 _pressedPos;
        kmVec2 _scrollOfffsetRange;
        kmVec2 _scrollOffset;

        kmScalar _rowHeight;
        kmScalar _colWidth;
        kmScalar _topMargin, _bottomMargin, _leftMargin, _rightMargin;
        kmScalar _hGap, _vGap;

    };
}

#endif