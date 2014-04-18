#ifndef GUI_ANIMATION_ANIMATABLE_H__INCLUDE
#define GUI_ANIMATION_ANIMATABLE_H__INCLUDE

#include "AnimProp.h"

namespace FCGUI
{
    class Animatable
    {
    public:
        Animatable();
        virtual ~Animatable();

        void AddProp(int ID, const TString &name, 
            const AnimProp::Setter &setter, const AnimProp::Getter &getter);

        const AnimProp *GetProp(int ID) const;

    private:
        std::map<int, AnimProp *> _props;
    };
}

#endif