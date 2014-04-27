#include "stdafx.h"
#include "SpriteAnimationController.h"
#include "SpriteAnimation.h"

CSpriteAnimationController::CSpriteAnimationController():
    m_elapsed(0.f), m_loop(false), m_playing(false)
{

}

CSpriteAnimationController::~CSpriteAnimationController()
{

}

void CSpriteAnimationController::PlayAnimation( SharePtr<CSpriteAnimation> animation, bool loop )
{
    m_currAnimation = animation;
    m_elapsed = 0.f;
    m_loop = loop;
    m_playing = true;
}

void CSpriteAnimationController::Update( float deltaTime )
{
    if(!m_playing) return;
    if(!m_currAnimation || m_currAnimation->Frames().size() == 0) return;

    m_elapsed += deltaTime;
    if(m_loop)
    {
        m_elapsed = fmodf(m_elapsed, m_currAnimation->Duration());
    }
    else if(m_elapsed > m_currAnimation->Duration())
    {
        m_elapsed = 0;
        m_playing = false;
    }
}

CSpriteFrame * CSpriteAnimationController::GetCurrFrame() const
{
    if(!m_currAnimation || m_currAnimation->Frames().size() == 0)
        return nullptr;

    size_t currFrmIndex = static_cast<size_t>(m_elapsed / m_currAnimation->FrameInterval());
    auto frames = m_currAnimation->Frames();
    BEATS_ASSERT(currFrmIndex < frames.size());
    return frames[currFrmIndex];
}
