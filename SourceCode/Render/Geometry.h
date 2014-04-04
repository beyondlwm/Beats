#ifndef RENDER_GEOMETRY_H__INCLUDE
#define RENDER_GEOMETRY_H__INCLUDE

class CPoint
{
public:
    float x, y;

    CPoint(float x = 0.f, float y = 0.f);
};

class CSize
{
public:
    float width, height;

    CSize(float width = 0.f, float height = 0.f);
};

class CRect
{
public:
    kmVec2 point;
    kmVec2 size;

    CRect(float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f);
};

#endif