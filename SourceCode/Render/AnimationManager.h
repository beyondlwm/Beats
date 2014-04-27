#ifndef RENDER_ANIMATIONMANAGER_H__INCLUDE
#define RENDER_ANIMATIONMANAGER_H__INCLUDE

class CBaseAnimationController;
class CAnimationController;
class CSpriteAnimationController;

class CAnimationManager
{
    BEATS_DECLARE_SINGLETON(CAnimationManager);
public:
    CAnimationController *CreateSkelAnimationController();
    void DeleteController(CBaseAnimationController *controller);

    void Update(float deltaTime);

private:
    std::list<CBaseAnimationController *> m_controllers;
};

#endif