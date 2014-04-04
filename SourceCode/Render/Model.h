#ifndef RENDER_MODEL_H__INCLUDE
#define RENDER_MODEL_H__INCLUDE

#include "RenderObject.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
#include "Material.h"
#include "Utility/BeatsUtility/SharePtr.h"

class CSkeleton;
class CSkin;
class CTexture;
class CAnimationController;
class CAnimation;
class CShaderProgram;

class CModel : public CComponentBase, public CRenderObject
{
    typedef CComponentBase super;
    DECLARE_REFLECT_GUID(CModel, 0x1547ABEC, CComponentBase)

public:
    CModel();
    CModel(CSerializer& serializer);
    virtual ~CModel();

    bool Init();
    void PlayAnimationById(long id, float fBlendTime, bool bLoop);
    void PlayAnimationByName(const char *name, float fBlendTime, bool bLoop);
    void AddTexture(SharePtr<CTexture> pTexture);
    CAnimationController* GetAnimationController();

    virtual void PreRender();
    virtual void Render();
    virtual void PostRender();

    void SetAnimaton(SharePtr<CAnimation> pAnimation);
    void SetSkeleton(SharePtr<CSkeleton> pSkeleton);
    SharePtr<CSkeleton> GetSkeleton() const;
    void SetSkin(SharePtr<CSkin> pSkin);

private:
    void RenderSkeleton();

private:
    bool m_bRenderSkeleton;
    CShaderProgram* m_pSkinProgram;
    CAnimationController* m_pAnimationController;
    SharePtr<CMaterial> m_pMaterial;
    SharePtr<CSkeleton> m_pSkeleton;
    SharePtr<CSkin> m_pSkin;
    std::vector<SharePtr<CTexture> > m_textures;
    std::map<long, SharePtr<CAnimation> > m_animations;
};

#endif