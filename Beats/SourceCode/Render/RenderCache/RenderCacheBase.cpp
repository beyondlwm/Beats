#include "stdafx.h"
#include "RenderCacheBase.h"
#include "../../Function/Function.h"

CRenderCacheBase::CRenderCacheBase(uint32_t maxVertexCount, IDirect3DDevice9* pRenderDevice, long vertexFVF, long vertexSize,ERenderCacheType type)
: m_maxVertexCount(maxVertexCount)
, m_vertexCounter(0)
, m_functionCounter(0)
, m_prmitiveType(D3DPT_TRIANGLELIST)
, m_type(type)
, m_vertexFVF(vertexFVF)
, m_vertexSize(vertexSize)
, m_pVertexBuffer(NULL)
, m_pIndexBuffer(NULL)
, m_pRenderDevice(pRenderDevice)
{
    CreateBuffer();
}

CRenderCacheBase::~CRenderCacheBase()
{
    DestroyBuffer();
}

void CRenderCacheBase::AddToCache( const void* pVertexData, const Function<void(void)>* pFunction /*= NULL*/ )
{
    memcpy(m_pCurVertex, pVertexData, m_vertexSize);
    m_pCurVertex = (char*)m_pCurVertex + m_vertexSize;
    AddIndex();
    if (pFunction != NULL)
    {
        if (m_functionCache.size() == 0 || m_functionCache.back().second != (*pFunction))
        {
            m_functionCache.push_back(std::pair<uint32_t, Function<void(void)>>(m_vertexCounter, *pFunction));
        }
    }
    ++m_vertexCounter;
    BEATS_ASSERT(m_vertexCounter <= m_maxVertexCount, _T("Can't add more vertex in render cache!"));
}


void CRenderCacheBase::PreRender()
{
    m_pVertexBuffer->Unlock();
    m_pIndexBuffer->Unlock();

    m_pRenderDevice->SetFVF(m_vertexFVF);
    HRESULT hr = m_pRenderDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_vertexSize);
    BEATS_ASSERT(hr == D3D_OK, _T("Set Stream Failed!"));
    hr = m_pRenderDevice->SetIndices(m_pIndexBuffer);
    BEATS_ASSERT(hr == D3D_OK, _T("Set Index Buffer Failed!"));
}

void CRenderCacheBase::PostRender()
{
    HRESULT hr = D3D_OK;
    hr = m_pVertexBuffer->Lock(0, 0, (void**)(&m_pCurVertex), D3DLOCK_NOSYSLOCK);
    BEATS_ASSERT(hr == D3D_OK);
    m_pIndexBuffer->Lock(0, 0, (void**)(&m_pCurIndex), D3DLOCK_NOSYSLOCK);
    BEATS_ASSERT(hr == D3D_OK);
    m_vertexCounter = 0;
    m_functionCounter = 0;
    m_functionCache.clear();
}

void CRenderCacheBase::Render()
{
    PreRender();
    BEATS_ASSERT(m_vertexCounter <= m_maxVertexCount, _T("The vertex buffer is overflow!"));
    if (m_functionCache.size() == 0)
    {
        if (m_vertexCounter > 0)
        {
            m_pRenderDevice->DrawIndexedPrimitive(m_prmitiveType, 0, 0, (UINT)m_vertexCounter, 0,  (UINT)(m_vertexCounter * 0.5f));
        }
    }
    else
    {
        //handle first scope.
        uint32_t firstVertexIndexForFunc = m_functionCache[0].first;
        if (firstVertexIndexForFunc > 0)
        {
            m_pRenderDevice->DrawIndexedPrimitive(m_prmitiveType, (UINT)0, 0, (UINT)firstVertexIndexForFunc - 1, (UINT)0, (UINT)((firstVertexIndexForFunc - 1) * 0.5f));
        }

        while (m_functionCounter < m_functionCache.size())
        {
            uint32_t startVertexIndex = m_functionCache[m_functionCounter].first;
            uint32_t endVertexIndex = (m_functionCounter + 1 < m_functionCache.size()) ? m_functionCache[m_functionCounter + 1].first - 1 : m_vertexCounter - 1;
            uint32_t vertexCount = 1 + (endVertexIndex - startVertexIndex);

            m_functionCache[m_functionCounter].second();

            m_pRenderDevice->DrawIndexedPrimitive(m_prmitiveType, (UINT)startVertexIndex, 0, (UINT)vertexCount, 0, (UINT)(vertexCount * 0.5f));

            ++m_functionCounter;

        }
    }
    PostRender();
}

void CRenderCacheBase::SetPrimitiveType(D3DPRIMITIVETYPE type)
{
    m_prmitiveType = type;
}

D3DPRIMITIVETYPE CRenderCacheBase::GetPrimitiveType()
{
    return m_prmitiveType;
}

void CRenderCacheBase::CreateBuffer()
{
    if (m_pVertexBuffer == NULL &&
        m_pIndexBuffer == NULL)
    {
        BEATS_ASSERT(m_maxVertexCount % 4 == 0, _T("max vertex count must be multiple of 4!"));
        HRESULT hr = m_pRenderDevice->CreateVertexBuffer((UINT)m_maxVertexCount * m_vertexSize, 
            D3DUSAGE_WRITEONLY,
            m_vertexFVF,
            D3DPOOL_SYSTEMMEM,
            &m_pVertexBuffer,
            0);
        BEATS_ASSERT(!FAILED(hr), _T("CreateVertexBuffer() - FAILED"));

        hr = m_pRenderDevice->CreateIndexBuffer((UINT)m_maxVertexCount / 4 * 6 * sizeof(uint32_t),
            D3DUSAGE_WRITEONLY,
            D3DFMT_INDEX32,
            D3DPOOL_SYSTEMMEM,
            &m_pIndexBuffer,
            0);
        BEATS_ASSERT(!FAILED(hr), _T("CreateIndexBuffer() - FAILED"));
        hr = m_pVertexBuffer->Lock(0, 0, (void**)(&m_pCurVertex), D3DLOCK_NOSYSLOCK);
        BEATS_ASSERT(hr == D3D_OK);
        hr = m_pIndexBuffer->Lock(0, 0, (void**)(&m_pCurIndex), D3DLOCK_NOSYSLOCK);
        BEATS_ASSERT(hr == D3D_OK);
    }
}

void CRenderCacheBase::DestroyBuffer()
{
    if (m_pVertexBuffer != NULL)
    {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = NULL;
    }
    if (m_pIndexBuffer != NULL)
    {
        m_pIndexBuffer->Release();
        m_pIndexBuffer = NULL;
    }

}
