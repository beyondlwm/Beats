#ifndef RENDER_CURVE_RENDERER_H__INCLUDE
#define RENDER_CURVE_RENDERER_H__INCLUDE
#include "RenderObject.h"
#include "CommonTypes.h"

struct Spline;
class CShaderProgram;
class CCurveRenderer : public CRenderObject
{
public:
    CCurveRenderer(SharePtr<Spline> spline);
    ~CCurveRenderer();

    virtual void PreRender();

    virtual void Render();

    virtual void PostRender();

    void SetProgram(CShaderProgram* program);

private:
    void renderGrid();
    void renderSpline();
    void renderPoints();

private:
    CShaderProgram* m_pProgram;
    SharePtr<Spline> m_spline;
    GLuint m_VBOGrid;
    GLuint m_VBOSpline;
    GLuint m_VBOPoints;

    static const int SPLINE_SEGMENTS = 32;
    static const int GRID_X_MAX = 500;
    static const int GRID_Y_MAX = 500;
    static const int GRID_WIDTH = 50;
    static const int GRID_HEIGHT = 50;
    static const int GRID_VERTEX_NUM = (GRID_X_MAX/GRID_WIDTH+1)*2 + (GRID_Y_MAX/GRID_HEIGHT+1)*2;
    static const CColor GRID_COLOR;
    static const CColor SPLINE_COLOR;
    static const CColor POINT_COLOR;
};

#endif