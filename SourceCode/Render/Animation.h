#ifndef RENDER_ANIMATION_H__INCLUDE
#define RENDER_ANIMATION_H__INCLUDE

#include "resource/Resource.h"

struct kmMat4;

class CAnimation : public CResource
{
public:
    CAnimation();
    virtual ~CAnimation();
    
    virtual EResourceType GetType();
    virtual bool Load();

    virtual bool Unload();

    float Duration();
    const std::vector<ESkeletonBoneType>& GetBones() const;
    const std::vector<kmMat4*>& GetBoneMatrixByFrame(size_t uFrame) const;
    const std::vector<kmMat4*>& GetBoneMatrixByTime(float playtime) const;
    size_t GetFPS() const;
    const kmMat4*GetOneBoneMatrixByTime(float fTime, ESkeletonBoneType boneType) const;
    unsigned short GetFrameCount();
private:
    unsigned short m_uFrameCount;
    size_t m_uFPS;
    float m_fDuration;
    std::vector<ESkeletonBoneType> m_bones;
    std::vector<std::vector<kmMat4*>> m_posOfFrame; 
};

#endif