#ifndef BEATS_RENDER_LINELISTCACHE_H__INCLUDE
#define BEATS_RENDER_LINELISTCACHE_H__INCLUDE

#include "RenderCacheBase.h"

class CLineListCache : public CRenderCacheBase
{
    typedef CRenderCacheBase super;
public:
    CLineListCache(uint32_t maxVertexCount, IDirect3DDevice9* m_pRenderDevice, long vertexFVF, long vertexSize);
    virtual ~CLineListCache();

    virtual void AddIndex();

};

#endif