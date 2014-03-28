#include "stdafx.h"
#include "Model.h"
#include "Skin.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Texture.h"
#include "AnimationController.h"
#include "ShaderProgram.h"
#include "RenderManager.h"
#include "AnimationManager.h"
#include "Animation.h"
#include "Resource/ResourceManager.h"
#include "Renderer.h"
#include "Material.h"
#include "RenderState.h"
#include "RenderStateParam.h"
#include "Shader.h"

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

CModel::CModel()
    : m_pAnimationController(CAnimationManager::GetInstance()->CreateSkelAnimationController())
    , m_bRenderSkeleton(true)
    , m_pStateParam(NULL)
{
}

CModel::CModel(CSerializer& serializer)
    : super(serializer)
    , m_pAnimationController(CAnimationManager::GetInstance()->CreateSkelAnimationController())
    , m_bRenderSkeleton(true)
    , m_pStateParam(NULL)
{
    DECLARE_PROPERTY(serializer, m_bRenderSkeleton, true, 0xFFFFFFFF, _T("äÖÈ¾¹Ç÷À"), NULL, _T("ÊÇ·ñäÖÈ¾¹Ç÷À"), NULL);
    DECLARE_PROPERTY(serializer, m_pSkeleton, true, 0xFFFFFFFF, _T("¹Ç÷À"), NULL, _T("¹Ç÷À"), NULL);
    DECLARE_PROPERTY(serializer, m_pStateParam, true, 0xFFFFFFFF, _T("²âÊÔ¸¸Àà"), NULL, _T("²âÊÔ¸¸Àà"), NULL)

}

CModel::~CModel()
{
    CAnimationManager::GetInstance()->DeleteController(m_pAnimationController);
}

bool CModel::Init()
{
#ifdef SW_SKEL_ANIM
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.vs"), false);
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("PointTexShader.ps"), false);
#else
    SharePtr<CShader> pVS = CResourceManager::GetInstance()->GetResource<CShader>(_T("SkinShader.vs"), false);
    SharePtr<CShader> pPS = CResourceManager::GetInstance()->GetResource<CShader>(_T("SkinShader.ps"), false);
#endif

    m_pSkinProgram = CRenderManager::GetInstance()->GetShaderProgram(pVS->ID(), pPS->ID());
    return true;
}

void CModel::PlayAnimationById( long id, float fBlendTime, bool bLoop )
{
    auto itr = m_animations.find(id);
    if(itr != m_animations.end())
    {
        m_pAnimationController->PlayAnimation(itr->second, fBlendTime, bLoop);
    }
}

void CModel::PlayAnimationByName( const char *name, float fBlendTime, bool bLoop )
{
    long id = MAKEFOURCC(name[0], name[1], name[2], name[3]);
    PlayAnimationById(id, fBlendTime, bLoop);
}

void CModel::AddTexture(SharePtr<CTexture> pTexture)
{
    m_textures.push_back(pTexture);
}

CAnimationController* CModel::GetAnimationController()
{
    return m_pAnimationController;
}

void CModel::PreRender()
{
    CRenderManager* pRenderMgr = CRenderManager::GetInstance();
    CRenderer* pRenderer = CRenderer::GetInstance();
    pRenderer->UseProgram(m_pSkinProgram->ID());
    FC_CHECK_GL_ERROR_DEBUG();

    if (m_pSkin && m_pSkin->IsLoaded())
    {
        for(size_t i = 0; i < m_textures.size(); ++i)
        {
            pRenderMgr->ApplyTexture(i, m_textures[i]->ID());
        }
        FC_CHECK_GL_ERROR_DEBUG();

        if(m_pAnimationController)
        {
            GLuint currShaderProgram = CRenderer::GetInstance()->GetCurrentState()->GetCurrentShaderProgram();
            if (currShaderProgram != 0)
            {
                auto &matrices = m_pAnimationController->GetDeltaMatrices();
#ifdef SW_SKEL_ANIM
                m_pSkin->CalcSkelAnim(matrices);
#else
                GLuint curProgram = currShaderProgram;
                for(auto &item : matrices)
                {
                    ESkeletonBoneType boneType = item.first;
                    char uniformName[32];
                    sprintf_s(uniformName, "%s[%d]", COMMON_UNIFORM_NAMES[UNIFORM_BONE_MATRICES], boneType);
                    GLuint location = pRenderer->GetUniformLocation(curProgram, uniformName);
                    pRenderer->SetUniformMatrix4fv(location, (const GLfloat *)item.second.mat, 1);
                }
#endif
            }
            FC_CHECK_GL_ERROR_DEBUG();
        }
    }
}

