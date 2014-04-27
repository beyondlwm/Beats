#ifndef GUI_WINDOW_SLIDER_H__INCLUDE
#define GUI_WINDOW_SLIDER_H__INCLUDE

#include "Window.h"
#include "Button.h"
class  CTextureFrag;

namespace FCGUI
{
    class  Slider : public Window
    {
         DEFINE_WINDOW_TYPE(WINDOW_SLIDER);
    public:
        enum EventType
        {
            EVENT_SLIDER = FCGUI::EVENT_SLIDER,
            EVENT_MOVE,
        };

        enum SliderType
        {
            SLIDER_TYPE_HORIZONTAL,
            SLIDER_TYPE_VERTICAL,
        };

    public:
        Slider();
        ~Slider();
        Slider(const TString &name);
        Slider::Slider(CSerializer& serializer);
         
        void SetRange(int nMin, int nMax);
        void SetMinRange(int nMin);
        void SetMaxRange(int nMax);
        void GetRange(int& nMin, int nMax);
        void SetPos(int nPos);
        int  GetPos();
        void SetThumbSize(kmVec2 size);
        kmVec2 GetThumbSize() const; 
        Button* GetSubThumb();
        SliderType GetSliderType();
        void   SetSliderType(SliderType type);
        void   UpdatePos();
        virtual bool OnMouseEvent( MouseEvent *event ) override;

    protected:
        void Init();
        bool   IsContainPos(const kmVec3 &pos);
    private:
        SliderType  _slierType;
        Button*    _pSubThumb;

        int _nMinRange;
        int _nMaxRange;

        int _nPos;
        bool _bPressed;

        kmVec2 _thumSize;
    };

}

#endif

