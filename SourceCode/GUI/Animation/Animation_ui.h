#ifndef GUI_ANIMATION_ANIMATION_H__INCLUDE
#define GUI_ANIMATION_ANIMATION_H__INCLUDE


namespace FCGUI
{
    class AnimationLayer;
    class Animatable;

    class Animation
    {
    public:
        enum class ReplayMode
        {
            DEFAULT,
            ONCE,
            LOOP,
            BOUNCE,
            REVERSE = 0x4,
            REVERSE_LOOP = REVERSE | LOOP,
            REVERSE_BOUNCE = REVERSE | BOUNCE,
        };

        Animation(const TString &name);

        ~Animation();

        TString Name() const;

        void SetDuration(kmScalar duration);
        kmScalar Duration() const;

        void SetMode(ReplayMode replayMode);
        ReplayMode Mode() const;

        AnimationLayer *CreateLayer(const TString &name = _T(""));
        void DeleteLayer(AnimationLayer *layer);

        void Apply(Animatable *target, kmScalar position);

    private:
        TString _name;
        kmScalar _duration;
        ReplayMode _replayMode;
        std::vector<AnimationLayer *> _layers;
    };

}

#endif // !GUI_ANIMATION_ANIMATION_H__INCLUDE
