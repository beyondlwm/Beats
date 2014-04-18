#include "stdafx.h"
#include "ComboBoxRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
#include "ListBoxRenderer.h"
#include "GUI/Window/ListBox.h"
using namespace FCGUI;

void ComboBoxRenderer::AddDropDownLayer(const TString &downTextureName, Button::State downState)
{
    ComboBox *combobox = static_cast<ComboBox*>(_window);
    Button *dropdownBtn = combobox->GetDropDownButtton();

    BaseRenderer* renderer = dropdownBtn->Renderer();
    renderer->AddLayer(downTextureName, downState);
}
void ComboBoxRenderer::AddComboBoxLayer(const TString &comboxTextureName, const TString &listboxTexureName, const TString &selTextureName)
{
    BaseRenderer::AddLayer(comboxTextureName);
     ComboBox *combobox = static_cast<ComboBox*>(_window);
    ListBox *listbox = combobox->GetListBox();

    ListBoxRenderer* renderer = static_cast<ListBoxRenderer*>(listbox->Renderer());
    renderer->AddLayer(listboxTexureName,selTextureName);
}