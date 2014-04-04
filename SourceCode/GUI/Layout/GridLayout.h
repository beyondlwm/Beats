#ifndef GUI_LAYOUT_GRID_LAYOUT_H__INCLUDE
#define GUI_LAYOUT_GRID_LAYOUT_H__INCLUDE

#include "BaseLayout.h"

namespace FCGUI
{
    //forward declaration
    class Window;

    class GridLayout : public BaseLayout
    {
    public:
        GridLayout(size_t rowCount, size_t colCount);

        virtual void SetWindow(Window *window) override;

        void SetRowCount(size_t rowCount);
        size_t RowCount() const;
        void SetColCount(size_t colCount);
        size_t ColCount() const;

        void SetMargin(kmScalar topMargin, kmScalar bottomMargin, 
            kmScalar leftMargin, kmScalar rightMargin);
        kmScalar TopMargin() const;
        kmScalar BottomMargin() const;
        kmScalar LeftMargin() const;
        kmScalar RightMargin() const;
        void SetGap(kmScalar hGap, kmScalar vGap);
        kmScalar HGap() const;
        kmScalar VGap() const;

        void SetRowHeight(kmScalar rowHeight);
        void SetColWidth(kmScalar colWidth);
        void SetRowHeightSingleFixed(size_t row, kmScalar height);
        void SetRowHeightSingleFactored(size_t row, kmScalar heightFactor);
        void SetColWidthSingleFixed(size_t col, kmScalar width);
        void SetColWidthSingleFactored(size_t col, kmScalar widthFactor);

        virtual bool AddChild(Window *window);
        virtual bool AddChild(Window *window, kmScalar x, kmScalar y);
        virtual bool AddChildToCell(Window *window, size_t row, size_t col);
        virtual bool RemoveChild(Window *window);
        Window *GetChild(size_t row, size_t col);

        virtual void PerformLayout() override;

    protected:
        void calcSplitPos();

    protected:
        size_t _rowCount;
        size_t _colCount;
        kmScalar _topMargin, _bottomMargin, _leftMargin, _rightMargin;
        kmScalar _hGap, _vGap;
        std::vector<std::pair<kmScalar, kmScalar> > _everyRowHeight;    //first:fixed second:factor
        kmScalar _rowHeight;
        std::vector<std::pair<kmScalar, kmScalar> > _everyColWidth;     //first:fixed second:factor
        kmScalar _colWidth;
        std::vector<std::pair<kmScalar, kmScalar> > _hSplitPos;
        std::vector<std::pair<kmScalar, kmScalar> > _vSplitPos;

        std::vector<Window *> _children;
    };
}

#endif