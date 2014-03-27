#ifndef RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#define RENDER_ANIMATIONCONTROLLER_H__INCLUDE
#include "BaseAnimationController.h"
class CAnimation;
class CSkeleton;

class CAnimationController : public CBaseAnimationController
{
public:
    typedef std::map<ESkeletonBoneType, kmMat4> BoneMatrixMap;

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

protected:
    void CalcDeltaMatrices();

private:
    bool m_bLoop;
    bool m_bPlaying;
    float m_fPlayingTime;
    float m_bBlendTime;
    SharePtr<CAnimation> m_pCurrentAnimation;
    SharePtr<CAnimation> m_pBlendDstAnimation;
    SharePtr<CSkeleton> m_pSkeleton;

    // this matrix convert Tpose skeleton to current frame pos.
    BoneMatrixMap m_deltaMatrices; 
};

#endif