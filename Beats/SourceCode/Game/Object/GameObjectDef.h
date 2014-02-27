#ifndef BEATS_GAME_OBJECT_GAMEOBJECTDEF_H__INCLUDE
#define BEATS_GAME_OBJECT_GAMEOBJECTDEF_H__INCLUDE

enum EGameObjectType
{
    eGOT_Beat,

    eGOT_Count,
    eGOT_Force32Bit = 0xffffffff
};

struct SPosition
{
    float x;
    float y;
    float z;
    float rotate;

    SPosition(float x1 = 0, float y1 = 0, float z1 = 0, float rotate1 = 0)
    {
        x = x1; y = y1; z = z1; rotate = rotate1;
    }

    const SPosition& operator += (const SPosition& pos)
    {
        x += pos.x;
        y += pos.y;
        z += pos.z;
        rotate += pos.rotate;
        return *this;
    }
};


#define GAME_OBJECT_RESERVE_COUNT 300
#endif