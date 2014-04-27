#ifndef RENDER_GAME_OBJECT_MANAGER_H__INCLUDE
#define RENDER_GAME_OBJECT_MANAGER_H__INCLUDE

class CGameObject;

class CGameObjectManager
{
public:
    CGameObjectManager();
    ~CGameObjectManager();

    CGameObject *CreateGameObject();
    void DestroyObject(CGameObject *object);

private:
    std::list<CGameObject *> m_objects;
};

#endif