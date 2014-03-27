#include "stdafx.h"
#include "Camera.h"
#include "RenderManager.h"

CCamera::CCamera()
    :m_viewAngle(0.f)
    , m_aspect(0.f)
    , m_nearDist(0.f)
    , m_farDist(0.f)
    , m_fPitch(0.f)
    , m_fYaw(0.f)
    , m_fRoll(0.f)
{
    kmVec3 eye, look, up;
    kmVec3Fill(&eye,0.0F, 0.5F, 10.0F);
    kmVec3Fill(&look,0.0F, 0.0F, 0.0F);
    kmVec3Fill(&up, 0.0F,1.0F,0.0F);
    SetCamera(eye,look,up);
}
CCamera::~CCamera()
{

}

void  CCamera::SetCamera(kmVec3 eye,kmVec3 look,kmVec3 up)
{
    kmVec3Fill(&m_eye, eye.x, eye.y, eye.z);
    kmVec3Fill(&m_look,look.x, look.y, look.z);
    kmVec3Fill(&m_up,up.x, up.y, up.z);
    kmVec3 vecup;
    kmVec3Fill(&vecup,up.x - eye.x,up.y - eye.y,up.z - eye.z);

    kmVec3Fill(&m_n,eye.x - look.x, eye.y - look.y, eye.z - look.z);

    kmVec3 cross1;
    kmVec3Cross(&cross1,&vecup, &m_n);
    kmVec3Fill(&m_u,cross1.x, cross1.y, cross1.z);

    kmVec3 cross2;
    kmVec3Cross(&cross2, &m_n, &m_u);
    kmVec3Fill(&m_v,cross2.x, cross2.y, cross2.z);

    kmVec3Normalize(&m_u, &m_u);
    kmVec3Normalize(&m_v, &m_v);
    kmVec3Normalize(&m_n, &m_n);

    Update();

}
void  CCamera::Roll(float angle)
{
    m_fRoll = angle;
    float cosTheta = cos(angle * MATH_PI / 180);  
    float sinnTheta = sin(angle * MATH_PI / 180); 

    kmVec3 srcu;
    kmVec3Assign(&srcu,&m_u);

    kmVec3 srcv;
    kmVec3Assign(&srcv,&m_v);;  

    kmVec3Fill(&m_u,cosTheta * srcu.x - sinnTheta * srcv.x, cosTheta * srcu.y - sinnTheta * srcv.y, cosTheta * srcu.z - sinnTheta * srcv.z);
    kmVec3Fill(&m_v,sinnTheta * srcu.x + cosTheta * srcv.x, sinnTheta * srcu.y + cosTheta * srcv.y, sinnTheta * srcu.z + cosTheta * srcv.z);

}
void  CCamera::Pitch(float angle)
{
    m_fPitch = angle;
    float cosTheta=cos(angle * MATH_PI / 180);  
    float sinTheta=sin(angle * MATH_PI / 180);  

    kmVec3 srcv;
    kmVec3Assign(&srcv,&m_v);
    kmVec3 srcn;
    kmVec3Assign(&srcn,&m_n);

    m_v.x = cosTheta * srcv.x - sinTheta * srcn.x;  
    m_v.y = cosTheta * srcv.y - sinTheta * srcn.y;  
    m_v.z = cosTheta * srcv.z - sinTheta * srcn.z;  

    m_n.x = sinTheta * srcv.x + cosTheta * srcn.x;  
    m_n.y = sinTheta * srcv.y + cosTheta * srcn.y;  
    m_n.z = sinTheta * srcv.z + cosTheta * srcn.z;
}
void  CCamera::Yaw(float angle)
{
    m_fYaw = angle;
    float cosTheta=cos(angle * MATH_PI / 180);  
    float sinTheta=sin(angle * MATH_PI / 180); 

    kmVec3 srcn, srcv, srcu;
    kmVec3Assign(&srcn,&m_n);
    kmVec3Assign(&srcv,&m_v);
    kmVec3Assign(&srcu,&m_u);

    m_n.z = cosTheta * srcn.z + sinTheta * srcn.x;
    m_n.x = cosTheta * srcn.x - sinTheta * srcn.z;

    m_v.z = cosTheta * srcv.z + sinTheta * srcv.x;
    m_v.x = cosTheta * srcv.x - sinTheta * srcv.z;

    m_u.z = cosTheta * srcu.z + sinTheta * srcu.x;
    m_u.x = cosTheta * srcu.x - sinTheta * srcu.z;
}
void  CCamera::Slide(float du, float dv, float dn)
{
    m_eye.x += du * m_u.x + dv * m_v.x + dn * m_n.x;  
    m_eye.y += du * m_u.y + dv * m_v.y + dn * m_n.y;  
    m_eye.z += du * m_u.z + dv * m_v.z + dn * m_n.z;  
    m_look.x += du * m_u.x + dv * m_v.x + dn * m_n.x;  
    m_look.y += du * m_u.y + dv * m_v.y + dn * m_n.y;  
    m_look.z += du * m_u.z + dv * m_v.z + dn * m_n.z;  

}
void CCamera::MoveStraight(float fSpeed)
{
    Slide(0.0F,0.0F,fSpeed);
}
void CCamera::MoveTransverse(float fSpeed)
{
    Slide(fSpeed,0.0F,0.0F);
}
void CCamera::MoveUpDown(float fSpeed)
{
    Slide(0.0F,fSpeed,0.0F);
}
double CCamera::GetDist()
{
    float dist = pow(m_eye.x,2) + pow(m_eye.y,2) + pow(m_eye.z,2);  
    return pow(dist,0.5);  

}
float CCamera::GetPitch() const
{
    return m_fPitch;
}
float CCamera::GetYaw() const
{
    return m_fYaw;
}
float CCamera::GetRoll() const
{
    return m_fRoll;
}

void CCamera::SetViewPos(float x, float y, float z)
{
    kmVec3Fill(&m_look, x, y, z);
}

void CCamera::Update()
{
    kmMat4Identity(&m_mat);

    m_mat.mat[0] = m_u.x;
    m_mat.mat[4] = m_u.y;
    m_mat.mat[8] = m_u.z;

    m_mat.mat[1] = m_v.x;
    m_mat.mat[5] = m_v.y;
    m_mat.mat[9] = m_v.z;

    m_mat.mat[2] = m_n.x;
    m_mat.mat[6] = m_n.y;
    m_mat.mat[10] = m_n.z;

    kmMat4 translate;
    kmMat4Translation(&translate, -m_eye.x, -m_eye.y, -m_eye.z);
    kmMat4Multiply(&m_mat, &m_mat, &translate);

    kmGLMultMatrix(&m_mat);
}
void CCamera::RotateX(float angle)  
{  
    Yaw(angle);
    float dist =(float)GetDist();  
    kmVec3Fill(&m_eye, dist * m_n.x, dist * m_n.y, dist * m_n.z);
}  

void CCamera::RotateY(float angle)  
{  
    Pitch(angle);
    float dist =(float)GetDist();  
    kmVec3Fill(&m_eye, dist * m_n.x, dist * m_n.y, dist * m_n.z);
} 

void  CCamera::SetShape(float viewAngle, float aspect, float Near, float Far)  
{  
    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLLoadIdentity();
    kmMat4 mat;
    kmMat4PerspectiveProjection(&mat,viewAngle,aspect,Near,Far);
    kmGLMultMatrix(&mat);     
}  

