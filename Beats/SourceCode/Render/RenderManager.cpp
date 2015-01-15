#include "stdafx.h"
#include "RenderManager.h"
#include "../ui/UIManager.h"
#include "RenderUnit.h"
#include "Camera/Camera.h"
#include "Particle/ParticleManager.h"
#include "RenderPublicDef.h"
#include "RenderCache/QuadCache.h"
#include "RenderCache/LineListCache.h"
#include "Shader/ShaderManager.h"

CRenderManager* CRenderManager::m_pInstance = NULL;

CRenderManager::CRenderManager()
: m_pCamera(new CCamera())
, m_pRenderDevice(NULL)
, m_bInit(false)
{
    m_unitsContainer.reserve(500);
    m_recycleUnitId.reserve(200);
    ZeroMemory(&m_d3dPresentParam, sizeof(m_d3dPresentParam));
}

CRenderManager::~CRenderManager()
{
    BEATS_SAFE_DELETE(m_pCamera);
    BEATS_SAFE_DELETE_VECTOR(m_unitsContainer);
    BEATS_SAFE_DELETE_VECTOR(m_caches);
    if (m_pRenderDevice != NULL)
    {
        m_pRenderDevice->Release();
    }
    CShaderManager::GetInstance()->Destroy();
}

void CRenderManager::Init(const HWND& hwnd, int width, int height, bool bWindow)
{
    IDirect3D9* pD3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    BEATS_ASSERT(pD3d9 != NULL, _T("Create Render D3D9 device failed!"));
    if (pD3d9 != NULL)
    {
        D3DCAPS9 caps;
        pD3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

        m_d3dPresentParam.BackBufferFormat           = D3DFMT_A8R8G8B8;
        m_d3dPresentParam.MultiSampleType            = D3DMULTISAMPLE_NONE;
        m_d3dPresentParam.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
        m_d3dPresentParam.hDeviceWindow              = hwnd;
        m_d3dPresentParam.Windowed                   = bWindow;
        m_d3dPresentParam.EnableAutoDepthStencil     = true; 
        m_d3dPresentParam.AutoDepthStencilFormat     = D3DFMT_D24S8;
        m_d3dPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
        m_d3dPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

        if (bWindow)
        {
            m_d3dPresentParam.BackBufferWidth          = 0;
            m_d3dPresentParam.BackBufferHeight         = 0;
            m_d3dPresentParam.BackBufferCount          = 0;
        }
        else
        {
            m_d3dPresentParam.BackBufferWidth          = width;
            m_d3dPresentParam.BackBufferHeight         = height;
            m_d3dPresentParam.BackBufferCount          = 1;
        }

        bool supportHardwareVextexProcess = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
        int vp = supportHardwareVextexProcess ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        HRESULT hr = pD3d9->CreateDevice(
                            D3DADAPTER_DEFAULT, // primary adapter
                            D3DDEVTYPE_HAL,         // device type
                            hwnd,               // window associated with device
                            vp,                 // vertex processing
                            &m_d3dPresentParam,             // present parameters
                            &m_pRenderDevice);            // return created device
        BEATS_ASSERT(hr == S_OK, _T("Create Render Device Failed!\nError%d\n%s\n%s"), hr, DXGetErrorString(hr), DXGetErrorDescription(hr));
        pD3d9->Release();
        if (hr == S_OK)
        {
            CShaderManager::GetInstance()->Initialize(m_pRenderDevice);
            CQuadCache* pQuadCache = new CQuadCache(1024, m_pRenderDevice, SVertex::VertexFormat, sizeof(SVertex));
            m_caches.push_back(pQuadCache);

            CLineListCache* pLineListCache = new CLineListCache(1024, m_pRenderDevice, SVertex2::VertexFormat, sizeof(SVertex2));
            m_caches.push_back(pLineListCache);
            m_bInit = true;
        }
    }
}

