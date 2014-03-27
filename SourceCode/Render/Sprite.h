#ifndef RENDER_SPRITE_H__INCLUDE
#define RENDER_SPRITE_H__INCLUDE

#include "RenderObject.h"
#include "CommonTypes.h"
#include "Geometry.h"

class CTexture;
class CSpriteFrame;
class CSpriteAnimation;
class CSpriteAnimationController;

class CSprite : public CRenderObject
{
public:
    CSprite();
    CSprite(SharePtr<CTexture> texture, const CRect &rect, const CSize &size);
    ~CSprite();

    virtual void PreRender() override;

    virtual void Render() override;

    virtual void PostRender() override;

    void AddAnimation(SharePtr<CSpriteAnimation> animation);

    void PlayAnimation(const TString &name, bool loop);

private:
    CSpriteAnimationController *animController();

private:
    CSpriteFrame *m_frame;
    std::map<TString, SharePtr<CSpriteAnimation> > m_animations;
    CSpriteAnimationController *m_animController;
};

#endif