void CModel::Render()
{
    if (m_pSkin && m_pSkin->IsLoaded())
    {
        CRenderer* pRenderer = CRenderer::GetInstance();

        pRenderer->BindVertexArray(m_pSkin->GetVAO());
        int nBufferSize = 0;
        pRenderer->GetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &nBufferSize);
        int originalVertexArraySize = ( nBufferSize / sizeof(short) );
        pRenderer->DrawElements(GL_TRIANGLES, originalVertexArraySize, GL_UNSIGNED_SHORT, 0);
        FC_CHECK_GL_ERROR_DEBUG();
        pRenderer->BindVertexArray(0);

        if (m_pSkeleton != NULL && m_pAnimationController != NULL)
        {
            RenderSkeleton();
        }
    }
}

void CModel::PostRender()
{
    if (m_pSkin && m_pSkin->IsLoaded())
    {
        CRenderer::GetInstance()->BindVertexArray(0);
    }
}

void CModel::SetAnimaton( SharePtr<CAnimation> pAnimation )
{
    m_animations[0] = pAnimation;
}

void CModel::SetSkeleton( SharePtr<CSkeleton> pSkeleton )
{
    m_pSkeleton = pSkeleton;
    m_pAnimationController->SetSkeleton(pSkeleton);
}

SharePtr<CSkeleton> CModel::GetSkeleton() const
{
    return m_pSkeleton;
}

void CModel::SetSkin( SharePtr<CSkin> pSkin )
{
    m_pSkin = pSkin;
}

void CModel::RenderSkeleton()
{
    SharePtr<CAnimation> pCurrentAnimation = m_pAnimationController->GetCurrentAnimation();
    BEATS_ASSERT(pCurrentAnimation);
    //Construct render info
    std::vector<float> bonesRenderData;
    CVertexPC startVertex;
    CVertexPC endVertex;
    const std::vector<ESkeletonBoneType>& bones = pCurrentAnimation->GetBones();
    float fPlayingTime = m_pAnimationController->GetPlayingTime();
    for (size_t i = 0; i < bones.size(); ++i)
    {
        SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(bones[i]);
        const kmMat4* pMatrix = pCurrentAnimation->GetOneBoneMatrixByTime(fPlayingTime, pSkeletonBone->GetBoneType());
        if (pSkeletonBone->GetVisible())
        {
            SharePtr<CSkeletonBone> pParentBone = pSkeletonBone->GetParent();
            if (pParentBone)
            {
                const kmMat4* pParentMatrix = pCurrentAnimation->GetOneBoneMatrixByTime(fPlayingTime, pParentBone->GetBoneType());
                BEATS_ASSERT(pMatrix != NULL && pParentMatrix != NULL);

                startVertex.position.x = (*pMatrix).mat[12];
                startVertex.position.y = (*pMatrix).mat[13];
                startVertex.position.z = (*pMatrix).mat[14];
                startVertex.color = 0xFF0000FF;

                endVertex.position.x = (*pParentMatrix).mat[12];
                endVertex.position.y = (*pParentMatrix).mat[13];
                endVertex.position.z = (*pParentMatrix).mat[14];
                endVertex.color = 0xFFFF00FF;
                CRenderManager::GetInstance()->RenderLine(startVertex, endVertex);
            }
        }
        if (pSkeletonBone->GetCoordinateVisible())
        {
            CRenderManager::GetInstance()->RenderCoordinate(pMatrix);
        }
    }
}
