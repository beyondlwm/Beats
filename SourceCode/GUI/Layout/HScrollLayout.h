#ifndef GUI_LAYOUT_HSCROLL_LAYOUT_H__INCLUDE
#define GUI_LAYOUT_HSCROLL_LAYOUT_H__INCLUDE

#include "ScrollLayout.h"

namespace FCGUI
{
    class HScrollLayout : public ScrollLayout
    {
    public:
        HScrollLayout(size_t rowCount = 1);

        virtual void PerformLayout() override;

        virtual bool AddChild( Window *window, kmScalar x, kmScalar y ) override;

    };
}

#endif