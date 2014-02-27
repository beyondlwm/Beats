#ifndef BEATS_PHYSICS_PHYSICSCONTROLLER_H__INCLUDE
#define BEATS_PHYSICS_PHYSICSCONTROLLER_H__INCLUDE

#include "../../../Utility/Math/MathPublic.h"
#include "../../../Utility/Math/Vector3.h"
#include "../Control/InputPublicDef.h"
class CPhysicsElement;

enum EControlAction
{
    eCA_Forward,
    eCA_Back,
    eCA_MoveLeft,
    eCA_MoveRight,
    eCA_TurnLeft,
    eCA_TurnRight,
    eCA_LookUp,
    eCA_LookDown,

    eCA_Count,
    eCA_ForceTo32Bit = 0xffffffff
};

class CPhysicsController
{
public:
    CPhysicsController(CPhysicsElement* pElement);
    ~CPhysicsController();

    void ChangeValue(float value, EVectorPos pos);

private:
    void MoveForward();
    void MoveBack();

    void MoveLeft();
    void MoveRight();

    void MoveUp();
    void MoveDown();

private:
    float m_moveSpeed;
    CPhysicsElement* m_pElement;
    std::vector<SKeyTriggerType> m_triggers;
};

#endif