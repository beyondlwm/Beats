#ifndef ENGINECENTER_H__INCLUDE
#define ENGINECENTER_H__INCLUDE

class CEngineCenter
{
    BEATS_DECLARE_SINGLETON(CEngineCenter);
public:
    bool Initialize(size_t uWindowWidth = 0xFFFFFFFF, size_t uWindowHeight = 0xFFFFFFFF);
};

#endif