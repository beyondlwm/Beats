#ifndef GUI_BEHAVIOR_BEHAVIOR_H__INCLUDE
#define GUI_BEHAVIOR_BEHAVIOR_H__INCLUDE

#include "BehaviorDefs.h"

namespace FCGUI{
    //forward declaration
    class Window;

    class BehaviorBase
    {
    public:
        BehaviorBase(Window *window);
        virtual ~BehaviorBase();

        virtual BehaviorType Type() const = 0;

    protected:
        Window *_window;
    };
}

#endif