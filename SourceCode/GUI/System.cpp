#include "stdafx.h"
#include "System.h"
#include "Renderer/BaseRenderer.h"
#include "Window/Window.h"
#include "Render/RenderManager.h"
#include "Event/MouseEvent.h"
#include "WindowManager.h"
#include "Render/Camera.h"
#include "GUI/Font/FontManager.h"
using namespace FCGUI;

System *System::m_pInstance = nullptr;

System::System()
{
}

System::~System()
{
    WindowManager::Destroy();
}

void System::Update(float deltaTime)
{
    WindowManager::GetInstance()->RootWindow()->Update(deltaTime);
}

void System::preRender()
{
}

void System::Render()
{
    BaseRenderer *renderer = WindowManager::GetInstance()->RootWindow()->Renderer();
    if(renderer)
    {
        kmMat4 transform;
        kmMat4Identity(&transform);

        preRender();
        renderer->Render(transform);
        postRender();
    }
}

void System::postRender()
{

}

void System::OnResolutionChanged( kmVec2 resolution )
{
    _resolution = resolution;
    WindowManager::GetInstance()->RootWindow()->SetPos(0.f, 0.f);
    WindowManager::GetInstance()->RootWindow()->SetSize(resolution);
}

void System::OnResolutionChanged(kmScalar width, kmScalar height)
{
    kmVec2 resolution;
    kmVec2Fill(&resolution, width, height);
    OnResolutionChanged(resolution);
}

kmVec2 System::GetResolution() const
{
    return _resolution;
}

bool System::InjectMouseEvent( MouseEvent *event)
{
    return WindowManager::GetInstance()->OnMouseEvent(event);
}

bool System::InjectKeyboardEvent( KeyboardEvent *event)
{
    return WindowManager::GetInstance()->OnKeyboardEvent(event);
}

void System::InitFontFace()
{
    FontManager::GetInstance()->CreateFace(_T("STFANGSO_12"), _T("STFANGSO.TTF") ,12);
}
