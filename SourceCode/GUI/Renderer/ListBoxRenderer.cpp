#include "stdafx.h"
#include "ListBoxRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
#include "Render/TextureFrag.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "GUI/Window/ListBox.h"
using namespace FCGUI;

void  ListBoxRenderer::AddLayer(const TString &backTextName, const TString &selTextName)
{
    ListBox *listbox = static_cast<ListBox *>(_window); 
    BaseRenderer::AddLayer(backTextName);

    Window *subSel = listbox->GetSubSelWin();
    BaseRenderer* subrender = subSel->Renderer();
    subrender->AddLayer(selTextName);
}

void  ListBoxRenderer::AddLayer(CTextureFrag* backLayer, CTextureFrag *selLayer)
{
    ListBox *listbox = static_cast<ListBox *>(_window); 
    BaseRenderer::AddLayer(backLayer);

    Window *subSel = listbox->GetSubSelWin();
    BaseRenderer* subrender = subSel->Renderer();
    subrender->AddLayer(selLayer);
}
void ListBoxRenderer::renderLayers( const kmMat4 &parentTransform ) const
{
    BaseRenderer::renderLayers(parentTransform);

    ListBox *listbox = static_cast<ListBox*>(_window);
    BEATS_ASSERT(listbox != nullptr);

    kmVec3 pos;
    kmVec3Fill(&pos,  listbox->RealPos().x, listbox->RealPos().y, 0.f);
    listbox->LocalToWorld(pos);
    const std::vector<TString> strVec = listbox->GetStringList();

    const TString &fontFaceName = listbox->_pFontFace->GetName();

    kmScalar padding = listbox->GetPadding();
    pos.y += padding;

    kmScalar height = listbox->GetItemHeight();
    for(size_t i = 0; i < strVec.size(); ++i)
    {
        const TString &str = strVec[i];
        FontManager::GetInstance()->RenderText(str, pos.x + padding, pos.y + i * height, fontFaceName);
    }
}