#include "stdafx.h"
#include "DependencyDescription.h"
#include "DependencyDescriptionLine.h"
#include "Component/ComponentGraphic.h"
#include "Component/ComponentEditorProxy.h"
#include <math.h>
#include "Utility/Math/Matrix4.h"
#include "Utility/Math/MathPublic.h"

CDependencyDescriptionLine::CDependencyDescriptionLine( CDependencyDescription* pOwner, size_t uIndex, CComponentProxy* pTo )
: m_bIsSelected(false)
, m_uIndex(uIndex)
, m_pOwner(pOwner)
, m_pConnectedComponent(NULL)
{
    m_pRect = new SVertex[4];
    m_pArrowRect = new SVertex[4];
    for (size_t i = 0; i < 4; ++i)
    {
        m_pRect[i].m_color = 0xffffffff;
        m_pRect[i].m_uv.x = i < 2 ? 0.f : 1.f;
        m_pRect[i].m_uv.y = (i % 2 == 0) ? 0.f : 1.f;
        m_pArrowRect[i].m_color = 0xffffffff;
        m_pArrowRect[i].m_uv.x = i < 2 ? 0.f : 1.f;
        m_pArrowRect[i].m_uv.y = (i % 2 == 0) ? 0.f : 1.f;
    }
    SetConnectComponent(pTo);
}

CDependencyDescriptionLine::~CDependencyDescriptionLine()
{
    m_pConnectedComponent->RemoveBeConnectedDependencyDescriptionLine(this);
    m_pOwner->RemoveDependencyLine(this);
    BEATS_SAFE_DELETE_ARRAY(m_pRect);
    BEATS_SAFE_DELETE_ARRAY(m_pArrowRect);
}

void CDependencyDescriptionLine::UpdateRect(float cellSize, bool bInverseY /*= false*/)
{
    if (m_pOwner != NULL && m_pConnectedComponent != NULL)
    {
        int iDependencyPosX = 0; 
        int iDependencyPosY = 0;
        size_t uIndex = m_pOwner->GetIndex();
        
        CComponentGraphic* pGraphics = static_cast<CComponentProxy*>(m_pOwner->GetOwner())->GetGraphics();
        BEATS_ASSERT(pGraphics != NULL);
        pGraphics->GetDependencyPosition(uIndex, &iDependencyPosX, &iDependencyPosY);
        // 1. Draw line rect
        // a. calculate angle.
        CVector2 point[2];
        point[0].x = iDependencyPosX * cellSize + cellSize * 0.5f * pGraphics->GetDependencyWidth();
        point[0].y = iDependencyPosY * cellSize - cellSize * 0.5f * pGraphics->GetDependencyHeight() * (bInverseY ? -1 : 1);

        int x = 0;
        int y = 0;
        static_cast<CComponentProxy*>(m_pConnectedComponent)->GetGraphics()->GetPosition(&x, &y);
        point[1].x = x * cellSize + cellSize * 0.5f * pGraphics->GetConnectWidth();
        point[1].y = y * cellSize - cellSize * 0.5f * pGraphics->GetHeaderHeight() * (bInverseY ? -1 : 1);

        CVector2 direction(point[1] - point[0]);
        float fAngle = atan2f(direction.x, direction.y);
        float fLength = direction.Length();
        CTransform4D rotateTransform;
        rotateTransform.SetRotationAboutZ(-fAngle);
        // b. found a rect line at the dependency center with direction up.

        // left top.
        CVector2 leftTop(-pGraphics->GetDependencyLineWidth(), fLength);
        CVector2 newLeftTop = rotateTransform * leftTop;
        newLeftTop.x += point[0].x;
        newLeftTop.y += point[0].y;

        m_pRect[0].m_position.x = newLeftTop.x;
        m_pRect[0].m_position.y = newLeftTop.y;
        m_pRect[0].m_position.z = 0.0f;

        // left down.
        CVector2 leftDown(-pGraphics->GetDependencyLineWidth(), 0);
        CVector2 newLeftDown = rotateTransform * leftDown;
        newLeftDown.x += point[0].x;
        newLeftDown.y += point[0].y;
        if (bInverseY)
        {
            newLeftDown.y -= cellSize * pGraphics->GetHeaderHeight() * 0.5f;
        }

        m_pRect[1].m_position.x = newLeftDown.x;
        m_pRect[1].m_position.y = newLeftDown.y;
        m_pRect[1].m_position.z = 0.0f;

        // right top.
        CVector2 rightTop(pGraphics->GetDependencyLineWidth(), fLength);
        CVector2 newRightTop = rotateTransform * rightTop;
        newRightTop.x += point[0].x;
        newRightTop.y += point[0].y;

        m_pRect[2].m_position.x = newRightTop.x;
        m_pRect[2].m_position.y = newRightTop.y;
        m_pRect[2].m_position.z = 0.0f;

        // right down.
        CVector2 rightDown(pGraphics->GetDependencyLineWidth(), 0);
        CVector2 newRightDown = rotateTransform * rightDown;
        newRightDown.x += point[0].x;
        newRightDown.y += point[0].y;
        if (bInverseY)
        {
            newRightDown.y -= cellSize * pGraphics->GetHeaderHeight() * 0.5f;
        }

        m_pRect[3].m_position.x = newRightDown.x;
        m_pRect[3].m_position.y = newRightDown.y;
        m_pRect[3].m_position.z = 0.0f;

        //2. Render arrow
        // left top
        float fArrowSize = pGraphics->GetDependencyLineArrowSize();
        CVector2 arrowLeftTop( -fArrowSize, fLength);
        CVector2 newArrowLeftTop = rotateTransform * arrowLeftTop;
        newArrowLeftTop.x += point[0].x;
        newArrowLeftTop.y += point[0].y;

        m_pArrowRect[0].m_position.x = newArrowLeftTop.x;
        m_pArrowRect[0].m_position.y = newArrowLeftTop.y;
        m_pArrowRect[0].m_position.z = 0.0f;

        //left down
        CVector2 arrowLeftDown(-fArrowSize, fLength - fArrowSize);
        CVector2 newArrowLeftDown = rotateTransform * arrowLeftDown;
        newArrowLeftDown.x += point[0].x;
        newArrowLeftDown.y += point[0].y;

        m_pArrowRect[1].m_position.x = newArrowLeftDown.x;
        m_pArrowRect[1].m_position.y = newArrowLeftDown.y;
        m_pArrowRect[1].m_position.z = 0.0f;

        //right top
        CVector2 arrowRightTop(fArrowSize, fLength);
        CVector2 newArrowRightTop = rotateTransform * arrowRightTop;
        newArrowRightTop.x += point[0].x;
        newArrowRightTop.y += point[0].y;

        m_pArrowRect[2].m_position.x = newArrowRightTop.x;
        m_pArrowRect[2].m_position.y = newArrowRightTop.y;
        m_pArrowRect[2].m_position.z = 0.0f;

        //right down
        CVector2 arrowRightDown(fArrowSize, fLength - fArrowSize);
        CVector2 newArrowRightDown = rotateTransform * arrowRightDown;
        newArrowRightDown.x += point[0].x;
        newArrowRightDown.y += point[0].y;

        m_pArrowRect[3].m_position.x = newArrowRightDown.x;
        m_pArrowRect[3].m_position.y = newArrowRightDown.y;
        m_pArrowRect[3].m_position.z = 0.0f;
    }
}

