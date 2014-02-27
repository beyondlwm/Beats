#include "stdafx.h"
#include "ComponentGraphics_DX.h"
#include "../../Components/Component/ComponentEditorProxy.h"
#include "../../Components/DependencyDescription.h"
#include "../../Components/DependencyDescriptionLine.h"

LPDIRECT3DTEXTURE9* CComponentGraphic_DX::m_pTextures = NULL;
const long SVertex::VertexFormat = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
const long SVertex2::VertexFormat = D3DFVF_XYZ | D3DFVF_DIFFUSE;

CComponentGraphic_DX::CComponentGraphic_DX(IDirect3DDevice9* pRenderDevice, LPD3DXFONT pFont)
: m_pRenderDevice(pRenderDevice)
, m_pFont(pFont)
{

}

CComponentGraphic_DX::~CComponentGraphic_DX()
{

}

void CComponentGraphic_DX::RecreateTextures( IDirect3DDevice9* pRenderDevice, const TCHAR* pPath )
{
    if (m_pTextures != NULL)
    {
        ReleaseTextures();
    }
    if (m_pTextures == NULL)
    {
        m_pTextures = new LPDIRECT3DTEXTURE9[eCT_Count];
        bool bLoadSuceess = false;
        TString szPath(pPath);
        bLoadSuceess = D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\RectBG.bmp")).c_str(), &m_pTextures[eCT_RectBG]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\ConnectRect.bmp")).c_str(), &m_pTextures[eCT_ConnectRect]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\SelectedRect.bmp")).c_str(), &m_pTextures[eCT_SelectedRectBG]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\NormalLine.bmp")).c_str(), &m_pTextures[eCT_NormalLine]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\SelectedLine.bmp")).c_str(), &m_pTextures[eCT_SelectedLine]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\NormalArrow.bmp")).c_str(), &m_pTextures[eCT_NormalArrow]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\SelectedArrow.bmp")).c_str(), &m_pTextures[eCT_SelectedArrow]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\ConnectedDependency.bmp")).c_str(), &m_pTextures[eCT_ConnectedDependency]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\ConnectedDependencyList.bmp")).c_str(), &m_pTextures[eCT_ConnectedDependencyList]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\WeakDependency.bmp")).c_str(), &m_pTextures[eCT_WeakDependency]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\WeakDependencyList.bmp")).c_str(), &m_pTextures[eCT_WeakDependencyList]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\StrongDependency.bmp")).c_str(), &m_pTextures[eCT_StrongDependency]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        szPath.assign(pPath);
        bLoadSuceess = bLoadSuceess &&D3DXCreateTextureFromFile(pRenderDevice, szPath.append(_T("\\Texture\\StrongDependencyList.bmp")).c_str(), &m_pTextures[eCT_StrongDependencyList]) == D3D_OK;
        BEATS_ASSERT(bLoadSuceess, _T("Create Component Texture Failed!"));
        if (bLoadSuceess == false)
        {
            BEATS_SAFE_DELETE_ARRAY(m_pTextures);
        }
    }
}

void CComponentGraphic_DX::ReleaseTextures()
{
    for (size_t i = 0; i < eCT_Count; ++i)
    {
        m_pTextures[i]->Release();
    }
    BEATS_SAFE_DELETE_ARRAY(m_pTextures);
}

CComponentGraphic* CComponentGraphic_DX::Clone()
{
    return new CComponentGraphic_DX(m_pRenderDevice, m_pFont);
}

