#include "stdafx.h"
#include "AnimationLayer.h"
#include "KeyFrame.h"
#include "Animatable.h"
#include "AnimProp.h"
#include "Animation_ui.h"

using namespace FCGUI;

AnimationLayer::AnimationLayer(const TString &name)
    : _name(name)
    , _animation(nullptr)
    , _enabled(true)
{

}

AnimationLayer::~AnimationLayer()
{
    for(auto keyFrame : _keyFrames)
    {
        BEATS_SAFE_DELETE(keyFrame);
    }
}

void AnimationLayer::Apply(Animatable *target, kmScalar position)
{
    if(_enabled && !_keyFrames.empty())
    {
        KeyFrame *left = nullptr;
        KeyFrame *right = _keyFrames.front();

        for(size_t i = 0; i < _keyFrames.size(); ++i)
        {
            KeyFrame *curr = _keyFrames[i];
            if(position > curr->Position())
            {
                left = curr;
                if((i + 1) < _keyFrames.size())
                {
                    right = _keyFrames[i + 1];
                    if(position <= right->Position())
                        break;
                }
                else
                    right = nullptr;
            }
        }

        KeyFrame::ValueList values = interpolation(left, right, position);
        BEATS_ASSERT(values.size() == _propIDs.size());
        for (size_t i = 0; i < _propIDs.size(); i++)
        {
            const AnimProp *prop = target->GetProp(_propIDs[i]);
            BEATS_ASSERT(prop);
            prop->setter(values[i]);
        }
    }
}

KeyFrame::ValueList AnimationLayer::interpolation(const KeyFrame *left, const KeyFrame *right, 
                                                  kmScalar position)
{
    KeyFrame::ValueList values;
    if(!left && right)
    {
        BEATS_ASSERT(right->ValueCount() == _propIDs.size());
        values = right->Values();
    }
    else if(!right && left)
    {
        BEATS_ASSERT(left->ValueCount() == _propIDs.size());
        values = left->Values();
    }
    else if(left && right)
    {
        BEATS_ASSERT(left->ValueCount() == _propIDs.size());
        BEATS_ASSERT(right->ValueCount() == _propIDs.size());
        kmScalar percent = (position - left->Position()) / (right->Position() - left->Position());
        for(size_t i = 0; i < left->ValueCount(); ++i)
        {
            kmScalar value = left->Value(i) * (1 - percent) + right->Value(i) * percent;
            values.push_back(value);
        }
    }
    return values;
}

void AnimationLayer::SetAnimation(Animation *animation)
{
    _animation = animation;
}

void AnimationLayer::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

bool AnimationLayer::Enabled() const
{
    return _enabled;
}

void AnimationLayer::AddPropID(int propID)
{
    _propIDs.push_back(propID);
    for(auto keyFrame : _keyFrames)
    {
        keyFrame->SetValueCount(_propIDs.size());
    }
}

void AnimationLayer::DeletePropID(int propID)
{
    for(size_t i = 0; i < _propIDs.size(); ++i)
    {
        if(_propIDs[i] == propID)
        {
            _propIDs.erase(_propIDs.begin() + i);
            for (auto keyFrame : _keyFrames)
            {
                keyFrame->DeleteValue(i);
            }
            break;
        }
    }
}

KeyFrame *AnimationLayer::CreateKeyFrame(kmScalar position)
{
    BEATS_ASSERT(_animation);
    position = std::min(position, _animation->Duration());
    KeyFrame *keyFrame = new KeyFrame(position);
    keyFrame->SetValueCount(_propIDs.size());
    _keyFrames.push_back(keyFrame);
    return keyFrame;
}

void AnimationLayer::DeleteKeyFrame(KeyFrame *keyFrame)
{
    for(size_t i = 0; i < _keyFrames.size(); ++i)
    {
        if(_keyFrames[i] == keyFrame)
        {
            _keyFrames.erase(_keyFrames.begin() + i);
            BEATS_SAFE_DELETE(keyFrame);
            break;
        }
    }
}
