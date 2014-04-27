#ifndef RENDER_CAMERA_H__INCLUDE
#define RENDER_CAMERA_H__INCLUDE

class CCamera
{
public:
    enum ECameraType
    {
        eCT_2D,
        eCT_3D,
    };

    enum ECameraMoveType
    {
        eCMT_NOMOVE = 0,
        eCMT_TRANVERSE,
        eCMT_STRAIGHT,
        eCMT_UPDOWN,

        eCMT_Count,
        eCMT_Force32Bit = 0xFFFFFFFF
    };

public:
    CCamera(ECameraType type = eCT_3D);
    ~CCamera();

    ECameraType GetType() const;

    void  SetCamera(const kmVec3& eyes,const kmVec3& look,const kmVec3& up);
    void  Roll(float angle);
    void  Pitch(float angle);
    void  Yaw(float angle);
    void  Translate(float x, float y, float z);

    void SetNear(float fZNear);
    void SetFar(float fZFar);
    void SetFOV(float fFOV);
    void SetWidth(size_t uWidth);
    void SetHeight(size_t uHeight);

    float GetNear() const;
    float GetFar() const;
    float GetFOV() const;
    size_t GetWidth() const;
    size_t GetHeight() const;

    void SetViewPos(float x, float y, float z);
    void GetViewPos(kmVec3& out) const;

    void GetProjectionMatrix(kmMat4 &mat) const;
    void GetViewMatrix(kmMat4& mat) const;

    void ApplyCameraChange();
    void Update(const kmVec3& vec3Speed, int type);

    void SetCameraCenterOffset(const kmVec2& centerOffset);

private:
    ECameraType m_type;
    float m_fRotateSpeed;
    float m_fZNear;
    float m_fZFar;
    float m_fFOV;
    size_t m_uWidth;
    size_t m_uHeight;
    kmVec3 m_vec3Eye;
    kmVec3 m_vec3LookAt;
    kmVec3 m_vec3Up;
    kmVec2 m_vec2CameraCenterOffset;
};
 
#endif