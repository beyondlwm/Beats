#ifndef RENDER_ANIMATION_H__INCLUDE
#define RENDER_ANIMATION_H__INCLUDE

#include "resource/Resource.h"

struct kmMat4;

class CAnimation : public CResource
{
    DECLARE_RESOURCE_TYPE(eRT_Animation)
public:
    CAnimation();
    virtual ~CAnimation();
    
    virtual bool Load();
    virtual bool Unload();

    float Duration();
    const std::vector<ESkeletonBoneType>& GetBones() const;
    const std::vector<kmMat4*>& GetBoneMatrixByFrame(size_t uFrame) const;
    const std::vector<kmMat4*>& GetBoneMatrixByTime(float playtime) const;
    size_t GetFPS() const;
    const kmMat4*GetOneBoneMatrixByTime(float fTime, ESkeletonBoneType boneType) const;
    unsigned short GetFrameCount();
    void SetCurFrame(size_t curFrame);
    size_t GetCurFrame() const;
private:
    unsigned short m_uFrameCount;
    size_t m_uFPS;
    float m_fDuration;
    size_t m_uCurFrame;
    std::vector<ESkeletonBoneType> m_bones;
    std::vector<std::vector<kmMat4*>> m_posOfFrame; 
};

#endif