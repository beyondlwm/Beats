#include "stdafx.h"
#include "RenderObjectManager.h"
#include "RenderObject.h"
#include "Model.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderManager.h"
#include "CurveRenderer.h"
#include "Spline\Curve.h"

CRenderObjectManager *CRenderObjectManager::m_pInstance = NULL;

CRenderObjectManager::CRenderObjectManager()
{

}

CRenderObjectManager::~CRenderObjectManager()
{
    for(auto renderObject:m_renderObjects3D)
    {
        delete renderObject;
    }
    for(auto renderObject:m_renderObjects2D)
    {
        delete renderObject;
    }
}

CModel *CRenderObjectManager::CreateModel()
{
    CModel *model = new CModel;
    m_renderObjects3D.push_back(model);
    return model;
}

CSprite *CRenderObjectManager::CreateSprite(
    SharePtr<CTexture> texture, const CRect &rect, const CSize &size)
{
    CSprite *sprite = new CSprite(texture, rect, size);
    m_renderObjects2D.push_back(sprite);
    return sprite;
}

CCurveRenderer *CRenderObjectManager::CreateCurve(SharePtr<Spline> spline)
{
    CCurveRenderer *curve = new CCurveRenderer(spline);
    m_renderObjects2D.push_back(curve);
    return curve;
}

void CRenderObjectManager::Render()
{
    FC_CHECK_GL_ERROR_DEBUG();

    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    pRenderMgr->SetupVPMatrix(false);
    for(auto renderObject : m_renderObjects3D)
    {
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->PreRender();
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->Render();
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->PostRender();
        FC_CHECK_GL_ERROR_DEBUG();
    }
    pRenderMgr->SetupVPMatrix(true);
    for(auto renderObject : m_renderObjects2D)
    {
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->PreRender();
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->Render();
        FC_CHECK_GL_ERROR_DEBUG();
        renderObject->PostRender();
        FC_CHECK_GL_ERROR_DEBUG();
    }
}
