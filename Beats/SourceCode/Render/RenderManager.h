#ifndef BEATS_RENDER_RENDERMANAGER_H__INCLUDE
#define BEATS_RENDER_RENDERMANAGER_H__INCLUDE

#include "RenderPublicDef.h"
#include "../../../Utility/Math/Vector3.h"

class CRenderUnit;
class CBaseObject;
class CCamera;
class CRenderCacheBase;
template<typename T>
class Function;

class CRenderManager
{
    BEATS_DECLARE_SINGLETON(CRenderManager);

public:
    void Init(const HWND& hwnd, int width, int height, bool bWindow);

    void Update(float deltaMs);    
    IDirect3DDevice9* GetDevice() const;
    void Register(CRenderUnit* pUnit);
    CRenderUnit* AcquireUnit(CBaseObject* pOwner);
    void SetCamera(CCamera* pCamera);

    void SendVertexToCache(ERenderCacheType cacheType, const void* pVertex, const Function<void(void)>* function = NULL);
    void SendQuadToCache( const CVector3& center, float width, float height, long color, const Function<void(void)>* function = NULL);

private:
    void RenderAxises();

private:
    CCamera* m_pCamera;
    IDirect3DDevice9* m_pRenderDevice;
    bool m_bInit;
    std::vector<CRenderCacheBase*> m_caches;
    //Never erase element in m_unitsContainer, we recycle use them with mark and sweep
    std::vector<size_t> m_recycleUnitId;
    std::vector<CRenderUnit*> m_unitsContainer;
    D3DPRESENT_PARAMETERS m_d3dPresentParam;
};

#endif
