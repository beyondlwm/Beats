#ifndef GUI_LAYOUT_HSCROLL_LAYOUT_H__INCLUDE
#define GUI_LAYOUT_HSCROLL_LAYOUT_H__INCLUDE

#include "ScrollLayout.h"

namespace FCGUI
{
    class HScrollLayout : public ScrollLayout
    {
    public:
        HScrollLayout(size_t rowCount = 1);

        void SetColumnWidth(kmScalar colWidth);

        virtual void PerformLayout() override;

    protected:
        kmScalar _colWidth;
    };
}

#endif