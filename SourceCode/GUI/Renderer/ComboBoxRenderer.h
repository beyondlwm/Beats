#ifndef GUI_RENDERER_COMBOBOX_RENDERER_H__INCLUDE
#define GUI_RENDERER_COMBOBOX_RENDERER_H__INCLUDE

#include "BaseRenderer.h"
#include "GUI/Window/ComboBox.h"
#include "GUI/Window/Button.h"
namespace FCGUI
{
    class  ComboBoxRenderer : public BaseRenderer
    {
        DEFINE_RENDERER_TYPE(RENDERER_COMBOBOX)
    public:
        void AddDropDownLayer(const TString &downTextureName, Button::State downState);
        void AddComboBoxLayer(const TString &comboxTextureName, const TString &listboxTexureName, const TString &selTextureName);
    };

}

#endif