void CRenderManager::Update(float deltaMs)
{
    if (m_bInit)
    {
        if( m_pRenderDevice ) 
        {
            HRESULT hr = m_pRenderDevice->TestCooperativeLevel();
            switch (hr)
            {
            case D3D_OK:
                {
                    RenderAxises();

                    m_pRenderDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
                    BEATS_ASSERT(m_pCamera != NULL, _T("Render must be with a camera!"));
                    m_pRenderDevice->SetTransform(D3DTS_VIEW, &m_pCamera->GetViewMatrix());
                    m_pRenderDevice->SetTransform(D3DTS_PROJECTION, &m_pCamera->GetProjMatrix());
                    D3DXMATRIX worldMatrix;
                    D3DXMatrixIdentity(&worldMatrix);
                    m_pRenderDevice->SetTransform(D3DTS_WORLD, &worldMatrix);
                    m_pRenderDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
                    m_pRenderDevice->SetTextureStageState( 1 , D3DTSS_COLOROP, D3DTOP_DISABLE);

                    /////////////////////////Start Render/////////////////////////////////
                    m_pRenderDevice->BeginScene();

                    BEATS_PERFORMDETECT_START(NBDT::ePT_RenderCache);
                    for (int i = 0; i < eRCT_Count; ++i)
                    {
                        m_caches[i]->Render();
                    }
                    BEATS_PERFORMDETECT_STOP(NBDT::ePT_RenderCache);

                    BEATS_PERFORMDETECT_START(NBDT::ePT_RenderParticle);
                    CParticleManager::GetInstance()->Update(deltaMs);
                    BEATS_PERFORMDETECT_STOP(NBDT::ePT_RenderParticle);

                    BEATS_PERFORMDETECT_START(NBDT::ePT_UpdateUI);
                    CUIManager::GetInstance()->Update(deltaMs);
                    BEATS_PERFORMDETECT_STOP(NBDT::ePT_UpdateUI);

                    m_pRenderDevice->EndScene();

                    m_pRenderDevice->Present(0, 0, 0, 0);
                }
                break;
            case D3DERR_DEVICELOST:
                {
                    //Do nothing but pause all rendering logic.
                }
                break;
            case D3DERR_DEVICENOTRESET:
                {
                    // Release render targets, depth stencil surfaces, additional swap chains, state blocks, 
                    // and D3DPOOL_DEFAULT resources associated with the device for once.

                    hr = m_pRenderDevice->Reset(&m_d3dPresentParam);
                    BEATS_ASSERT(hr == D3D_OK, _T("Reset d3d device failed! check if some resource are not released!\nError%d\n%s\n%s"),
                                                hr, DXGetErrorString(hr), DXGetErrorDescription(hr));
                }
            }
        }
    }
}

IDirect3DDevice9* CRenderManager::GetDevice() const
{
    return m_pRenderDevice;
}

void CRenderManager::Register( CRenderUnit* pUnit )
{
    m_unitsContainer.push_back(pUnit);
}

CRenderUnit* CRenderManager::AcquireUnit(CBaseObject* pOwner)
{
    CRenderUnit* pUnit = NULL;
    if (m_recycleUnitId.size() == 0)
    {
        pUnit = new CRenderUnit(pOwner);
    }
    else
    {
        pUnit = m_unitsContainer[*(m_recycleUnitId.end())];
        m_recycleUnitId.pop_back();
    }
    return pUnit;
}

void CRenderManager::SetCamera( CCamera* pCamera )
{
    m_pCamera = pCamera;
}

void CRenderManager::SendVertexToCache( ERenderCacheType cacheType, const void* pVertex, const Function<void(void)>* pFunction /*= NULL*/ )
{
    m_caches[cacheType]->AddToCache(pVertex, pFunction);
}

void CRenderManager::SendQuadToCache( const CVector3& center, float width, float height, long color, const Function<void(void)>* pFunction /*= NULL*/)
{
    //////////////////////////////////////////////////////////////////////////
    ///Generate quad, Quad index:
    /*
    0¡ª1
    |  |
    2¡ª3
    */
    for (uint32_t i = 0; i < 4; ++i)
    {
        SVertex vertex;
        vertex.m_uv.x = (float)(i % 2);
        vertex.m_uv.y = i > 1 ? 1.f : 0.f;

        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        vertex.m_position.x = center.x + ((i % 2 == 0) ? -halfWidth : halfWidth);
        vertex.m_position.y = center.y + ((i > 1) ? -halfHeight : halfHeight);
        vertex.m_position.z = center.z;
        vertex.m_color = color;

        CRenderManager::GetInstance()->SendVertexToCache(eRCT_Quad, &vertex, pFunction);
    }
}

void CRenderManager::RenderAxises()
{
    SVertex2 start(0,0,0,0xffff0000);
    SendVertexToCache(eRCT_Line, &start, NULL);
    SVertex2 YEnd(0,100,0,0xffff0000);
    SendVertexToCache(eRCT_Line, &YEnd, NULL);

    start.m_color = 0xff00ff00;
    SendVertexToCache(eRCT_Line, &start, NULL);
    SVertex2 ZEnd(0,0,100,0xff00ff00);
    SendVertexToCache(eRCT_Line, &ZEnd, NULL);

    start.m_color = 0xff0000ff;
    SendVertexToCache(eRCT_Line, &start, NULL);
    SVertex2 XEnd(100,0,0,0xff0000ff);
    SendVertexToCache(eRCT_Line, &XEnd, NULL);

}