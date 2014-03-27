#ifndef RENDER_CAMERA_H__INCLUDE
#define RENDER_CAMERA_H__INCLUDE

class CCamera  
{  
public:  
    CCamera();  
    ~CCamera();

    void  SetCamera(kmVec3 eyes,kmVec3 look,kmVec3 up);  
    void  Roll(float angle);  
    void  Pitch(float angle);  
    void  Yaw(float angle);  
    void  Slide(float du, float dv, float dn);  
    double GetDist();  
    void  SetShape(float viewAngle,float aspect,float Near,float Far); 
    void MoveStraight(float fSpeed);
    void MoveTransverse(float fSpeed);
    void MoveUpDown(float fSpeed);

    float GetPitch() const;
    float GetYaw() const;
    float GetRoll() const;
    void SetViewPos(float x, float y, float z);
    void Update();

    void RotateX(float angle);
    void RotateY(float angle);

private:  
    kmVec3         m_eye,m_look,m_up;  
    kmVec3         m_u,m_v,m_n;  
    float          m_viewAngle, m_aspect, m_nearDist, m_farDist;  
    
    float          m_fPitch;
    float          m_fYaw;
    float          m_fRoll;

    kmMat4 m_mat;
};
 
#endif