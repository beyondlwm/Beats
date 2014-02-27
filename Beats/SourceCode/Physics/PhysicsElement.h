#ifndef BEATS_PHYSICS_PHYSICSELEMENT_H__INCLUDE
#define BEATS_PHYSICS_PHYSICSELEMENT_H__INCLUDE

#include "../../../Utility/Math/MathPublic.h"
#include "../../../Utility/Math/Vector3.h"

class CBaseObject;
class CPhysicsElement
{
public:
    CPhysicsElement(CBaseObject* pOwner = NULL);
    ~CPhysicsElement();

    CPhysicsElement& operator = (const CPhysicsElement& rightValue);

    void Update(float deltaTimeMs);
    void Attach(CPhysicsElement* pParent);
    void Detach();
    void DetachAllChildren();
    bool IsActive() const;
    void SetActive(bool active);

    void SetLocalPos(const CVector3& value);
    void SetLocalPos(float x, float y, float z);
    void SetLocalPos(float value, EVectorPos pos);
    const CVector3& GetLocalPos() const;

    void SetWorldPos(const CVector3& value);
    void SetWorldPos(float x, float y, float z);
    void SetWorldPos(float value, EVectorPos pos);
    const CVector3 GetWorldPos() const;

    CVector3& Translate(const CVector3& value);
    CVector3& Translate(float x, float y, float z);
    CVector3& Translate(float value, EVectorPos pos);

    void SetDirection(const CVector3& value);
    void SetDirection(float x, float y, float z);
    void SetDirection(float value, EVectorPos pos);
    const CVector3& GetDirection() const;

    void SetUpDirection(const CVector3& value);
    void SetUpDirection(float x, float y, float z);
    void SetUpDirection(float value, EVectorPos pos);
    const CVector3& GetUpDirection() const;

    void SetVelocity(const CVector3& value);
    void SetVelocity(float x, float y, float z);
    void SetVelocity(float value, EVectorPos pos);
    const CVector3& GetVelocity() const;

    void SetID(size_t uId);
    size_t GetID() const;

private:
    void AddChild(CPhysicsElement* pChild);
    void RemoveChild(CPhysicsElement* pChild);
    void SetParent(CPhysicsElement* pParent);

private:
    size_t m_uId;
    bool m_active;
    CPhysicsElement* m_pParent;
    CBaseObject* m_pOwner;
    //all in Ms
    CVector3 m_pos;
    CVector3 m_up;
    CVector3 m_direction;
    CVector3 m_velocity;
    CVector3 m_acceleration;
    std::set<CPhysicsElement*> m_children;
};

#endif