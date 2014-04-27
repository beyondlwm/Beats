#ifndef RENDER_SPRITE_ANIMATION_CONTROLLER_H__INCLUDE
#define RENDER_SPRITE_ANIMATION_CONTROLLER_H__INCLUDE
#include "BaseAnimationController.h"

class CSpriteAnimation;
class CSpriteFrame;
class CSpriteAnimationController : public CBaseAnimationController
{
public:
    CSpriteAnimationController();
    ~CSpriteAnimationController();

    void PlayAnimation(SharePtr<CSpriteAnimation> animation, bool loop);

    virtual void Update( float deltaTime ) override;

    CSpriteFrame *GetCurrFrame() const;

private:
    SharePtr<CSpriteAnimation> m_currAnimation;
    float m_elapsed;
    bool m_loop;
    bool m_playing;
};

#endif