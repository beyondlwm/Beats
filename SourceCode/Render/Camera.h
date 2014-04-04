#ifndef RENDER_CAMERA_H__INCLUDE
#define RENDER_CAMERA_H__INCLUDE

class CCamera
{  
public:  
    CCamera();
    ~CCamera();

    void  SetCamera(const kmVec3& eyes,const kmVec3& look,const kmVec3& up);
    void  Roll(float angle);
    void  Pitch(float angle);
    void  Yaw(float angle);
    void  Translate(float x, float y, float z);

    void SetNear(float fZNear);
    void SetFar(float fZFar);
    void SetFOV(float fFOV);
    void SetAspect(float fAspect);

    float GetNear() const;
    float GetFar() const;
    float GetFOV() const;
    float GetAspect() const;

    void SetViewPos(float x, float y, float z);

    const void GetMatrix(kmMat4& mat) const;

    void ApplyCameraChange(bool bProj2D = false);

private:
    float m_fRotateSpeed;
    float m_fZNear;
    float m_fZFar;
    float m_fFOV;
    float m_fAspect;
    kmVec3 m_vec3Eye;
    kmVec3 m_vec3LookAt;
    kmVec3 m_vec3Up;
};
 
#endif