#include "stdafx.h"
#include "CheckBox.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFragManager.h"
#include "GUI/Renderer/CheckBoxRenderer.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
using namespace FCGUI;

CheckBox::CheckBox(const TString &name)
    : Window(name)
{
    Init();
}

CheckBox::CheckBox( CSerializer serializer)
    :Window( serializer )
{
    Init();
    DECLARE_PROPERTY(serializer, _fContentWidth, true, 0xFFFFFFFF, _T("±êÇ©Ïî¿í¶È"), NULL, NULL, NULL);
}

CheckBox::~CheckBox()
{
  
}

void CheckBox::InitChild()
{
    _pSubContent = WindowManager::GetInstance()->Create<Window>(_T("SubContent"),this);
    _pSubContent->SetArea( 0.f, 0.f, 1.f, 0.f, _fContentWidth, 0.f, 1.f, 1.f );

}

void CheckBox::Init()
{
    _bCheck = false;
    _state = STATE_NORMAL_UNCHECKED;
    _fContentWidth = 120.f ;

    InitChild();
}

int CheckBox::CurrState() const
{
    return _state;
}

Window* CheckBox::GetSubWindow()
{
        return _pSubContent;
}

bool CheckBox::OnMouseEvent( MouseEvent *event )
{
    bool bRet = false;
    if(IsEnabled() && Window::OnMouseEvent(event))
    {
        if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(event->Type() == EVENT_MOUSE_RELEASED)
            { 
                _bCheck = !_bCheck;
                if(_bCheck)
                {
                    _state = STATE_NORMAL_CHECKED;
                }
                else
                {
                    _state = STATE_NORMAL_UNCHECKED;
                }
               
                WindowEvent event(EVENT_CHECK);
                DispatchEvent(&event);
            }
        }
        bRet = true;
    }
    return bRet;
}

bool CheckBox::IsCheck() const
{
    return _bCheck;
}

void CheckBox::SetCheck(bool bCheck)
{
    if(IsEnabled())
    {
        _bCheck = bCheck;

        if(_bCheck)
        {
            _state = STATE_NORMAL_CHECKED;
        }
        else
        {
            _state = STATE_NORMAL_UNCHECKED;
        }

        WindowEvent event(EVENT_CHECK);
        DispatchEvent(&event);
    }
}

