#include "stdafx.h"
#include "SpriteAnimation.h"
#include "SpriteFrame.h"
#include <algorithm>

CSpriteAnimation::CSpriteAnimation( const TString &name ):
    m_name(name), m_duration(0.f), m_frameInterval(0.f)
{

}

CSpriteAnimation::~CSpriteAnimation()
{
    for(auto frame : m_frames)
    {
        delete frame;
    }
}

void CSpriteAnimation::SetFrames( std::vector<CSpriteFrame *> frames )
{
    m_frames = frames;
}

void CSpriteAnimation::SetDuration( float duration )
{
    m_duration = duration;
    if(!m_frames.empty())
    {
        m_frameInterval = m_duration / m_frames.size();
    }
}

void CSpriteAnimation::SetFrameInterval( float frameInterval )
{
    m_frameInterval = frameInterval;
    if(!m_frames.empty())
    {
        m_duration = m_frameInterval * m_frames.size();
    }
}

const TString & CSpriteAnimation::Name() const
{
    return m_name;
}

const std::vector<CSpriteFrame *> & CSpriteAnimation::Frames() const
{
    return m_frames;
}

float CSpriteAnimation::Duration() const
{
    return m_duration;
}

float CSpriteAnimation::FrameInterval() const
{
    return m_frameInterval;
}

EResourceType CSpriteAnimation::GetType()
{
    return eRT_SpriteAnimation;
}

bool CSpriteAnimation::Load()
{
    SetLoadedFlag(true);
    return true;
}

bool CSpriteAnimation::Unload()
{
    SetLoadedFlag(false);
    return true;
}
