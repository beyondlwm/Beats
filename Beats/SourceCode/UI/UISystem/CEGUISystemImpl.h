#ifndef BEATS_UI_UISYSTEM_CEGUISYSTEMIMPL_H__INCLUDE
#define BEATS_UI_UISYSTEM_CEGUISYSTEMIMPL_H__INCLUDE

#include "UISystemBase.h"
#include "CEGUIEventArgs.h"
#include <utility>

namespace CEGUI
{
    class Direct3D9Renderer;
}

class CCEGUISystemImpl : public CUISystemBase
{
public:
    CCEGUISystemImpl();
    virtual ~CCEGUISystemImpl();

public:
    virtual void Init();
    virtual void Update();

private:
    void InitResource();
    void InitResourceGroup();
    void UpdateMouseInput();
    void UpdateKeyboardInput();
    void RestoreRenderState();

private:
    CEGUI::Direct3D9Renderer* m_pRenderer;

};


#endif