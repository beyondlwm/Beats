#ifndef BEATS_RENDER_CAMERA_CAMERA_H__INCLUDE
#define BEATS_RENDER_CAMERA_CAMERA_H__INCLUDE

#include "../../Utility/Math/Vector3.h"
#include "../../Physics/PhysicsElement.h"

class CPhysicsController;

class CCamera
{
public:
    CCamera();
    ~CCamera();

    void Pitch(float angle); // rotate on right vector
    void Yaw(float angle);   // rotate on up vector
    void Roll(float angle);  // rotate on look vector

    void GetViewMatrix(D3DXMATRIX* out);
    void GetProjMatrix(D3DXMATRIX* out);
    const D3DXMATRIX& GetViewMatrix();
    const D3DXMATRIX& GetProjMatrix();

    void UpdateMatrix();

    CPhysicsElement* GetPhysics();
private:
    void SetDirection(D3DXVECTOR3& dest, const D3DXVECTOR3& src);
    void Rotate(float angle, const D3DXVECTOR3& aroundVec, D3DXVECTOR3& axis1);
    void Rotate(float angle, const D3DXVECTOR3& aroundVec, D3DXVECTOR3& axis1, D3DXVECTOR3& axis2);
    void RefreshDxData();

private:
    CPhysicsController* m_pController;

    //these 3 data only caches for dx calculation
    D3DXVECTOR3 m_pos;
    D3DXVECTOR3 m_lookAt;
    D3DXVECTOR3 m_up;

    D3DXMATRIX m_viewMatrix;
    D3DXMATRIX m_projectionMatrix;
    CPhysicsElement m_physicsElement;
};

#endif