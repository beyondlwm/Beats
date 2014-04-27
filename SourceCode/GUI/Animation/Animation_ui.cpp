#include "stdafx.h"
#include "Animation_ui.h"
#include "AnimationLayer.h"

using namespace FCGUI;

Animation::Animation(const TString &name)
    : _replayMode(ReplayMode::ONCE)
    , _name(name)
    , _duration(0.f)
{

}

Animation::~Animation()
{
    for(auto layer : _layers)
    {
        BEATS_SAFE_DELETE(layer);
    }
}

TString Animation::Name() const
{
    return _name;
}

void Animation::SetDuration(kmScalar duration)
{
    _duration = duration;
}

kmScalar Animation::Duration() const
{
    return _duration;
}

void Animation::SetMode(ReplayMode replayMode)
{
    _replayMode = replayMode;
}

Animation::ReplayMode Animation::Mode() const
{
    return _replayMode;
}

AnimationLayer *Animation::CreateLayer(const TString &name)
{
    AnimationLayer *layer = new AnimationLayer(name);
    layer->SetAnimation(this);
    _layers.push_back(layer);
    return layer;
}

void Animation::DeleteLayer(AnimationLayer *layer)
{
    for(size_t i = 0; i < _layers.size(); ++i)
    {
        if(_layers[i] == layer)
        {
            _layers.erase(_layers.begin() + i);
            BEATS_SAFE_DELETE(layer);
            break;
        }
    }
}

void Animation::Apply( Animatable *target, kmScalar position )
{
    if(position < 0.f)
        position = 0.f;
    if(position > _duration)
        position = _duration;

    for(auto layer : _layers)
    {
        layer->Apply(target, position);
    }
}

