#include "stdafx.h"
#include "AnimationManager_ui.h"
#include "AnimationInstance.h"

using namespace FCGUI;

AnimationManager *AnimationManager::m_pInstance = nullptr;

AnimationManager::AnimationManager()
{

}

AnimationManager::~AnimationManager()
{
    for(auto instance : _instances)
    {
        BEATS_SAFE_DELETE(instance);
    }
    for(auto animation : _animations)
    {
        BEATS_SAFE_DELETE(animation.second);
    }
}

void AnimationManager::Update( kmScalar dt )
{
    for(auto itr = _instances.begin(); itr != _instances.end();)
    {
        if((*itr)->Destroyed())
        {
            BEATS_SAFE_DELETE(*itr);
            itr = _instances.erase(itr);
        }
        else
        {
            (*itr)->Update(dt);
            ++itr;
        }
    }
}

Animation *AnimationManager::CreateAnimation(const TString &name)
{
    BEATS_ASSERT(_animations.find(name) == _animations.end(), 
        _T("Animation with name:%s already existed"), name.c_str());
    Animation *animation = new Animation(name);
    _animations[name] = animation;
    return animation;
}

void AnimationManager::DeleteAnimation(const TString &name)
{
    auto itr = _animations.find(name);
    if(itr != _animations.end())
    {
        BEATS_SAFE_DELETE(itr->second);
        _animations.erase(itr);
    }
}

AnimationInstance *AnimationManager::InstantiateAnimation(const TString &name, 
                                                          Animatable *target,
                                                          Animation::ReplayMode replayMode, 
                                                          kmScalar speed,
                                                          bool destroyOnStop)
{
    AnimationInstance *instance = nullptr;
    auto itr = _animations.find(name);
    if(itr != _animations.end())
    {
        instance = new AnimationInstance(itr->second, target, replayMode, speed, destroyOnStop);
        _instances.push_back(instance);
    }
    return instance;
}

void AnimationManager::DestroyInstance(AnimationInstance *instance)
{
    auto itr = std::find(_instances.begin(), _instances.end(), instance);
    if(itr != _instances.end())
    {
        _instances.erase(itr);
    }
    BEATS_SAFE_DELETE(instance);
}

