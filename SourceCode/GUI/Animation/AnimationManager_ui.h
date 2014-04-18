#ifndef GUI_ANIMATION_ANIMATION_MANAGER_H__INCLUDE
#define GUI_ANIMATION_ANIMATION_MANAGER_H__INCLUDE

#include "Animation_ui.h"

namespace FCGUI
{
    class AnimationInstance;
    class Animatable;

    class AnimationManager
    {
        BEATS_DECLARE_SINGLETON(AnimationManager);
    public:
        void Update(kmScalar dt);

        Animation *CreateAnimation(const TString &name);
        void DeleteAnimation(const TString &name);

        AnimationInstance *InstantiateAnimation(const TString &name, Animatable *target,
            Animation::ReplayMode replayMode = Animation::ReplayMode::DEFAULT, 
            kmScalar speed = 1.f, bool destroyOnStop = false);
        void DestroyInstance(AnimationInstance *instance);

    private:
        std::map<TString, Animation *> _animations;
        std::list<AnimationInstance *> _instances;
    };

}

#endif // !GUI_ANIMATION_ANIMATION_MANAGER_H__INCLUDE
