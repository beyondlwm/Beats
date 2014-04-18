#include "stdafx.h"
#include "AnimationManager.h"
#include "AnimationController.h"

CAnimationManager *CAnimationManager::m_pInstance = nullptr;

CAnimationManager::CAnimationManager()
{

}

CAnimationManager::~CAnimationManager()
{
    for(auto controller : m_controllers)
    {
        BEATS_SAFE_DELETE(controller);
    }
}

CAnimationController * CAnimationManager::CreateSkelAnimationController()
{
    CAnimationController *controller = new CAnimationController();
    m_controllers.push_back(controller);
    return controller;
}

void CAnimationManager::Update( float deltaTime )
{
    for(auto controller : m_controllers)
    {
        controller->Update(deltaTime);
    }
}

void CAnimationManager::DeleteController( CBaseAnimationController *controller )
{
    auto itr = find(m_controllers.begin(), m_controllers.end(), controller);
    if(itr != m_controllers.end())
    {
        delete controller;
        m_controllers.erase(itr);
    }
}
