#ifndef RENDER_ANIMATABLE_SPRITE_H__INCLUDE
#define RENDER_ANIMATABLE_SPRITE_H__INCLUDE

#include "Sprite.h"
#include "GUI/Animation/Animatable.h"

class CAnimatableSprite : public CSprite, public FCGUI::Animatable
{
public:
    typedef std::vector<CTextureFrag *> FrameList;

    enum Property
    {
        PROP_CURR_FRAME,
    };

    CAnimatableSprite(CTextureFrag *textureFrag);
    CAnimatableSprite(const TString &textureFragName);
    CAnimatableSprite(const TString &textureFragName, const kmVec2 &size);
    CAnimatableSprite(const TString &textureFragName, const kmVec2 &size, const kmVec2 &origin);
    virtual ~CAnimatableSprite();

    void SetFrames(const FrameList &frames);
    void AddFrame(CTextureFrag *frame);
    const FrameList &Frames() const;
    void SetCurrFrame(size_t currFrame);
    size_t CurrFrame() const;

    virtual void Render() override;

private:
    void initAnimProp();

private:
    FrameList m_frames;
    size_t m_currFrame;
};

#endif