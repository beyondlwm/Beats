#include "stdafx.h"
#include "Camera.h"
#include "../../Physics/PhysicsController.h"

CCamera::CCamera()
: m_pController(NULL)
{
    UpdateMatrix();
    D3DXMatrixOrthoLH(&m_projectionMatrix, BEATS_WINDOW_WIDTH, BEATS_WINDOW_HEIGHT, 0, 1000.f);
    m_pController = new CPhysicsController(&m_physicsElement);
}

CCamera::~CCamera()
{
    BEATS_SAFE_DELETE(m_pController);
}

void CCamera::Rotate( float angle, const D3DXVECTOR3& aroundVec, D3DXVECTOR3& axis1, D3DXVECTOR3& axis2 )
{
    BEATS_ASSERT(aroundVec != D3DXVECTOR3(0,0,0) && axis1 != D3DXVECTOR3(0,0,0) && axis2 != D3DXVECTOR3(0,0,0), _T("Axis can't be zero!"));
    D3DXMATRIX T;
    D3DXMatrixRotationAxis(&T, &aroundVec, angle);

    D3DXVec3TransformCoord(&axis1, &axis1, &T);
    D3DXVec3TransformCoord(&axis2, &axis2, &T);
}

void CCamera::Rotate( float angle, const D3DXVECTOR3& aroundVec, D3DXVECTOR3& axis1)
{
    BEATS_ASSERT(aroundVec != D3DXVECTOR3(0,0,0) && axis1 != D3DXVECTOR3(0,0,0) , _T("Axis can't be zero!"));
    D3DXMATRIX T;
    D3DXMatrixRotationAxis(&T, &aroundVec, angle);

    D3DXVec3TransformCoord(&axis1, &axis1, &T);
}

void CCamera::GetViewMatrix( D3DXMATRIX* out )
{
    UpdateMatrix();
    (*out) = m_viewMatrix;
}

const D3DXMATRIX& CCamera::GetViewMatrix()
{
    UpdateMatrix();
    return m_viewMatrix;
}
void CCamera::GetProjMatrix( D3DXMATRIX* out )
{
    UpdateMatrix();
    (*out) = m_projectionMatrix;
}

const D3DXMATRIX& CCamera::GetProjMatrix()
{
    UpdateMatrix();
    return m_projectionMatrix;
}
void CCamera::Pitch( float angle )
{
    D3DXVECTOR3 rightDir;
    D3DXVec3Cross(&rightDir, &m_lookAt, &m_up);
    Rotate(angle, rightDir, m_lookAt, m_up);
    D3DXMatrixLookAtLH(&m_viewMatrix, &m_pos, &m_lookAt, &m_up);
}

void CCamera::Yaw( float angle )
{
    Rotate(angle, m_up, m_lookAt);
    D3DXMatrixLookAtLH(&m_viewMatrix, &m_pos, &m_lookAt, &m_up);
}

void CCamera::Roll( float angle )
{
    Rotate(angle, m_lookAt, m_up);
    D3DXMatrixLookAtLH(&m_viewMatrix, &m_pos, &m_lookAt, &m_up);
}

void CCamera::UpdateMatrix()
{
    RefreshDxData();
    D3DXMatrixLookAtLH(&m_viewMatrix, &m_pos, &m_lookAt, &m_up);
}

void CCamera::RefreshDxData()
{
    m_physicsElement.GetWorldPos().ConvertTo(m_pos);
    m_physicsElement.GetDirection().ConvertTo(m_lookAt);
    m_lookAt += m_pos;//always look at the same direction.
    m_physicsElement.GetUpDirection().ConvertTo(m_up);
}

CPhysicsElement* CCamera::GetPhysics()
{
    return &m_physicsElement;
}
