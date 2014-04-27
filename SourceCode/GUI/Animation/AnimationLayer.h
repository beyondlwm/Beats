#ifndef GUI_ANIMATION_ANIMATION_LAYER_H__INCLUDE
#define GUI_ANIMATION_ANIMATION_LAYER_H__INCLUDE

#include "KeyFrame.h"

namespace FCGUI
{
    class Animatable;
    class Animation;

    class AnimationLayer
    {
    public:
        AnimationLayer(const TString &name = _T(""));

        ~AnimationLayer();

        void SetAnimation(Animation *animation);

        void SetEnabled(bool enabled);
        bool Enabled() const;

        void AddPropID(int propID);
        void DeletePropID(int propID);

        KeyFrame *CreateKeyFrame(kmScalar position);
        void DeleteKeyFrame(KeyFrame *keyFrame);
        
        void Apply(Animatable *target, kmScalar position);

    private:
        KeyFrame::ValueList interpolation(const KeyFrame *left, const KeyFrame *right, kmScalar position);

    private:
        Animation *_animation;
        TString _name;
        bool _enabled;
        std::vector<int> _propIDs;
        std::vector<KeyFrame *> _keyFrames;
    };
}

#endif // !GUI_ANIMATION_ANIMATION_LAYER_H__INCLUDE
