#include "stdafx.h"
#include "LineListCache.h"

CLineListCache::CLineListCache(uint32_t maxVertexCount, IDirect3DDevice9* pRenderDevice, long vertexFVF, long vertexSize)
: super(maxVertexCount, pRenderDevice, vertexFVF, vertexSize, eRCT_Line)
{
    m_prmitiveType = D3DPT_LINELIST;
}


CLineListCache::~CLineListCache()
{

}

void CLineListCache::AddIndex()
{
    (*(m_pCurIndex++)) = (DWORD)m_vertexCounter;
}
