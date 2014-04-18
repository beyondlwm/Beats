#ifndef GUI_ANIMATION_ANIM_PROP_H__INCLUDE
#define GUI_ANIMATION_ANIM_PROP_H__INCLUDE

namespace FCGUI
{
    class AnimProp
    {
    public:
        typedef std::function<void(kmScalar)> Setter;
        typedef std::function<kmScalar()> Getter;

        AnimProp(int ID, const TString &name, const Setter &setter, const Getter &getter)
            : ID(ID)
            , name(name)
            , setter(setter)
            , getter(getter)
        {}

        int ID;
        TString name;
        Setter setter;
        Getter getter;
    };
}

#endif // !GUI_ANIMATION_ANIM_PROP_H__INCLUDE
