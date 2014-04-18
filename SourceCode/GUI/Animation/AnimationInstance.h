#ifndef GUI_ANIMATION_ANIMATION_INSTANCE_H__INCLUDE
#define GUI_ANIMATION_ANIMATION_INSTANCE_H__INCLUDE

#include "Animation_ui.h"
#include "Event/EventDispatcher.h"
#include "Event/EventType.h"

namespace FCGUI
{
    class Animation;

    class AnimationInstance : public EventDispatcher
    {
    public:
        enum EventType
        {
            EVENT_ANIMATION = ::EVENT_ANIMATION,
            EVENT_STARTED,
            EVENT_STOPED,
            EVENT_PAUSED,
            EVENT_RESUMED,
            EVENT_LOOPED,
            EVENT_BOUNCED,
        };

        AnimationInstance(Animation *animation, Animatable *target, 
            Animation::ReplayMode replayMode = Animation::ReplayMode::DEFAULT, 
            kmScalar speed = 1.0f, bool destroyOnStop = false);
        ~AnimationInstance();

        void Update(kmScalar dt);

        void Play();
        void Pause();
        void Resume();
        void Stop();
        void Goto(kmScalar position);
        void GotoAndPlay(kmScalar position);
        void GotoAndPause(kmScalar position);

        kmScalar Position() const;

        bool Destroyed() const;

    private:
        bool isReverse() const;
        bool isLoop() const;
        bool isBounce() const;

    private:
        Animation *_animation;
        Animatable *_target;
        Animation::ReplayMode _replayMode;
        kmScalar _speed;
        kmScalar _position;
        bool _backward;
        bool _playing;
        bool _destroyOnStop;
        bool _destroyed;
    };

}

#endif // !GUI_ANIMATION_ANIMATION_INSTANCE_H__INCLUDE
