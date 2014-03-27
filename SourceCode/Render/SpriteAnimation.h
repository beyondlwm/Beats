#ifndef RENDER_SPRITE_ANIMATION_H__INCLUDE
#define RENDER_SPRITE_ANIMATION_H__INCLUDE
#include "Resource/Resource.h"

class CSpriteFrame;
class CSpriteAnimation : public CResource
{
public:
    CSpriteAnimation(const TString &name);
    ~CSpriteAnimation();

    virtual EResourceType GetType() override;
    virtual bool Load() override;
    virtual bool Unload() override;

    void SetFrames(std::vector<CSpriteFrame *> frames);
    void SetDuration(float duration);
    void SetFrameInterval(float frameInterval);

    const TString &Name() const;
    const std::vector<CSpriteFrame *> &Frames() const;
    float Duration() const;
    float FrameInterval() const;

private:
    TString m_name;
    std::vector<CSpriteFrame *> m_frames;
    float m_duration;
    float m_frameInterval;
};

#endif