const SVertex* CDependencyDescriptionLine::GetRectArray()
{
    return m_pRect;
}

CDependencyDescription* CDependencyDescriptionLine::GetOwnerDependency()
{
    return m_pOwner;
}

CComponentProxy* CDependencyDescriptionLine::GetConnectedComponent()
{
    return m_pConnectedComponent;
}

void CDependencyDescriptionLine::SetConnectComponent( CComponentProxy* pComponentTo )
{
    if (m_pConnectedComponent != NULL)
    {
        m_pConnectedComponent->RemoveBeConnectedDependencyDescriptionLine(this);
    }
    m_pConnectedComponent = pComponentTo;
    if (m_pConnectedComponent != NULL)
    {
        m_pConnectedComponent->AddBeConnectedDependencyDescriptionLine(this);
    }
}

bool CDependencyDescriptionLine::IsSelected()
{
    return m_bIsSelected;
}

void CDependencyDescriptionLine::SetSelected(bool bFlag)
{
    m_bIsSelected = bFlag;
}

size_t CDependencyDescriptionLine::GetIndex()
{
    return m_uIndex;
}

void CDependencyDescriptionLine::SetIndex( size_t uIndex )
{
    m_uIndex = uIndex;
}

const SVertex* CDependencyDescriptionLine::GetArrowRectArray()
{
    return m_pArrowRect;
}

bool CDependencyDescriptionLine::HitTest( float x, float y )
{
    bool bRet = false;
    BEATS_ASSERT(m_pRect != NULL);
    if (m_pRect != NULL)
    {
        float top = (m_pRect[1].m_position.x - m_pRect[0].m_position.x) * (y - m_pRect[0].m_position.y) - (x - m_pRect[0].m_position.x) * (m_pRect[1].m_position.y - m_pRect[0].m_position.y);
        float buttom = (m_pRect[3].m_position.x - m_pRect[2].m_position.x) * (y - m_pRect[2].m_position.y) - (x - m_pRect[2].m_position.x) * (m_pRect[3].m_position.y - m_pRect[2].m_position.y);
        bool bBetweenHorizontal = top * buttom <= 0;

        float left = (m_pRect[3].m_position.x - m_pRect[1].m_position.x) * (y - m_pRect[1].m_position.y) - (x - m_pRect[1].m_position.x) * (m_pRect[3].m_position.y - m_pRect[1].m_position.y);
        float right = (m_pRect[2].m_position.x - m_pRect[0].m_position.x) * (y - m_pRect[0].m_position.y) - (x - m_pRect[0].m_position.x) * (m_pRect[2].m_position.y - m_pRect[0].m_position.y);
        bool bBetweenVertical = left * right <= 0;
        bRet = bBetweenHorizontal && bBetweenVertical;
    }
    return bRet;
}
