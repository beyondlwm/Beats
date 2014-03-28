#ifndef GUI_LAYOUT_SCROLL_LAYOUT_H__INCLUDE
#define GUI_LAYOUT_SCROLL_LAYOUT_H__INCLUDE

#include "GridLayout.h"

namespace FCGUI
{
    class ScrollLayout : public GridLayout
    {
    public:
        ScrollLayout(size_t rowCount, size_t colCount);

        virtual bool AddChild( Window *window ) override;

        virtual bool AddChild( Window *window, kmScalar x, kmScalar y ) override;

        virtual bool AddChildToCell( Window *window, size_t row, size_t col ) override;

        virtual bool RemoveChild( size_t row, size_t col ) override;

        virtual void PerformLayout() override = 0;

    protected:
        kmScalar _colWidth;
        kmScalar _rowHeight;
    };
}

#endif