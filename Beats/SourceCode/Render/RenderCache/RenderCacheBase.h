#ifndef BEATS_RENDER_RENDERCACHEBASE_H__INCLUDE
#define BEATS_RENDER_RENDERCACHEBASE_H__INCLUDE

#include "../RenderPublicDef.h"

template<typename T>
class Function;

class CRenderCacheBase
{
public:
    CRenderCacheBase(size_t maxVertexCount, IDirect3DDevice9* m_pRenderDevice, long vertexFVF, long vertexSize,  ERenderCacheType type);
    virtual ~CRenderCacheBase();
    
    virtual void AddIndex() = 0;
    void CreateBuffer();
    void DestroyBuffer();

    void AddToCache(const void* pVertexData, const Function<void(void)>* pFunction = NULL);

    void PreRender();
    void Render();
    void PostRender();

    void SetPrimitiveType(D3DPRIMITIVETYPE type);
    D3DPRIMITIVETYPE GetPrimitiveType();

protected:
    DWORD* m_pCurIndex;
    size_t m_vertexCounter;
    size_t m_maxVertexCount;
    size_t m_functionCounter;
    D3DPRIMITIVETYPE m_prmitiveType;
    ERenderCacheType m_type;
    long m_vertexFVF;
    long m_vertexSize;
    void* m_pCurVertex;
    IDirect3DDevice9* m_pRenderDevice;
    IDirect3DVertexBuffer9* m_pVertexBuffer;
    IDirect3DIndexBuffer9* m_pIndexBuffer;
    std::vector<std::pair<size_t, Function<void(void)>>> m_functionCache;
};

#endif