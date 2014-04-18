#include "stdafx.h"
#include "Slider.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/Renderer/SliderRenderer.h"
#include "GUI/WindowManager.h"
#include "Render/TextureFragManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
using namespace FCGUI;

Slider::Slider(const TString &name)
    : Window(name)
{
    Init();
}

Slider::Slider(CSerializer& serializer)
    : Window(serializer)
{
    Init();
    DECLARE_PROPERTY(serializer, _thumSize, true, 0xFFFFFFFF, _T("»¬¿é³ß´ç"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _nMinRange, true, 0xFFFFFFFF, _T("×îÐ¡·¶Î§"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _nMaxRange, true, 0xFFFFFFFF, _T("×î´ó·¶Î§"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _nPos, true, 0xFFFFFFFF, _T("Î»ÖÃ"), NULL, NULL, NULL);
}

void Slider::Init()
{
    _nMinRange = 0;
    _nMaxRange = 100;
    _nPos = 0;
    _bPressed = false;
    _slierType = SLIDER_TYPE_HORIZONTAL;
    kmVec2Fill(&_thumSize, 11.f, 22.f);
    _pSubThumb = WindowManager::GetInstance()->Create<Button>(_T("SubBall"),this);
    UpdatePos();
}

Slider::~Slider()
{

}

bool Slider::OnMouseEvent( MouseEvent *event )
{
    if(event->Button() == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(event->Type() == EVENT_MOUSE_PRESSED)
        {
            _bPressed = true;
        }
        else if(event->Type() == EVENT_MOUSE_RELEASED)
        {
            if(_bPressed)
            {
                kmVec2 size = Size();
                kmVec3 pos;
                kmVec3Fill( &pos, event->X(),  event->Y(), 0.f );
                Window::WorldToLocal( pos );
               
                if(IsContainPos(pos))
                {
                    if(_slierType == SLIDER_TYPE_HORIZONTAL)
                    {
                        float percent = pos.x / size.x;
                        _nPos = (int)(_nMinRange + (_nMaxRange - _nMinRange) * percent);
                    }
                    else if(_slierType == Slider::SLIDER_TYPE_VERTICAL)
                    {
                        float percent = pos.y / size.y;
                        _nPos = (int)(_nMinRange + (_nMaxRange - _nMinRange) * percent);
                    }

                    UpdatePos();
                    WindowEvent event(EVENT_MOVE);
                    DispatchEvent(&event);
                }
                _bPressed = false;
            }

        }
        else if(event->Type() == EVENT_MOUSE_MOVED)
        {
            if(_bPressed)
            {
                kmVec2 size = Size();
                kmVec3 pos;
                kmVec3Fill( &pos, event->X(),  event->Y(), 0.f );
                Window::WorldToLocal( pos );
                if(IsContainPos(pos))
                {
                    if(_slierType == SLIDER_TYPE_HORIZONTAL)
                    {
                        float percent = pos.x / size.x;
                        _nPos = (int)(_nMinRange + (_nMaxRange - _nMinRange) * percent);
                    }
                    else if(_slierType == Slider::SLIDER_TYPE_VERTICAL)
                    {
                        float percent = pos.y / size.y;
                        _nPos = (int)(_nMinRange + (_nMaxRange - _nMinRange) * percent);
                    }

                    UpdatePos();
                    WindowEvent event(EVENT_MOVE);
                    DispatchEvent(&event);
                }
            }
        }
    }
    return true;
}


void Slider::SetRange(int nMin, int nMax)
{
    _nMinRange = nMin;
    _nMaxRange = nMax;
}

void Slider::SetMinRange(int nMin)
{
    _nMinRange = nMin;
}

void Slider::SetMaxRange(int nMax)
{
    _nMaxRange = nMax;
}

void Slider::GetRange(int& nMin, int nMax)
{
    nMin = _nMinRange;
    nMax = _nMaxRange;
}

void Slider::SetPos(int nPos)
{
    _nPos = nPos;
}

int  Slider::GetPos()
{
    return _nPos;
}

void Slider::SetThumbSize(kmVec2 size)
{
    _thumSize = size;
}

kmVec2 Slider::GetThumbSize() const
{
    return _thumSize;
}

Button* Slider::GetSubThumb()
{
    return _pSubThumb;
}

bool Slider::IsContainPos(const kmVec3 &pos)
{
     kmVec2 size = RealSize();
     return (pos.x >=0 && pos.x <= size.x 
             && pos.y >=0 && pos.y <= size.y );
}

Slider::SliderType Slider::GetSliderType()
{
    return _slierType;
}

void Slider::SetSliderType(SliderType type)
{
    _slierType =  type;
}

void Slider::UpdatePos()
{
    Window *subBall = GetSubThumb();

    kmScalar fPercent = (float)_nPos / (float)(_nMaxRange - _nMinRange);
    if( SLIDER_TYPE_HORIZONTAL == _slierType)
        subBall->SetArea(0.f, 0.f, fPercent, 0.f, _thumSize.x, 0.f, fPercent, 1.0f);
    else if(SLIDER_TYPE_VERTICAL == _slierType)
    {
        subBall->SetArea(0.f, 0.f, 0.0f, fPercent, 0.f, _thumSize.y, 1.0f, fPercent);
    }
}



