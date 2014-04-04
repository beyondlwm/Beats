#include "stdafx.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderManager.h"
#include "SpriteFrame.h"
#include "SpriteFrameBatchManager.h"
#include "SpriteAnimationController.h"
#include "SpriteAnimation.h"
#include "AnimationManager.h"

CSprite::CSprite():
    m_frame(nullptr),
    m_animController(nullptr)
{

}

CSprite::CSprite(CTextureFrag *textureFrag)
    : m_animController(nullptr)
{
    m_frame = new CSpriteFrame(textureFrag);
}

CSprite::CSprite(const TString &textureFragName, const kmVec2 &size)
    : m_animController(nullptr)
{
    kmVec2 point;
    kmVec2Fill(&point, 0.f, 0.f);
    m_frame = new CSpriteFrame(textureFragName, size, point);
}

CSprite::~CSprite()
{
    delete m_frame;
}

void CSprite::PreRender()
{
}

void CSprite::Render()
{
    kmMat4 transform;
    kmMat4Identity(&transform);

    CSpriteFrameBatchManager *batchMgr = CSpriteFrameBatchManager::GetInstance();
    if(m_animController)
    {
        batchMgr->AddSpriteFrame(m_animController->GetCurrFrame(), transform);
    }
    else if(m_frame)
    {
        batchMgr->AddSpriteFrame(m_frame, transform);
    }
}

void CSprite::PostRender()
{
}

void CSprite::AddAnimation( SharePtr<CSpriteAnimation> animation )
{
    m_animations[animation->Name()] = animation;
}

void CSprite::PlayAnimation( const TString &name, bool loop )
{
    auto itr = m_animations.find(name);    
    BEATS_ASSERT(itr != m_animations.end());
    animController()->PlayAnimation(itr->second, loop);
}

CSpriteAnimationController * CSprite::animController()
{
    if(!m_animController)
    {
        m_animController = CAnimationManager::GetInstance()->CreateSpriteAnimationController();
    }
    return m_animController;
}
