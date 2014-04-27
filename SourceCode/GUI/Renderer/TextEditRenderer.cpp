#include "stdafx.h"
#include "TextEditRenderer.h"
#include "Render/SpriteFrameBatchManager.h"
#include "Render/TextureFragManager.h"
#include "Render/TextureFrag.h"
#include "GUI/Font/FontManager.h"
#include "GUI/Font/FontFace.h"
#include "GUI/Window/TextEdit.h"
using namespace FCGUI;

TextEditRenderer::TextEditRenderer()
{

}

void TextEditRenderer::renderLayers( const kmMat4 &parentTransform ) const
{
    BaseRenderer::renderLayers(parentTransform);

    TextEdit *edit = static_cast<TextEdit*>(_window);
    BEATS_ASSERT(edit != nullptr);

    const TString&  text = edit->GetText();
    if(!text.empty())
    {
        kmVec3 pos; 
        kmVec3Fill( &pos, edit->RealPos().x, edit->RealPos().y, 0.f );
        edit->LocalToWorld( pos );
        const std::vector<TString> strVec = edit->GetTextOfLines();
        const TString &fontFaceName = edit->_pFontFace->GetName();
        kmScalar padding = edit->GetPadding();
        pos.y += padding;
        
        kmScalar height = 0.f;
        for(size_t i = 0; i < strVec.size(); ++i)
        {
            const TString &str = strVec[i];
            if(i > 0)
            {
                height += edit->GetLineMaxHeight(i);
            }

            FontManager::GetInstance()->RenderText(str, pos.x + padding, pos.y + height, fontFaceName);
        }
    }     
}