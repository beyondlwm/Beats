#ifndef GUI_RENDERER_CHECKBOX_RENDERER_H__INCLUDE
#define GUI_RENDERER_CHECKBOX_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/CheckBox.h"

class CTextureFrag;

namespace FCGUI
{
    class  CheckBoxRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_CHECKBOX)
    public:
        CheckBoxRenderer();
        
        void AddLayer(const TString &contentTextureName, const TString &indicatorTextureName, CheckBox::State state);
        void AddLayer(CTextureFrag *contentLayer, CTextureFrag* indicatorLayer, CheckBox::State state);
    };
}



#endif