#include "stdafx.h"
#include "Progress.h"
#include "GUI/Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "GUI/WindowManager.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentBase.h"
using namespace FCGUI;

Progress::Progress(const TString &name)
{
    Init();
}

Progress::Progress(CSerializer serializer)
{
    Init();
    DECLARE_PROPERTY(serializer, _nRange, true, 0xFFFFFFFF, _T("·¶Î§"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _nPos, true, 0xFFFFFFFF, _T("Î»ÖÃ"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, _nStep, true, 0xFFFFFFFF, _T("²½³¤"), NULL, NULL, NULL);
}

Progress::~ Progress ()
{
    
}

void Progress::Init()
{
    _nRange = 100;
    _nPos = 0;
    _nStep = 1;
     _state = STATE_NORMAL;
    _pSubWndFill = WindowManager::GetInstance()->Create<Window>(_T("SubFill"),this);
    UpdateProgress();
}

void Progress::SetRange(int nRange)
   
{
    _nRange = nRange;
}

int Progress::GetRange() const
{
    return _nRange;
}

void Progress::SetPos(int nPos)
{
    _nPos = nPos;

    UpdateProgress();
}

int Progress::GetPos()
{
    return _nPos;
}

int Progress::SetStep(int nStep)
{
    int nOldStep = _nStep;
    _nStep = nStep;

    return nOldStep;
}

void Progress::Step()
{
    _nPos += _nStep;

    UpdateProgress();
}

Window* Progress::GetSubWndFill()
{
    return _pSubWndFill;
}

int Progress::CurrState() const
{
    return _state;
}

void Progress::UpdateProgress()
{
    float fPercent = (float)(_nPos) / (float)_nRange;
    GetSubWndFill()->SetArea(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, fPercent, 1.f);
}


