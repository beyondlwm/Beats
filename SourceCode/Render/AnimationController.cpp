#include "stdafx.h"
#include "Animation.h"
#include "AnimationController.h"
#include "Skeleton.h"
#include "SkeletonBone.h"
#include "Resource/ResourcePublic.h"
#include <math.h>
#include "RenderManager.h"

CAnimationController::CAnimationController()
    : m_fPlayingTime(0.0f)
    , m_bLoop(false)
    , m_bPlaying(false)
    , m_bBlendTime(0)
{
}

CAnimationController::~CAnimationController()
{

}

void CAnimationController::SetSkeleton(SharePtr<CSkeleton> skeleton)
{
    m_pSkeleton = skeleton;
}

SharePtr<CAnimation> CAnimationController::GetCurrentAnimation()
{
    return m_pCurrentAnimation;
}

void CAnimationController::PlayAnimation(const SharePtr<CAnimation>& pAnimation, 
                                         float fBlendTime, bool bLoop)
{
    //TODO: Blend
    m_pCurrentAnimation = pAnimation;
    m_bLoop = bLoop;
    m_fPlayingTime = 0;
    m_bPlaying = true;
}

void CAnimationController::GoToFrame( size_t frame )
{
    if(m_pCurrentAnimation)
    {
        m_fPlayingTime = (float)frame / m_pCurrentAnimation->GetFPS();
        m_bPlaying = false;
        CalcDeltaMatrices();
    }
}

void CAnimationController::Pause()
{
    m_bPlaying = false;
}

void CAnimationController::Resume()
{
    if(m_pCurrentAnimation)
    {
        m_bPlaying = true;
    }
}

void CAnimationController::Stop()
{
    m_fPlayingTime = 0;
    CalcDeltaMatrices();
    m_bPlaying = false;
}

bool CAnimationController::IsPlaying()
{
    return m_bPlaying;
}

size_t CAnimationController::GetCurrFrame() const
{
    size_t uRet = 0xFFFFFFFF;
    if(m_pCurrentAnimation)
    {
        uRet = static_cast<size_t>(m_fPlayingTime * m_pCurrentAnimation->GetFPS());
    }
    return uRet;
}

float CAnimationController::GetPlayingTime() const
{
    return m_fPlayingTime;
}

void CAnimationController::Update(float fDeltaTime)
{
    if(!m_bPlaying) return;

    // if(frame changed and not in blend state)
    if(!m_pCurrentAnimation || !m_pSkeleton)
        return;


    m_fPlayingTime += fDeltaTime;
    if(m_fPlayingTime > m_pCurrentAnimation->Duration())
    {
        if(m_bLoop)
            m_fPlayingTime = fmod(m_fPlayingTime, m_pCurrentAnimation->Duration());
        else
            m_fPlayingTime = m_pCurrentAnimation->Duration() - 1.f/m_pCurrentAnimation->GetFPS();
    }
    // TODO: check loop logic. check if it is in blend state.
    CalcDeltaMatrices();
}

void CAnimationController::CalcDeltaMatrices()
{
    m_deltaMatrices.clear();
    const std::vector<ESkeletonBoneType>& bones = m_pCurrentAnimation->GetBones();
    const std::vector<kmMat4*>& boneMatrixOfFrame = 
        m_pCurrentAnimation->GetBoneMatrixByTime(m_fPlayingTime);
    for (size_t i = 0; i < bones.size(); ++i)
    {
        SharePtr<CSkeletonBone> pSkeletonBone = m_pSkeleton->GetSkeletonBone(bones[i]);
        const kmMat4& tPosMatrix = pSkeletonBone->GetTPosMatrix();
        kmMat4 posMatrixInverse;
        kmMat4Inverse(&posMatrixInverse,&tPosMatrix);
        kmMat4Multiply(&m_deltaMatrices[pSkeletonBone->GetBoneType()],boneMatrixOfFrame[i],&posMatrixInverse);
    }
}

const CAnimationController::BoneMatrixMap& CAnimationController::GetDeltaMatrices() const
{
    return m_deltaMatrices;
}
