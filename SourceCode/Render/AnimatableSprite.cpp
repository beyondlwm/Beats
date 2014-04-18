#include "stdafx.h"
#include "AnimatableSprite.h"
#include "SpriteFrameBatchManager.h"

CAnimatableSprite::CAnimatableSprite(CTextureFrag *textureFrag)
    : CSprite(textureFrag)
    , m_currFrame(0)
{
    initAnimProp();
}

CAnimatableSprite::CAnimatableSprite(const TString &textureFragName)
    : CSprite(textureFragName)
    , m_currFrame(0)
{
    initAnimProp();
}

CAnimatableSprite::CAnimatableSprite(const TString &textureFragName, 
                                     const kmVec2 &size, const kmVec2 &origin)
    : CSprite(textureFragName, size, origin)
    , m_currFrame(0)
{
    initAnimProp();
}

CAnimatableSprite::CAnimatableSprite(const TString &textureFragName, 
                                     const kmVec2 &size)
    : CSprite(textureFragName, size)
    , m_currFrame(0)
{
    initAnimProp();
}

CAnimatableSprite::~CAnimatableSprite()
{
}

void CAnimatableSprite::initAnimProp()
{
    AddProp(PROP_CURR_FRAME, _T("Current Frame"), 
        [this](kmScalar currFrame){SetCurrFrame((size_t)currFrame);},
        [this](){return (kmScalar)CurrFrame();});
}

void CAnimatableSprite::SetFrames(const FrameList &frames)
{
    m_frames.assign(frames.begin(), frames.end());
}

void CAnimatableSprite::AddFrame(CTextureFrag *frame)
{
    m_frames.push_back(frame);
}

const CAnimatableSprite::FrameList &CAnimatableSprite::Frames() const
{
    return m_frames;
}

void CAnimatableSprite::SetCurrFrame(size_t currFrame)
{
    if(currFrame >= m_frames.size())
    {
        m_currFrame = m_frames.size() - 1;
    }
    else
    {
        m_currFrame = currFrame;
    }
}

size_t CAnimatableSprite::CurrFrame() const
{
    return m_currFrame;
}

void CAnimatableSprite::Render()
{
    if(m_frames.empty())
    {
        CSprite::Render();
    }
    else
    {
        kmMat4 transform;
        kmMat4Identity(&transform);

        BEATS_ASSERT(m_currFrame < m_frames.size());
        CSpriteFrameBatchManager *batchMgr = CSpriteFrameBatchManager::GetInstance();
        batchMgr->AddQuad(m_quad, m_frames[m_currFrame], transform);
    }
}
