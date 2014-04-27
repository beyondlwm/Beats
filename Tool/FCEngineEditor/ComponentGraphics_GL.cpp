#include "stdafx.h"
#include "ComponentGraphics_GL.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Dependency/DependencyDescriptionLine.h"
#include "Render/Texture.h"
#include "render/RenderManager.h"
#include "render/Renderer.h"
#include "GUI/Font/FontManager.h"
#include "Resource/ResourceManager.h"
#include "Render/SpriteFrameBatchManager.h"


CComponentGraphic_GL::CComponentGraphic_GL()
    : m_pFont(NULL)
{
    // Base class is dx implemented.
    m_gridPosZ *= -1;
}

CComponentGraphic_GL::~CComponentGraphic_GL()
{

}

void CComponentGraphic_GL::CreateTextures()
{
    if (m_pFont == NULL)
    {
        m_pFont = FCGUI::FontManager::GetInstance()->GetFace(_T("ComponentGraphicFont"));
        if (m_pFont == NULL)
        {
            m_pFont = FCGUI::FontManager::GetInstance()->CreateFace(_T("ComponentGraphicFont"), _T("msyh.ttf"), 16, 0);
        }
    }
    BEATS_ASSERT(m_pFont != NULL);
    if (m_pTextures[eCT_RectBG].Get() == NULL)
    {
        CResourceManager* pResourceMgr = CResourceManager::GetInstance();
        m_pTextures[eCT_RectBG] = pResourceMgr->GetResource<CTexture>(_T("Component/RectBG.jpg"), false);
        bool bLoadSuccess = m_pTextures[eCT_RectBG].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_ConnectRect] = pResourceMgr->GetResource<CTexture>(_T("Component/ConnectRect.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_ConnectRect].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_SelectedRectBG] = pResourceMgr->GetResource<CTexture>(_T("Component/SelectedRect.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_SelectedRectBG].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_NormalLine] = pResourceMgr->GetResource<CTexture>(_T("Component/NormalLine.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_NormalLine].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_SelectedLine] = pResourceMgr->GetResource<CTexture>(_T("Component/SelectedLine.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_SelectedLine].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_NormalArrow] = pResourceMgr->GetResource<CTexture>(_T("Component/NormalArrow.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_NormalArrow].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_SelectedArrow] = pResourceMgr->GetResource<CTexture>(_T("Component/SelectedArrow.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_SelectedArrow].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_ConnectedDependency] = pResourceMgr->GetResource<CTexture>(_T("Component/ConnectedDependency.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_ConnectedDependency].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_ConnectedDependencyList] = pResourceMgr->GetResource<CTexture>(_T("Component/ConnectedDependencyList.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_ConnectedDependencyList].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_WeakDependency] = pResourceMgr->GetResource<CTexture>(_T("Component/WeakDependency.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_WeakDependency].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_WeakDependencyList] = pResourceMgr->GetResource<CTexture>(_T("Component/WeakDependencyList.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_WeakDependencyList].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_StrongDependency] = pResourceMgr->GetResource<CTexture>(_T("Component/StrongDependency.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_StrongDependency].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));

        m_pTextures[eCT_StrongDependencyList] = pResourceMgr->GetResource<CTexture>(_T("Component/StrongDependencyList.jpg"), false);
        bLoadSuccess = m_pTextures[eCT_StrongDependencyList].Get() != NULL;
        BEATS_ASSERT(bLoadSuccess, _T("Create Component Texture Failed!"));
    }
}

void CComponentGraphic_GL::GetDependencyPosition(size_t uDependencyIndex, int* pOutX, int* pOutY)
{
    BEATS_ASSERT(m_pOwner->GetDependency(uDependencyIndex) != NULL);
    *pOutX = m_gridPosX + MIN_WIDTH;
    *pOutY = m_gridPosY + HEADER_HEIGHT + (int)(uDependencyIndex + 1) * DEPENDENCY_HEIGHT;
}

CComponentGraphic* CComponentGraphic_GL::Clone()
{
    return new CComponentGraphic_GL();
}

void CComponentGraphic_GL::DrawHead(float cellSize)
{
    CreateTextures();
    CSpriteFrameBatchManager * pSpriteBatchMgr = CSpriteFrameBatchManager::GetInstance();
    CQuadPT head;
    head.tl.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left top
    head.tl.position.y = m_gridPosY * cellSize;
    head.tl.position.z = (float)m_gridPosZ;
    head.tl.tex.u = 0;
    head.tl.tex.v = 0;

    head.bl.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;// Left down
    head.bl.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.bl.position.z = (float)m_gridPosZ;
    head.bl.tex.u = 0;
    head.bl.tex.v = 1;

    head.tr.position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right top
    head.tr.position.y = m_gridPosY * cellSize;
    head.tr.position.z = (float)m_gridPosZ;
    head.tr.tex.u = 1;
    head.tr.tex.v = 0;

    head.br.position.x = (m_gridPosX + CONNECTION_WIDTH + MIN_WIDTH) * cellSize;    // Right down
    head.br.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    head.br.position.z = (float)m_gridPosZ;
    head.br.tex.u = 1;
    head.br.tex.v = 1;
    pSpriteBatchMgr->AddQuad(head, m_pTextures[eCT_RectBG]);

    // 2. Draw title text.
    FCGUI::FontManager::GetInstance()->RenderText(m_pOwner->GetDisplayName(), head.tl.position.x, head.tl.position.y, _T("ComponentGraphicFont"));

    // 3. Draw Connect rect.
    CQuadPT connectRect;

    connectRect.tl.position.x = m_gridPosX * cellSize;    // Left top
    connectRect.tl.position.y = m_gridPosY * cellSize;
    connectRect.tl.position.z = (float)m_gridPosZ;
    connectRect.tl.tex.u = 0;
    connectRect.tl.tex.v = 0;

    connectRect.bl.position.x = m_gridPosX * cellSize;    // Left down
    connectRect.bl.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.bl.position.z = (float)m_gridPosZ;
    connectRect.bl.tex.u = 0;
    connectRect.bl.tex.v = 1;

    connectRect.tr.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right top
    connectRect.tr.position.y = m_gridPosY * cellSize;
    connectRect.tr.position.z = (float)m_gridPosZ;    
    connectRect.tr.tex.u = 1;
    connectRect.tr.tex.v = 0;

    connectRect.br.position.x = (m_gridPosX + CONNECTION_WIDTH) * cellSize;    // Right down
    connectRect.br.position.y = (m_gridPosY + HEADER_HEIGHT) * cellSize;
    connectRect.br.position.z = (float)m_gridPosZ;    
    connectRect.br.tex.u = 1;
    connectRect.br.tex.v = 1;

    pSpriteBatchMgr->AddQuad(connectRect, m_pTextures[eCT_ConnectRect]);
}

void CComponentGraphic_GL::DrawDependencies( float cellSize )
{
    CreateTextures();
    CSpriteFrameBatchManager* pSpriteBatchMgr = CSpriteFrameBatchManager::GetInstance();
    size_t uDependencyCount = m_pOwner->GetDependencies().size();
    for (int i = 0; i < (int)uDependencyCount; ++i)
    {
        // 1. Draw background.
        CQuadPT dependencyPoint;
        dependencyPoint.tl.position.x = m_gridPosX * cellSize;    // Left top
        dependencyPoint.tl.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tl.position.z = (float)m_gridPosZ;
        dependencyPoint.tl.tex.u = 0;
        dependencyPoint.tl.tex.v = 0;

        dependencyPoint.bl.position.x = m_gridPosX * cellSize;    // Left down
        dependencyPoint.bl.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.bl.position.z = (float)m_gridPosZ;
        dependencyPoint.bl.tex.u = 0;
        dependencyPoint.bl.tex.v = 1;

        dependencyPoint.tr.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right top
        dependencyPoint.tr.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize ;
        dependencyPoint.tr.position.z = (float)m_gridPosZ;
        dependencyPoint.tr.tex.u = 1;
        dependencyPoint.tr.tex.v = 0;

        dependencyPoint.br.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Right down
        dependencyPoint.br.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.br.position.z = (float)m_gridPosZ;    
        dependencyPoint.br.tex.u = 1;
        dependencyPoint.br.tex.v = 1;

        pSpriteBatchMgr->AddQuad(dependencyPoint, m_pTextures[eCT_RectBG]);

        // 2. TODO:Draw dependency name.

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

        dependencyPoint.tl.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left top
        dependencyPoint.tl.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tl.position.z = (float)m_gridPosZ;
        dependencyPoint.tl.tex.u = 0;
        dependencyPoint.tl.tex.v = 0;

        dependencyPoint.bl.position.x = (m_gridPosX + MIN_WIDTH) * cellSize;    // Left down
        dependencyPoint.bl.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.bl.position.z = (float)m_gridPosZ;
        dependencyPoint.bl.tex.u = 0;
        dependencyPoint.bl.tex.v = 1;

        dependencyPoint.tr.position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right top
        dependencyPoint.tr.position.y = (m_gridPosY + HEADER_HEIGHT + i * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.tr.position.z = (float)m_gridPosZ;
        dependencyPoint.tr.tex.u = 1;
        dependencyPoint.tr.tex.v = 0;

        dependencyPoint.br.position.x = (m_gridPosX + MIN_WIDTH + CONNECTION_WIDTH) * cellSize;    // Right down
        dependencyPoint.br.position.y = (m_gridPosY + HEADER_HEIGHT + (i + 1) * DEPENDENCY_HEIGHT) * cellSize;
        dependencyPoint.br.position.z = (float)m_gridPosZ;
        dependencyPoint.br.tex.u = 1;
        dependencyPoint.br.tex.v = 1;

        pSpriteBatchMgr->AddQuad(dependencyPoint, m_pTextures[textureType]);

        // 4. Draw the line.
        DrawDependencyLine(cellSize, m_pOwner->GetDependency(i));
    }
}

void CComponentGraphic_GL::DrawDependencyLine( float /*cellSize*/, const CDependencyDescription* pDependency )
{
    CreateTextures();
    CSpriteFrameBatchManager* pSpriteBatchMgr = CSpriteFrameBatchManager::GetInstance();

    BEATS_ASSERT(pDependency != NULL);
    if (pDependency->IsVisible())
    {
        size_t uDependencyLineCount = pDependency->GetDependencyLineCount();
        for (size_t i = 0; i < uDependencyLineCount; ++i)
        {
            CDependencyDescriptionLine* pDependencyLine = pDependency->GetDependencyLine(i);
            const SVertex* pData = pDependencyLine->GetRectArray();
            static const size_t SVERTEX_SIZE = 24;
            CSerializer serializer(SVERTEX_SIZE * 4, (void*)pData);
            DWORD tmpColor = 0;
            CQuadPT dependencyLine;
            serializer >> dependencyLine.br.position.x;
            serializer >> dependencyLine.br.position.y;
            serializer >> dependencyLine.br.position.z;
            serializer >> tmpColor;
            serializer >> dependencyLine.br.tex.u;
            serializer >> dependencyLine.br.tex.v;

            serializer >> dependencyLine.bl.position.x;
            serializer >> dependencyLine.bl.position.y;
            serializer >> dependencyLine.bl.position.z;
            serializer >> tmpColor;
            serializer >> dependencyLine.bl.tex.u;
            serializer >> dependencyLine.bl.tex.v;

            serializer >> dependencyLine.tr.position.x;
            serializer >> dependencyLine.tr.position.y;
            serializer >> dependencyLine.tr.position.z;
            serializer >> tmpColor;
            serializer >> dependencyLine.tr.tex.u;
            serializer >> dependencyLine.tr.tex.v;

            serializer >> dependencyLine.tl.position.x;
            serializer >> dependencyLine.tl.position.y;
            serializer >> dependencyLine.tl.position.z;
            serializer >> tmpColor;
            serializer >> dependencyLine.tl.tex.u;
            serializer >> dependencyLine.tl.tex.v;


            pSpriteBatchMgr->AddQuad(dependencyLine, m_pTextures[pDependencyLine->IsSelected() ? eCT_SelectedLine : eCT_NormalLine]);

            const SVertex* pArrowData = pDependencyLine->GetArrowRectArray();
            CSerializer serializerArrow(SVERTEX_SIZE * 4, (void*)pArrowData);
            CQuadPT arrow;
            serializerArrow >> arrow.br.position.x;
            serializerArrow >> arrow.br.position.y;
            serializerArrow >> arrow.br.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.br.tex.u;
            serializerArrow >> arrow.br.tex.v;

            serializerArrow >> arrow.bl.position.x;
            serializerArrow >> arrow.bl.position.y;
            serializerArrow >> arrow.bl.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.bl.tex.u;
            serializerArrow >> arrow.bl.tex.v;

            serializerArrow >> arrow.tr.position.x;
            serializerArrow >> arrow.tr.position.y;
            serializerArrow >> arrow.tr.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tr.tex.u;
            serializerArrow >> arrow.tr.tex.v;

            serializerArrow >> arrow.tl.position.x;
            serializerArrow >> arrow.tl.position.y;
            serializerArrow >> arrow.tl.position.z;
            serializerArrow >> tmpColor;
            serializerArrow >> arrow.tl.tex.u;
            serializerArrow >> arrow.tl.tex.v;

            pSpriteBatchMgr->AddQuad(arrow, m_pTextures[pDependencyLine->IsSelected() ? eCT_SelectedArrow : eCT_NormalArrow]);

            //Render index number for dependency list.
            if (pDependency->IsListType())
            {
                kmVec3 deltaDirection;
                kmVec3Subtract(&deltaDirection, &dependencyLine.tr.position, &dependencyLine.tl.position);
                float fXPos = (dependencyLine.tl.position.x + deltaDirection.x * 0.15f);
                static const float fHardCodeOffset = 32;
                float fYPos = (dependencyLine.tl.position.y + deltaDirection.y * 0.15f - fHardCodeOffset);

                TCHAR szIndex[8];
                _stprintf(szIndex, _T("%d"), pDependencyLine->GetIndex());
                FCGUI::FontManager::GetInstance()->RenderText(szIndex, fXPos, fYPos, _T("ComponentGraphicFont"));
            }
        }
    }
}

void CComponentGraphic_GL::DrawSelectedRect(float fCellSize)
{
    CreateTextures();
    float fWidth = GetWidth() * fCellSize;
    float fHeight = GetHeight() * fCellSize;
    int x, y;
    GetPosition(&x, &y);
    CQuadPT rect;
    // left top
    rect.tl.position.x = x * fCellSize;
    rect.tl.position.y = y * fCellSize;
    rect.tl.position.z = (float)m_gridPosZ;
    rect.tl.tex.u = 0;
    rect.tl.tex.v = 0;
    // left down
    rect.bl.position.x = x * fCellSize;
    rect.bl.position.y = y * fCellSize + fHeight;
    rect.bl.position.z = (float)m_gridPosZ;
    rect.bl.tex.u = 0;
    rect.bl.tex.v = 1;
    // right top
    rect.tr.position.x = x * fCellSize + fWidth;
    rect.tr.position.y = y * fCellSize;
    rect.tr.position.z = (float)m_gridPosZ;
    rect.tr.tex.u = 1;
    rect.tr.tex.v = 0;
    // right down
    rect.br.position.x = x * fCellSize + fWidth;
    rect.br.position.y = y * fCellSize + fHeight;
    rect.br.position.z = (float)m_gridPosZ;
    rect.br.tex.u = 1;
    rect.br.tex.v = 1;
    CSpriteFrameBatchManager::GetInstance()->AddQuad(rect, m_pTextures[eCT_SelectedRectBG]);
}

EComponentAeraRectType CComponentGraphic_GL::HitTestForAreaType( int x, int y, void** pReturnData)
{
    EComponentAeraRectType result = eCART_Invalid;
    if (x >= m_gridPosX && x < m_gridPosX + (int)m_width && y > m_gridPosY && y <= m_gridPosY + (int)m_height)
    {
        result = eCART_Body;
        if (y <= m_gridPosY + HEADER_HEIGHT && x < m_gridPosX + CONNECTION_WIDTH)
        {
            result = eCART_Connection;
        }
        else if (x >= m_gridPosX + MIN_WIDTH && y > m_gridPosY + HEADER_HEIGHT)
        {
            result = eCART_Dependency;
            if (pReturnData != NULL)
            {
                (*pReturnData) = m_pOwner->GetDependency((y - m_gridPosY - HEADER_HEIGHT - 1) / DEPENDENCY_HEIGHT);
            }
        }
    }
    return result;
}