#include "stdafx.h"
#include "TextLabel.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "Event/KeyboardEvent.h"
#include "GUI/WindowManager.h"
#include "GUI/Font/FontManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
using namespace FCGUI;

TextLabel::TextLabel( const TString &name )
    :  Window(name)
{
    Init();
}

TextLabel::TextLabel(CSerializer serializer)
{
    Init();
    DECLARE_PROPERTY(serializer, _padding, true, 0xFFFFFFFF, _T("±ß¿ò´óÐ¡"), NULL, NULL, NULL);
}

void TextLabel::Init()
{
    _padding = 3;
    _displayText = _T(""); 
    _pFontFace  = FontManager::GetInstance()->GetFace(_T("STFANGSO_12"));
}

bool TextLabel::OnMouseEvent( MouseEvent *event ) 
{
    bool bRet = false;
    if( IsEnabled() )
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_PRESSED)
            {
                WindowEvent event(EVENT_TEXTLABEL_PRESSED);
                DispatchEvent(&event);
            }
        }
        bRet = true;
    }
    return bRet;
}
const TString& TextLabel::GetDisplayText() const
{
    return _displayText;
}

void TextLabel::SetText(const TString &text)
{
    _displayText = text;
}

kmScalar TextLabel::GetPadding() const
{
    return _padding;
}

kmVec2 TextLabel::GetWorldPos()
{
    kmVec3 pos;
    kmVec2 worldPos;
    kmVec3Fill(&pos,  PercentPos().x,  PercentPos().y, 0.f);
    kmMat4 worldTM = WorldTransform();
    kmVec3Transform(&pos, &pos, &worldTM);
    kmVec2Fill(&worldPos, pos.x, pos.y);

    return worldPos;
}