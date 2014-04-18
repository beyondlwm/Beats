#include "stdafx.h"
#include "ProgressRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
using namespace FCGUI;

ProgressRenderer::ProgressRenderer()
{

}

void  ProgressRenderer::AddLayer(const TString &fillTextName, const TString &backTextName, Progress::State state)
{
    Progress *process = static_cast<Progress *>(_window); 
    BaseRenderer::AddLayer(backTextName,state);

    Window *sub = process->GetSubWndFill();
    BaseRenderer* subrender = sub->Renderer();
    subrender->AddLayer(fillTextName);
}

void  ProgressRenderer::AddLayer(CTextureFrag* fillLayer, CTextureFrag *backLayer, Progress::State state)
{
    Progress *process = static_cast<Progress *>(_window); 
    BaseRenderer::AddLayer(backLayer,state);

    Window *sub = process->GetSubWndFill();
    BaseRenderer* subrender = sub->Renderer();
    subrender->AddLayer(fillLayer);
}
