#ifndef GUI_WINDOW_PROGRESS_H__INCLUDE
#define GUI_WINDOW_PROGRESS_H__INCLUDE
#include "Window.h"

namespace FCGUI
{
    class  Progress : public Window
    {
        DEFINE_WINDOW_TYPE(WINDOW_PROGRESS);

    public:
        Progress(const TString &name);
        Progress(CSerializer serializer);
        Progress ();
        ~ Progress ();

        void    SetRange(int nRange);
        int     GetRange() const;
        void    SetPos(int nPos);
        int     GetPos();
        int     SetStep(int nStep);
        void    Step();
        Window* GetSubWndFill();
        void    UpdateProgress();
        virtual int CurrState() const override;
    protected:
        void Init();
    private:
        Window      *_pSubWndFill;

        State        _state;
        int          _nRange;
        int          _nPos;
        int          _nStep;
    };
}

#endif