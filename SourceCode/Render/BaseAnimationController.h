#ifndef RENDER_BASE_ANIMATION_CONTROLLER_H__INCLUDE
#define RENDER_BASE_ANIMATION_CONTROLLER_H__INCLUDE

class CBaseAnimationController
{
public:
    virtual ~CBaseAnimationController(){}
    virtual void Update(float deltaTime) = 0;
};

#endif