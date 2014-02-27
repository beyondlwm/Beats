#ifndef BEATS_UI_UISYSTEM_UISYSTEMBASE_H__INCLUDE
#define BEATS_UI_UISYSTEM_UISYSTEMBASE_H__INCLUDE

class CUISystemBase
{
public:
    CUISystemBase();
    virtual ~CUISystemBase();

public:
    virtual void Init() = 0;
    virtual void Update() = 0;
};


#endif