void CComponentGraphic_DX::DrawHead(float cellSize)
{
    m_pRenderDevice->SetTexture(0, m_pTextures[eCT_RectBG]);

    SVertex headPoints[4];
    headPoints[0].m_position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Left top
    headPoints[0].m_position.y = m_gridPosY * cellSize;    
    headPoints[0].m_position.z = (float)m_gridPosZ;    
    headPoints[0].m_color = 0xffffffff;
    headPoints[0].m_uv.x = 0;
    headPoints[0].m_uv.y = 0;

    headPoints[1].m_position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Left down
    headPoints[1].m_position.y = (m_gridPosY - HEADER_HEIGHT) * cellSize;
    headPoints[1].m_position.z = (float)m_gridPosZ;    
    headPoints[1].m_color = 0xffffffff;
    headPoints[1].m_uv.x = 0;
    headPoints[1].m_uv.y = 1;

    headPoints[2].m_position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right top
    headPoints[2].m_position.y = m_gridPosY * cellSize ;
    headPoints[2].m_position.z = (float)m_gridPosZ;    
    headPoints[2].m_color = 0xffffffff;
    headPoints[2].m_uv.x = 1;
    headPoints[2].m_uv.y = 0;

    headPoints[3].m_position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right down
    headPoints[3].m_position.y = (m_gridPosY - HEADER_HEIGHT) * cellSize;
    headPoints[3].m_position.z = (float)m_gridPosZ;    
    headPoints[3].m_color = 0xffffffff;
    headPoints[3].m_uv.x = 1;
    headPoints[3].m_uv.y = 1;
    m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, headPoints, sizeof(SVertex));

    // 2. Draw title text.
    IDirect3DSurface9* pSurface = NULL;
    m_pRenderDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
    D3DSURFACE_DESC surfaceDesc;
    pSurface->GetDesc(&surfaceDesc);
    pSurface->Release();

    D3DVIEWPORT9 viewPort;
    m_pRenderDevice->GetViewport(&viewPort);    
    float dYRate = (float)surfaceDesc.Height / viewPort.Height;
    float dXRate = (float)surfaceDesc.Width / viewPort.Width;
    D3DMATRIX viewMatrix;
    m_pRenderDevice->GetTransform(D3DTS_VIEW, &viewMatrix);
    int x = (int)(((m_gridPosX + CONNECTION_WIDTH) * cellSize + viewMatrix._41) / dXRate + viewPort.Width * 0.5f);
    int y = (int)((-m_gridPosY * cellSize - viewMatrix._42) / dYRate + viewPort.Height * 0.5f);
    RECT rc;
    rc.left = x;
    rc.right = rc.left + (long)((MIN_WIDTH * cellSize) / dXRate);
    rc. top = y;
    rc.bottom = rc. top + (long)((HEADER_HEIGHT * cellSize) / dYRate);
    m_pFont->DrawText(NULL, m_pOwner->GetDisplayName().c_str(), -1,  &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0xffffffff);

    // 3. Draw Connect rect.
    m_pRenderDevice->SetTexture(0, m_pTextures[eCT_ConnectRect]);
    headPoints[0].m_position.x = m_gridPosX * cellSize;    // Left top
    headPoints[0].m_position.y = m_gridPosY * cellSize;    
    headPoints[0].m_position.z = (float)m_gridPosZ;    
    headPoints[0].m_color = 0xffffffff;
    headPoints[0].m_uv.x = 0;
    headPoints[0].m_uv.y = 0;

    headPoints[1].m_position.x = m_gridPosX * cellSize;    // Left down
    headPoints[1].m_position.y = (m_gridPosY - HEADER_HEIGHT) * cellSize;
    headPoints[1].m_position.z = (float)m_gridPosZ;    
    headPoints[1].m_color = 0xffffffff;
    headPoints[0].m_uv.x = 0;
    headPoints[0].m_uv.y = 1;

    headPoints[2].m_position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right top
    headPoints[2].m_position.y = m_gridPosY * cellSize ;
    headPoints[2].m_position.z = (float)m_gridPosZ;    
    headPoints[2].m_color = 0xffffffff;
    headPoints[0].m_uv.x = 1;
    headPoints[0].m_uv.y = 0;

    headPoints[3].m_position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right down
    headPoints[3].m_position.y = (m_gridPosY - HEADER_HEIGHT) * cellSize;
    headPoints[3].m_position.z = (float)m_gridPosZ;    
    headPoints[3].m_color = 0xffffffff;
    headPoints[0].m_uv.x = 1;
    headPoints[0].m_uv.y = 1;

    m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, headPoints, sizeof(SVertex));
}

