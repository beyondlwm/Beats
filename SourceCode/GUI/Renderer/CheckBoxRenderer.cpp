#include "stdafx.h"
#include "CheckBoxRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
using namespace FCGUI;

CheckBoxRenderer::CheckBoxRenderer()
{
 
}

void CheckBoxRenderer::AddLayer(const TString &contentTextureName, const TString &indicatorTextureName, CheckBox::State state)
{
     CheckBox *check = static_cast<CheckBox *>(_window); 
    
     BaseRenderer::AddLayer(indicatorTextureName,state);

     Window *sub = check->GetSubWindow();
     BaseRenderer* checkrender = sub->Renderer();
     unsigned int contentState = Window::STATE_NORMAL;
     if( state == CheckBox::STATE_DISABLED_CHECKED || state == CheckBox::STATE_DISABLED_UNCHECKED )
     {
         contentState = Window::STATE_DISABLED;
     }
     checkrender->AddLayer(contentTextureName,contentState);
}

void CheckBoxRenderer::AddLayer(CTextureFrag *contentLayer, CTextureFrag* indicatorLayer, CheckBox::State state)
{
    CheckBox *check = static_cast<CheckBox *>(_window);
    
    BaseRenderer::AddLayer(indicatorLayer,state);

    Window *sub = check->GetSubWindow();
    BaseRenderer* checkrender =sub->Renderer();
    unsigned int contentState = Window::STATE_NORMAL;
    if( state == CheckBox::STATE_DISABLED_CHECKED || state == CheckBox::STATE_DISABLED_UNCHECKED )
    {
        contentState = Window::STATE_DISABLED;
    }
    checkrender->AddLayer(contentLayer,contentState);
}


