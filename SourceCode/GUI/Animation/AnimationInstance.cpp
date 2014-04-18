#include "stdafx.h"
#include "AnimationInstance.h"
#include "AnimationManager_ui.h"
#include "Event/BaseEvent.h"

using namespace FCGUI;

AnimationInstance::AnimationInstance(Animation *animation, Animatable *target, 
                                     Animation::ReplayMode replayMode, 
                                     kmScalar speed, bool destroyOnStop)
    : _animation(animation)
    , _target(target)
    , _replayMode(replayMode == Animation::ReplayMode::DEFAULT ? _animation->Mode() : replayMode)
    , _speed(speed)
    , _position(0.f)
    , _backward(false)
    , _playing(false)
    , _destroyOnStop(destroyOnStop)
    , _destroyed(false)
{
    Stop();
}

AnimationInstance::~AnimationInstance()
{

}

void AnimationInstance::Update( kmScalar dt )
{
    if(_playing)
    {
        dt *= _speed;
        if(isReverse() != _backward)
            dt = -dt;
        kmScalar newposition = _position + dt;
        if(newposition > _animation->Duration())
        {
            if(isLoop())
            {
                _position = newposition - _animation->Duration();
                BaseEvent event(EVENT_LOOPED);
                DispatchEvent(&event);
            }
            else if(isBounce())
            {
                _position = _animation->Duration() - (newposition - _animation->Duration());
                _backward = !_backward;
                BaseEvent event(EVENT_BOUNCED);
                DispatchEvent(&event);
            }
            else
            {
                _position = _animation->Duration();
                if(_destroyOnStop)
                    _destroyed = true;
                BaseEvent event(EVENT_STOPED);
                DispatchEvent(&event);
            }
        }
        else if(newposition < 0.f)
        {
            if(isLoop())
            {
                _position = _animation->Duration() + newposition;
                BaseEvent event(EVENT_LOOPED);
                DispatchEvent(&event);
            }
            else if(isBounce())
            {
                _position = - newposition;
                _backward = !_backward;
                BaseEvent event(EVENT_BOUNCED);
                DispatchEvent(&event);
            }
            else
            {
                _position = 0.f;
                if(_destroyOnStop)
                    _destroyed = true;
                BaseEvent event(EVENT_STOPED);
                DispatchEvent(&event);
            }
        }
        else
        {
            _position = newposition;
        }

        _animation->Apply(_target, _position);
    }
}

void AnimationInstance::Play()
{
    Stop();
    Resume();
    BaseEvent event(EVENT_STARTED);
    DispatchEvent(&event);
}

void AnimationInstance::Pause()
{
    _playing = false;
    BaseEvent event(EVENT_PAUSED);
    DispatchEvent(&event);
}

void AnimationInstance::Resume()
{
    _playing = true;
    BaseEvent event(EVENT_RESUMED);
    DispatchEvent(&event);
}

void AnimationInstance::Stop()
{
    _playing = false;
    _position = isReverse() ? _animation->Duration() : 0.f;
    BaseEvent event(EVENT_STOPED);
    DispatchEvent(&event);
}

void AnimationInstance::Goto(kmScalar position)
{
    _position = fmodf(position, _animation->Duration());
}

void AnimationInstance::GotoAndPause(kmScalar position)
{
    Goto(position);
    Pause();
}

void AnimationInstance::GotoAndPlay(kmScalar position)
{
    Goto(position);
    Resume();
}

kmScalar AnimationInstance::Position() const
{
    return _position;
}

bool AnimationInstance::Destroyed() const
{
    return _destroyed;
}

bool AnimationInstance::isReverse() const
{
    return (static_cast<int>(_replayMode) & static_cast<int>(Animation::ReplayMode::REVERSE)) != 0;
}

bool AnimationInstance::isLoop() const
{
    return (static_cast<int>(_replayMode) & ~static_cast<int>(Animation::ReplayMode::REVERSE)) 
        == static_cast<int>(Animation::ReplayMode::LOOP);
}

bool AnimationInstance::isBounce() const
{
    return (static_cast<int>(_replayMode) & ~static_cast<int>(Animation::ReplayMode::REVERSE)) 
        == static_cast<int>(Animation::ReplayMode::BOUNCE);
}