void CComponentGraphic_DX::DrawDependencies( float cellSize )
{
    size_t uDependencyCount = m_pOwner->GetDependencies().size();
    for (int i = 0; i < (int)uDependencyCount; ++i)
    {
        // 1. Draw background.
        m_pRenderDevice->SetTexture(0, m_pTextures[eCT_RectBG]);
        SVertex dependencyPoints[4];
        dependencyPoints[0].m_position.x = m_gridPosX * cellSize;    // Left top
        dependencyPoints[0].m_position.y = (m_gridPosY - HEADER_HEIGHT - i * DEPENDENCY_HEIGHT) * cellSize;    
        dependencyPoints[0].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[0].m_color = 0xffffffff;
        dependencyPoints[0].m_uv.x = 0;
        dependencyPoints[0].m_uv.y = 0;

        dependencyPoints[1].m_position.x = m_gridPosX * cellSize;    // Left down
        dependencyPoints[1].m_position.y = (m_gridPosY - HEADER_HEIGHT - (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoints[1].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[1].m_color = 0xffffffff;
        dependencyPoints[1].m_uv.x = 0;
        dependencyPoints[1].m_uv.y = 1;

        dependencyPoints[2].m_position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right top
        dependencyPoints[2].m_position.y = (m_gridPosY - HEADER_HEIGHT - i * DEPENDENCY_HEIGHT) * cellSize ;
        dependencyPoints[2].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[2].m_color = 0xffffffff;
        dependencyPoints[2].m_uv.x = 1;
        dependencyPoints[2].m_uv.y = 0;

        dependencyPoints[3].m_position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right down
        dependencyPoints[3].m_position.y = (m_gridPosY - HEADER_HEIGHT - (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoints[3].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[3].m_color = 0xffffffff;
        dependencyPoints[3].m_uv.x = 1;
        dependencyPoints[3].m_uv.y = 1;

        m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, dependencyPoints, sizeof(SVertex));

        // 2. Draw dependency name.
        IDirect3DSurface9* pSurface = NULL;
        m_pRenderDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
        D3DSURFACE_DESC surfaceDesc;
        pSurface->GetDesc(&surfaceDesc);
        pSurface->Release();
        D3DVIEWPORT9 viewPort;
        m_pRenderDevice->GetViewport(&viewPort);    
        float dYRate = (float)surfaceDesc.Height / viewPort.Height;
        float dXRate = (float)surfaceDesc.Width / viewPort.Width;
        D3DMATRIX viewMatrix;
        m_pRenderDevice->GetTransform(D3DTS_VIEW, &viewMatrix);
        int x = (int)((m_gridPosX * cellSize + viewMatrix._41) / dXRate + viewPort.Width * 0.5f);
        int y = (int)((-m_gridPosY * cellSize - viewMatrix._42) / dYRate + viewPort.Height * 0.5f);
        RECT rc2;
        rc2.left = x;
        rc2.right = rc2.left + (long)((MIN_WIDTH * cellSize) / dXRate);
        rc2.top = y + (int)((i * DEPENDENCY_HEIGHT + HEADER_HEIGHT) * cellSize) + HEADER_HEIGHT;
        rc2.bottom = rc2.top + (long)((DEPENDENCY_HEIGHT * cellSize) / dYRate);
        m_pFont->DrawText(NULL, m_pOwner->GetDependency(i)->GetDisplayName(), -1,  &rc2, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0xffffffff);

        // 3. Draw Connect rect.
        CDependencyDescription* pDescription = m_pOwner->GetDependency(i);
        BEATS_ASSERT(pDescription != NULL);
        bool bConnected = pDescription->GetDependencyLineCount() > 0;
        EComponentTexture textureType = eCT_Count;
        EDependencyType descriptionType = pDescription->GetType();
        bool bIsList = pDescription->IsListType();
        if (bConnected)
        {
            textureType = bIsList ? eCT_ConnectedDependencyList : eCT_ConnectedDependency;
        }
        else
        {
            if (bIsList)
            {
                textureType = descriptionType == eDT_Strong ? eCT_StrongDependencyList : eCT_WeakDependencyList;
            }
            else
            {
                textureType = descriptionType == eDT_Strong ? eCT_StrongDependency : eCT_WeakDependency;
            }
        }
        BEATS_ASSERT(textureType != eCT_Count);
        m_pRenderDevice->SetTexture(0, m_pTextures[textureType]);

        dependencyPoints[0].m_position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left top
        dependencyPoints[0].m_position.y = (m_gridPosY - HEADER_HEIGHT - i * DEPENDENCY_HEIGHT) * cellSize;    
        dependencyPoints[0].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[0].m_color = 0xffffffff;
        dependencyPoints[0].m_uv.x = 0;
        dependencyPoints[0].m_uv.y = 0;

        dependencyPoints[1].m_position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left down
        dependencyPoints[1].m_position.y = (m_gridPosY - HEADER_HEIGHT - (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoints[1].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[1].m_color = 0xffffffff;
        dependencyPoints[1].m_uv.x = 0;
        dependencyPoints[1].m_uv.y = 1;

        dependencyPoints[2].m_position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right top
        dependencyPoints[2].m_position.y = (m_gridPosY - HEADER_HEIGHT - i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoints[2].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[2].m_color = 0xffffffff;
        dependencyPoints[2].m_uv.x = 1;
        dependencyPoints[2].m_uv.y = 0;

        dependencyPoints[3].m_position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right down
        dependencyPoints[3].m_position.y = (m_gridPosY - HEADER_HEIGHT - (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoints[3].m_position.z = (float)m_gridPosZ;    
        dependencyPoints[3].m_color = 0xffffffff;
        dependencyPoints[3].m_uv.x = 1;
        dependencyPoints[3].m_uv.y = 1;
        m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, dependencyPoints, sizeof(SVertex));

        // 4. Draw the line.
        DrawDependencyLine(cellSize, m_pOwner->GetDependency(i));
    }
}

void CComponentGraphic_DX::DrawDependencyLine( float cellSize, const CDependencyDescription* pDependency )
{
    BEATS_ASSERT(pDependency != NULL);
    if (pDependency->IsVisible())
    {
        size_t uDependencyLineCount = pDependency->GetDependencyLineCount();
        for (size_t i = 0; i < uDependencyLineCount; ++i)
        {
            CDependencyDescriptionLine* pDependencyLine = pDependency->GetDependencyLine(i);
            const SVertex* pData = pDependencyLine->GetRectArray();
            m_pRenderDevice->SetTexture(0, m_pTextures[pDependencyLine->IsSelected() ? eCT_SelectedLine : eCT_NormalLine]);
            m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pData, sizeof(SVertex));
            const SVertex* pArrowData = pDependencyLine->GetArrowRectArray();
            m_pRenderDevice->SetTexture(0, m_pTextures[pDependencyLine->IsSelected() ? eCT_SelectedArrow : eCT_NormalArrow]);
            m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pArrowData, sizeof(SVertex));

            //Render Index Number for dependency list.
            if (pDependency->IsListType())
            {
                D3DXVECTOR3 vecWorldPos((pData[0].m_position.x - pData[1].m_position.x) * 0.3f + pData[1].m_position.x,
                    (pData[0].m_position.y - pData[1].m_position.y) * 0.3f + pData[1].m_position.y + cellSize*0.3f/*(offset)*/,
                    0);
                D3DXVECTOR3 vecScreenPos(0.f,0.f,0.f);

                D3DXMATRIX viewMatrix;
                m_pRenderDevice->GetTransform(D3DTS_VIEW, &viewMatrix);
                D3DXMATRIX mtxProjection;
                m_pRenderDevice->GetTransform(D3DTS_PROJECTION, &mtxProjection);
                D3DVIEWPORT9 viewPort;
                m_pRenderDevice->GetViewport(&viewPort);    

                D3DXVec3Project(&vecScreenPos, &vecWorldPos, &viewPort, &mtxProjection, &viewMatrix, NULL);
                RECT rc2;
                rc2.left = (long)vecScreenPos.x;
                rc2.right = rc2.left + 20;
                rc2.top = (long)vecScreenPos.y;
                rc2.bottom = rc2.top + 20;
                TCHAR szIndex[8];
                _stprintf(szIndex, _T("%d"), pDependencyLine->GetIndex());

                m_pFont->DrawText(NULL, szIndex, -1,  &rc2, DT_LEFT | DT_TOP | DT_SINGLELINE, 0xffffff00);
            }
        }
    }
}

void CComponentGraphic_DX::DrawSelectedRect(float cellSize)
{
    m_pRenderDevice->SetTexture(0, CComponentGraphic_DX::m_pTextures[eCT_SelectedRectBG]);
    float fWidth = GetWidth() * cellSize;
    float fHeight = GetHeight() * cellSize;
    int x, y;
    GetPosition(&x, &y);
    SVertex rect[4];
    // left top
    rect[0].m_position.x = x * cellSize;
    rect[0].m_position.y = y * cellSize;
    rect[0].m_position.z = (float)m_gridPosZ;
    rect[0].m_color = 0xffffffff;
    rect[0].m_uv.x = 0;
    rect[0].m_uv.y = 0;
    // left down
    rect[1].m_position.x = x * cellSize;
    rect[1].m_position.y = y * cellSize - fHeight;
    rect[1].m_position.z = (float)m_gridPosZ;
    rect[1].m_color = 0xffffffff;
    rect[1].m_uv.x = 0;
    rect[1].m_uv.y = 1;
    // right top
    rect[2].m_position.x = x * cellSize + fWidth;
    rect[2].m_position.y = y * cellSize;
    rect[2].m_position.z = (float)m_gridPosZ;
    rect[2].m_color = 0xffffffff;
    rect[2].m_uv.x = 1;
    rect[2].m_uv.y = 0;
    // right down
    rect[3].m_position.x = x * cellSize + fWidth;
    rect[3].m_position.y = y * cellSize - fHeight;
    rect[3].m_position.z = (float)m_gridPosZ;
    rect[3].m_color = 0xffffffff;
    rect[3].m_uv.x = 1;
    rect[3].m_uv.y = 1;
    m_pRenderDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, rect, sizeof(SVertex));
}
