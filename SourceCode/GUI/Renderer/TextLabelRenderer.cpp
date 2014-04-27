#include "stdafx.h"
#include "TextLabelRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
#include "Render/TextureFrag.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "GUI/Window/TextLabel.h"
using namespace FCGUI;

TextLabelRenderer::TextLabelRenderer()
{

}

void TextLabelRenderer::renderLayers( const kmMat4 &parentTransform ) const
{
    BaseRenderer::renderLayers(parentTransform);

    TextLabel *textLabel = static_cast<TextLabel*>(_window);
    BEATS_ASSERT(textLabel != nullptr);

    const TString&  text = textLabel->GetDisplayText();
    if(!text.empty())
    {
        kmVec2 pos = textLabel->GetWorldPos(); 
        const TString &fontFaceName = textLabel->_pFontFace->GetName();
        kmScalar padding = textLabel->GetPadding();
        pos.y += padding;

        FontManager::GetInstance()->RenderText(text, pos.x + padding, pos.y, fontFaceName);
    }     
}