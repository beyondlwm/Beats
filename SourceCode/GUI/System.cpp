#include "stdafx.h"
#include "System.h"
#include "Renderer/RendererBase.h"
#include "Window/Window.h"
#include "Render/RenderManager.h"

using namespace FCGUI;

System *System::m_pInstance = nullptr;

System::System()
    : _rootWindow(new Window)
{
    _rootWindow->SetRenderer(new RendererBase());
}

System::~System()
{
    delete _rootWindow;
}

void System::preRender()
{
    CRenderManager::GetInstance()->SetupVPMatrix(true);
}

void System::Render()
{
    RendererBase *renderer = _rootWindow->Renderer();
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
    kmVec2 pos;
    kmVec2Fill(&pos, 0.f, 0.f);
    _rootWindow->SetPos(pos);
    _rootWindow->SetSize(resolution);
}

kmVec2 System::GetResolution() const
{
    return _resolution;
}

Window *System::RootWindow() const
{
    return _rootWindow;
}

bool System::InjectMouseEvent( MouseEvent *)
{
    return true;
}

bool System::InjectKeyboardEvent( KeyboardEvent *)
{
    return true;
}
