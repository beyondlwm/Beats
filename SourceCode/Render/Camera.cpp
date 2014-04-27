#include "stdafx.h"
#include "Camera.h"
#include "RenderManager.h"
#include "Renderer.h"
#include "RenderState.h"

CCamera::CCamera(ECameraType type /* = eCT_3D */)
    : m_type(type)
    , m_fRotateSpeed(2.5f)
    , m_fZNear(0)
    , m_fZFar(1000.f)
    , m_fFOV(51.8f)
    , m_uWidth(0)
    , m_uHeight(0)
{
    kmVec3 eye, look, up;
    kmVec3Fill(&eye,0.0F, m_type == eCT_2D ? 0 : 0.5f, m_type == eCT_2D ? 0 : 10.0F);
    kmVec3Fill(&look,0.0F, 0.0F, 0.0F);
    kmVec3Fill(&up, 0.0F,1.0F,0.0F);
    SetCamera(eye,look,up);
    kmVec2Fill(&m_vec2CameraCenterOffset, 0, 0);
    SetNear(m_type == eCT_2D ? -1.f : 0.1f);
}

CCamera::~CCamera()
{

}

CCamera::ECameraType CCamera::GetType() const
{
    return m_type;
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

void CCamera::SetWidth(size_t uWidth)
{
    if (m_uWidth != uWidth)
    {
        m_uWidth = uWidth;
        if (m_uHeight > 0)
        {
            ApplyCameraChange();
        }
    }
}

void CCamera::SetHeight(size_t uHeight)
{
    if (m_uHeight != uHeight)
    {
        m_uHeight = uHeight;
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

size_t CCamera::GetWidth() const
{
    return m_uWidth;
}

size_t CCamera::GetHeight() const
{
    return m_uHeight;
}

void CCamera::SetViewPos(float x, float y, float z)
{
    kmVec3Fill(&m_vec3Eye, x, y, z);
    ApplyCameraChange();
}

void CCamera::GetViewPos(kmVec3& out) const
{
    kmVec3Assign(&out, &m_vec3Eye);
}

void CCamera::GetViewMatrix(kmMat4& mat) const
{
    kmMat4Identity(&mat);
    switch(m_type)
    {
    case eCT_2D:
        kmMat4Translation(&mat, m_vec3Eye.x, m_vec3Eye.y, m_vec3Eye.z);
        break;
    case eCT_3D:
        kmMat4LookAt(&mat, &m_vec3Eye, &m_vec3LookAt, &m_vec3Up);
        break;
    }
}

void CCamera::GetProjectionMatrix(kmMat4 &mat) const
{
    switch(m_type)
    {
    case eCT_2D:
        kmMat4OrthographicProjection(&mat, 
            m_vec2CameraCenterOffset.x, static_cast<kmScalar>(m_uWidth) + m_vec2CameraCenterOffset.x,
            static_cast<kmScalar>(m_uHeight) + m_vec2CameraCenterOffset.y, m_vec2CameraCenterOffset.y,
            m_fZNear, m_fZFar);
        break;
    case eCT_3D:
        kmMat4PerspectiveProjection(&mat, m_fFOV, (float)m_uWidth / m_uHeight, m_fZNear, m_fZFar);
        break;
    }
}

void CCamera::ApplyCameraChange()
{
    CRenderState* pState = CRenderer::GetInstance()->GetCurrentState();
    BEATS_ASSERT(pState != NULL);
    GLint uCurrentShader = pState->GetCurrentShaderProgram();
#ifdef _DEBUG
    GLint uCurShaderFromGPU = -1;
    glGetIntegerv(GL_CURRENT_PROGRAM, &uCurShaderFromGPU);
    BEATS_ASSERT(uCurShaderFromGPU == uCurrentShader || (uCurrentShader == 0 && uCurShaderFromGPU == -1), _T("State is out of sync for current shader program! State:%d GPU State:%d"),
        uCurrentShader, uCurShaderFromGPU);
#endif
    if(uCurrentShader != 0)
    {
        kmMat4 projMat;
        GetProjectionMatrix(projMat);
        kmMat4 cameraMat;
        GetViewMatrix(cameraMat);
        kmMat4 MVPmat;
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
        FC_CHECK_GL_ERROR_DEBUG();
#endif
    }
}

void CCamera::Update(const kmVec3& vec3Speed, int type)
{
    kmMat4 cameraMat;
    GetViewMatrix(cameraMat);
    kmMat4Inverse(&cameraMat, &cameraMat);
    kmVec3 vec3Translation;
    kmVec3Zero(&vec3Translation);
    if ((type & (1 << eCMT_TRANVERSE)) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetRightVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.x);
        tmpTranslation.y = 0;
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    if ((type & (1 << eCMT_STRAIGHT)) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetForwardVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.z);
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    if ((type & (1 << eCMT_UPDOWN)) != 0)
    {
        kmVec3 tmpTranslation;
        kmMat4GetUpVec3(&tmpTranslation, &cameraMat);
        kmVec3Scale(&tmpTranslation, &tmpTranslation, vec3Speed.y);
        tmpTranslation.x = 0;
        tmpTranslation.z = 0;
        kmVec3Add(&vec3Translation, &vec3Translation, &tmpTranslation);
    }
    Translate(vec3Translation.x, vec3Translation.y, vec3Translation.z);
    ApplyCameraChange();
}

void CCamera::SetCameraCenterOffset(const kmVec2& centerOffset)
{
    m_vec2CameraCenterOffset.x = centerOffset.x;
    m_vec2CameraCenterOffset.y = centerOffset.y;
}
