#include "stdafx.h"
#include "PhysicsController.h"
#include "PhysicsElement.h"
#include "../Control/InputManager.h"
#include "../Function/Bind.h"

CPhysicsController::CPhysicsController( CPhysicsElement* pElement )
: m_moveSpeed(0.1f)
, m_pElement(pElement)
{
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_W, eKS_Press), Bind(*this, &CPhysicsController::MoveForward), false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_S, eKS_Press), Bind(*this, &CPhysicsController::MoveBack), false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_A, eKS_Press), Bind(*this, &CPhysicsController::MoveLeft), false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_D, eKS_Press), Bind(*this, &CPhysicsController::MoveRight), false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_UP, eKS_Press), Bind(*this, &CPhysicsController::MoveUp), false);
    CInputManager::GetInstance()->RegisterKeyBoardCallback(SKeyTriggerType(eKC_DOWN, eKS_Press), Bind(*this, &CPhysicsController::MoveDown), false);
}

CPhysicsController::~CPhysicsController()
{

}

void CPhysicsController::MoveForward()
{
    m_pElement->Translate(m_pElement->GetDirection() * m_moveSpeed);
}

void CPhysicsController::MoveBack()
{
    m_pElement->Translate(-m_pElement->GetDirection() * m_moveSpeed);
}

void CPhysicsController::MoveLeft()
{
    CVector3 leftDirection = m_pElement->GetUpDirection().Cross(m_pElement->GetDirection());
    m_pElement->Translate(-leftDirection * m_moveSpeed);
    const CVector3& worldPos = m_pElement->GetWorldPos();
    BEATS_PRINT(_T("Cur Pos : %f, %f, %f\n"), worldPos.x, worldPos.y, worldPos.z);
}

void CPhysicsController::MoveRight()
{
    CVector3 leftDirection = m_pElement->GetUpDirection().Cross(m_pElement->GetDirection());
    m_pElement->Translate(leftDirection * m_moveSpeed);
    const CVector3& worldPos = m_pElement->GetWorldPos();
    BEATS_PRINT(_T("Cur Pos : %f, %f, %f\n"), worldPos.x, worldPos.y, worldPos.z);
}

void CPhysicsController::MoveUp()
{
    m_pElement->Translate(m_pElement->GetUpDirection() * m_moveSpeed);
}

void CPhysicsController::MoveDown()
{
    m_pElement->Translate(-m_pElement->GetUpDirection() * m_moveSpeed);
}