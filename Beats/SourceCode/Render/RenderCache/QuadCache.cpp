#include "stdafx.h"
#include "QuadCache.h"

CQuadCache::CQuadCache(size_t maxVertexCount, IDirect3DDevice9* pRenderDevice, long vertexFVF, long vertexSize)
: super(maxVertexCount, pRenderDevice, vertexFVF, vertexSize, eRCT_Quad)
{
    
}

CQuadCache::~CQuadCache()
{

}

void CQuadCache::AddIndex()
{
    //we need sequence such as 0, 1, 2; 1, 3, 2; 4, 5, 6; 5, 7, 6; ¡ª¡ª clockwise
    (*(m_pCurIndex++)) = (DWORD)m_vertexCounter;
    if (m_vertexCounter % 4 > 1)
    {
        (*(m_pCurIndex++)) = (DWORD)m_vertexCounter - 1;
    }
}

