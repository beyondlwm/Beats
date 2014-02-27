#ifndef BEATS_GAME_OBJECT_BASEOBJECT_H__INCLUDE
#define BEATS_GAME_OBJECT_BASEOBJECT_H__INCLUDE

#define INVALID_ID 0xFFFFFFFF
//call recycle instead of deleting CBaseObject 
//because we use cache pool in CObjectFactory,otherwise you may get low efficiency.

#include "GameObjectDef.h"

class CPhysicsElement;
class CRenderUnit;

class CBaseObject
{
public:
    CBaseObject(EGameObjectType type, long id = INVALID_ID);
    virtual ~CBaseObject();

    EGameObjectType GetType();
    bool IsActive();
    void SetActive( bool active );
    void SetRecycleFlag(bool recycle);
    bool GetRecycleFlag();
    unsigned long GetID();
    void SetID(unsigned long id);

    CPhysicsElement* GetPhysics() const;
    CRenderUnit* GetGraphics() const;

    virtual void Init();
    virtual void UnInit();
    virtual void Update(float timeDeltaMs);

    //don't call it out of factory.
    virtual void OnReCreate();
    virtual void OnRecycle();

protected:
    long m_id;
    bool m_active;
    bool m_recycleFlag;
    float m_rotate;
    EGameObjectType m_type;
    CBaseObject* m_pParent;
    CPhysicsElement* m_pPhysics;
    CRenderUnit* m_pGraphics;

};

#endif