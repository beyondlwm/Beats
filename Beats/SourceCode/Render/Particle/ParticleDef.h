#ifndef BEATS_RENDER_PARTICLE_PARTICLEDEF_H__INCLUDE
#define BEATS_RENDER_PARTICLE_PARTICLEDEF_H__INCLUDE

#define INFINITE_VALUE -1
#define INVALID_TEXTURE_INDEX -1
#define INVALID_ID 0xFFFFFFFF

enum EParticleType
{
    ePT_Visual,
    ePT_Emitter,

    ePT_Count,
    ePT_ForceTo32Bit = 0xffffffff
};

#endif