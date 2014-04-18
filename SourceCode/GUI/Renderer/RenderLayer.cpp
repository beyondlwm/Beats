#include "stdafx.h"
#include "RenderLayer.h"
#include "Render/TextureFragManager.h"

using namespace FCGUI;

FCGUI::RenderLayer::RenderLayer( const TString &textureFragName )
    : _currFrame(0)
{
    _frames.push_back(CTextureFragManager::GetInstance()->GetTextureFrag(textureFragName));
    initAnimProp();
}

FCGUI::RenderLayer::RenderLayer( CTextureFrag *textureFrag )
    : _currFrame(0)
{
    _frames.push_back(textureFrag);
    initAnimProp();
}

FCGUI::RenderLayer::RenderLayer( const FrameList &frames )
    : _currFrame(0)
{
    _frames.assign(frames.begin(), frames.end());
    initAnimProp();
}

FCGUI::RenderLayer::~RenderLayer()
{

}

void RenderLayer::initAnimProp()
{
    AddProp(PROP_CURR_FRAME, _T("Current Frame"), 
        [this](kmScalar currFrame){SetCurrFrame((size_t)currFrame);},
        [this](){return (kmScalar)CurrFrame();});
}

void RenderLayer::SetCurrFrame(size_t currFrame)
{
    if(currFrame >= _frames.size())
    {
        _currFrame = _frames.size() - 1;
    }
    else
    {
        _currFrame = currFrame;
    }
}

size_t RenderLayer::CurrFrame() const
{
    return _currFrame;
}

CTextureFrag *RenderLayer::GetTextureFrag() const
{
    CTextureFrag *frag = nullptr;
    if(_currFrame < _frames.size())
    {
        frag = _frames[_currFrame];
    }
    return frag;
}