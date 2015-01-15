#ifndef BEATS_RENDER_QUADCACHE_H__INCLUDE
#define BEATS_RENDER_QUADCACHE_H__INCLUDE

#include "RenderCacheBase.h"

class CQuadCache : public CRenderCacheBase
{
    typedef CRenderCacheBase super;
public:
    CQuadCache(uint32_t maxVertexCount, IDirect3DDevice9* pRenderDevice, long vertexFVF, long vertexSize);
    virtual ~CQuadCache();

public:
    virtual void AddIndex();

};

#endif