#ifndef RENDER_SKELETON_H__INCLUDE
#define RENDER_SKELETON_H__INCLUDE

#include "resource/Resource.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
class CSkeletonBone;

class CSkeleton : public CComponentBase, public CResource
{
    typedef CComponentBase super;
    DECLARE_REFLECT_GUID(CSkeleton, 0x1147A0EF, CComponentBase)
    DECLARE_RESOURCE_TYPE(eRT_Skeleton)

public:
    CSkeleton();
    CSkeleton(CSerializer& serializer);
    virtual ~CSkeleton();

    virtual bool Load();
    virtual bool Unload();

    SharePtr<CSkeletonBone> GetSkeletonBone(ESkeletonBoneType type) const;
    std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>>& GetBoneMap();
private:
    std::vector<size_t> m_uTestComponentValue;
    std::map<ESkeletonBoneType, SharePtr<CSkeletonBone>> m_bonesMap;
};

#endif