#ifndef RENDER_RENDEROBJECTMANAGER_H__INCLUDE
#define RENDER_RENDEROBJECTMANAGER_H__INCLUDE
#include "Geometry.h"

class CRenderObject;
class CModel;
class CSprite;
class CCurveRenderer;
class CTexture;
struct Spline;

class CRenderObjectManager
{
    BEATS_DECLARE_SINGLETON(CRenderObjectManager);
public:
    CModel *CreateModel();
    CSprite *CreateSprite(const TString &textureFragName, const kmVec2 &size);
    CCurveRenderer *CreateCurve(SharePtr<Spline> spline);
    void Render();

private:
    std::list<CRenderObject *> m_renderObjects3D;
    std::list<CRenderObject *> m_renderObjects2D;
};

#endif