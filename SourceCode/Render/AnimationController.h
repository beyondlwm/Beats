#ifndef RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#define RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#include "BaseAnimationController.h"
class CAnimation;
class CSkeleton;
class CSkeletonBone;

class CAnimationController : public CBaseAnimationController
{
public:

    typedef std::map<ESkeletonBoneType, kmMat4> BoneMatrixMap;
    typedef std::vector<kmMat4>                 BoneMatrix;

    CAnimationController();
    virtual ~CAnimationController();

    void SetSkeleton(SharePtr<CSkeleton> skeleton);
    SharePtr<CAnimation> GetCurrentAnimation();
    void PlayAnimation(const SharePtr<CAnimation>& pAnimation, float fBlendTime, bool bLoop);
    void GoToFrame(size_t frame);
    void Pause();
    void Resume();
    void Stop();
    bool IsPlaying();
    size_t GetCurrFrame() const;
    float GetPlayingTime() const;
    void Update(float fDeltaTime) override;
    const BoneMatrixMap& GetDeltaMatrices() const;

    const BoneMatrixMap& GetInitWorldTransform() const;
    const BoneMatrixMap& GetCurWorldTransform() const;

    void CalcDeltaMatrices();

    void  GetBoneInitWorldTM(kmMat4& transform, const SharePtr<CSkeletonBone> pTposSkeletonBone);
    void  GetBoneCurWorldTM(kmMat4&  transform, const ESkeletonBoneType boneType);
    void  BlendAnimation(const SharePtr<CAnimation> pBeforeAnimation, const SharePtr<CAnimation> pAfterAnimation);
    const kmMat4*  GetBoneTM(ESkeletonBoneType boneType);
    size_t GetPlayingFrame();
private:
    void  BlendData(const std::vector<kmMat4*>& startBones, const std::vector<kmMat4*>& endBones);
    void  Interpolation(const kmMat4& startMat, const kmMat4& endMat,CAnimationController::BoneMatrix& insertMat);
    void  Slerp( kmQuaternion* pOut,const kmQuaternion* q1,const kmQuaternion* q2,kmScalar t);
    void  ClearBlendData();
    bool  CheckBlend();
    void  CalcInitBoneWorldTM();

private:
    bool m_bLoop;
    size_t m_uLoopCount;
    bool m_bPlaying;
    float m_fPlayingTime;
    float m_bBlendTime;
    SharePtr<CAnimation> m_pCurrentAnimation;
    SharePtr<CAnimation> m_pPreAnimation;
    SharePtr<CSkeleton> m_pSkeleton;

    // this matrix convert Tpose skeleton to current frame pos.
    BoneMatrixMap m_deltaMatrices;

    //this matrix convert  bone space to world space
    BoneMatrixMap m_initBoneWorldTM;
    BoneMatrixMap m_curBoneWorldTM;
    std::vector<BoneMatrix> m_frameMatOfBone;
};

#endif