#include "stdafx.h"
#include "Camera.h"
#include "RenderManager.h"
#include "Renderer.h"
#include "RenderState.h"

CCamera::CCamera()
    : m_fRotateSpeed(2.5f)
    , m_fZNear(0.1f)
    , m_fZFar(1000.f)
    , m_fFOV(51.8f)
    , m_fAspect(0.0)
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

void CCamera::SetCamera(const kmVec3& eyes,const kmVec3& look,const kmVec3& up)
{
    kmVec3Assign(& m_vec3Eye, &eyes);
    kmVec3Assign(& m_vec3LookAt, &look);
    kmVec3Assign(& m_vec3Up, &up);
    ApplyCameraChange();
}

void  CCamera::Roll(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        kmVec3 vec3Straight;
        kmVec3Subtract(&vec3Straight, &m_vec3LookAt, &m_vec3Eye);
        kmMat4 rotationMat;
        kmMat4RotationAxisAngle(&rotationMat, &vec3Straight, angle * m_fRotateSpeed);
        kmVec3Transform(&m_vec3Up, &m_vec3Up, &rotationMat);

        ApplyCameraChange();
    }
}
void  CCamera::Pitch(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        kmVec3 vec3Straight;
        kmVec3Subtract(&vec3Straight, &m_vec3LookAt, &m_vec3Eye);
        kmVec3 vec3RightDirection;
        kmVec3Cross(&vec3RightDirection, &vec3Straight, &m_vec3Up);
        kmMat4 rotationMat;
        kmMat4RotationAxisAngle(&rotationMat, &vec3RightDirection, angle * m_fRotateSpeed);
        kmVec3Transform(&vec3Straight, &vec3Straight, &rotationMat);
        kmVec3Add(&m_vec3LookAt, &m_vec3Eye, &vec3Straight);

        ApplyCameraChange();
    }
}
void  CCamera::Yaw(float angle)
{
    if (!BEATS_FLOAT_EQUAL(angle, 0))
    {
        kmMat4 rotationMat;
        kmMat4RotationAxisAngle(&rotationMat, &m_vec3Up, angle * m_fRotateSpeed);
        kmVec3 vec3Offset;
        kmVec3Subtract(&vec3Offset, &m_vec3LookAt, &m_vec3Eye);

        kmVec3Transform(&vec3Offset, &vec3Offset, &rotationMat);
        kmVec3Add(&m_vec3LookAt, &m_vec3Eye, &vec3Offset);

        ApplyCameraChange();
    }
}

void  CCamera::Translate(float x, float y, float z)
{
    m_vec3Eye.x += x;
    m_vec3Eye.y += y;
    m_vec3Eye.z += z;
    m_vec3LookAt.x += x;
    m_vec3LookAt.y += y;
    m_vec3LookAt.z += z;
}

void CCamera::SetNear(float fZNear)
{
    if (!BEATS_FLOAT_EQUAL(m_fZNear, fZNear))
    {
        m_fZNear = fZNear;
        ApplyCameraChange();
    }
}

void CCamera::SetFar(float fZFar)
{
    if (!BEATS_FLOAT_EQUAL(m_fZFar, fZFar))
    {
        m_fZFar = fZFar;
        ApplyCameraChange();
    }
}

void CCamera::SetFOV(float fFOV)
{
    if (!BEATS_FLOAT_EQUAL(m_fFOV, fFOV))
    {
        m_fFOV = fFOV;
        ApplyCameraChange();
    }
}

void CCamera::SetAspect(float fAspect)
{
    if (!BEATS_FLOAT_EQUAL(m_fAspect, fAspect))
    {
        m_fAspect = fAspect;
        ApplyCameraChange();
    }
}

float CCamera::GetNear() const
{
    return m_fZNear;
}

float CCamera::GetFar() const
{
    return m_fZFar;
}

float CCamera::GetFOV() const
{
    return m_fFOV;
}

float CCamera::GetAspect() const
{
    return m_fAspect;
}

void CCamera::SetViewPos(float x, float y, float z)
{
    kmVec3Fill(&m_vec3Eye, x, y, z);
    ApplyCameraChange();
}

const void CCamera::GetMatrix(kmMat4& mat) const
{
    kmMat4Identity(&mat);
    kmMat4LookAt(&mat, &m_vec3Eye, &m_vec3LookAt, &m_vec3Up);
}

void CCamera::ApplyCameraChange(bool bProj2D)
{
    GLuint uCurrentShader = CRenderer::GetInstance()->GetCurrentState()->GetCurrentShaderProgram();
    if(uCurrentShader != 0)
    {
        kmMat4 projMat;
        kmMat4 MVPmat;
        if(bProj2D)
        {
            int iWidth = 0;
            int iHeight = 0;
            CRenderManager::GetInstance()->GetWindowSize(iWidth, iHeight);

            kmMat4OrthographicProjection(&projMat, 
                0.0f, static_cast<kmScalar>(iWidth),
                static_cast<kmScalar>(iHeight), 0.0f,
                -1000.1f, 10000.0f);
        }
        else
        {
            kmMat4PerspectiveProjection(&projMat, m_fFOV, m_fAspect, m_fZNear, m_fZFar);
        }
        kmMat4 cameraMat;
        if(bProj2D)
            kmMat4Identity(&cameraMat);
        else
            GetMatrix(cameraMat);
        kmMat4Multiply(&MVPmat, &projMat, &cameraMat);
#ifdef USE_UBO
        const GLvoid *matrices[4] = {
            vpmatrix.mat,
            modelmatrix.mat,
            viewMatrix.mat,
            projection.mat,
        };
        GLsizeiptr sizes[4] = {
            sizeof(kmMat4),
            sizeof(kmMat4),
            sizeof(kmMat4),
            sizeof(kmMat4),
        };

        UpdateUBO(UNIFORM_BLOCK_MVP_MATRIX, matrices, sizes, 4);
#else
        GLint MVPLocation = CRenderer::GetInstance()->GetUniformLocation(uCurrentShader, COMMON_UNIFORM_NAMES[UNIFORM_MVP_MATRIX]);
        BEATS_ASSERT(MVPLocation != -1);
        CRenderer::GetInstance()->SetUniformMatrix4fv(MVPLocation, (const float*)MVPmat.mat, 1);
    }
#endif
}