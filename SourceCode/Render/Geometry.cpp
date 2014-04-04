#include "stdafx.h"
#include "Geometry.h"

CPoint::CPoint(float x, float y):
    x(x), y(y)
{
}

CSize::CSize( float width, float height ):
    width(width), height(height)
{

}

CRect::CRect( float x, float y, float width, float height )
{
    point.x = x;
    point.y = y;
    size.x = width;
    size.y = height;
}
