#include "stdafx.h"
#include "Mouse.h"

CMouse* CMouse::m_pInstance = NULL;

CMouse::CMouse()
: m_xPos(0)
, m_yPos(0)
, m_xWindowPos(0)
, m_yWindowPos(0)
, m_isMoving(false)
, m_isInWindow(false)
, m_isInWindowLastUpdate(false)
, m_lastJustPressTickCount(0)
, m_coopSetting(0)
, m_globalHwnd(NULL)
, m_pMouseDevice(NULL)

{
    memset(&m_curState, 0, sizeof(DIMOUSESTATE));
    memset(&m_lastState, 0, sizeof(DIMOUSESTATE));
    memset(&m_lastJustPressState, 0, sizeof(DIMOUSESTATE));
}

CMouse::~CMouse()
{
    m_pMouseDevice->Unacquire();
    m_pMouseDevice->Release();
    m_pMouseDevice = NULL;
}

void CMouse::Init( const HWND& hwnd, IDirectInput8* device )
{
    HRESULT hr = device->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL);
    BEATS_ASSERT(hr == S_OK, _T("Create Mouse Device Failed!"));

    hr = m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
    BEATS_ASSERT(hr == S_OK, _T("Set Format For Mouse Failed!"));

    m_coopSetting = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;
    hr = m_pMouseDevice->SetCooperativeLevel(hwnd, m_coopSetting);
    BEATS_ASSERT(hr == S_OK, _T("Set Cooperative Level For Mouse Failed!"));

    hr = m_pMouseDevice->Acquire();

    m_globalHwnd = hwnd;
}

void CMouse::Capture()
{
    RecordLastJustPressState();
    m_lastState = m_curState;
    HRESULT  hr = m_pMouseDevice->GetDeviceState( sizeof(DIMOUSESTATE), &m_curState );
    
    if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
    {
        memset(&m_curState, 0, sizeof(DIMOUSESTATE));
        memset(&m_lastState, 0, sizeof(DIMOUSESTATE));
        memset(&m_lastJustPressState, 0, sizeof(DIMOUSESTATE));

        hr = m_pMouseDevice->Acquire();
        if (hr != DIERR_OTHERAPPHASPRIO)
        {
            m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_curState);    
        }
    }
    else
    {
        ClipMousePos();
        UpdateMouseInWindowFlag();
    }
}

EMouseBtnState CMouse::GetMouseBtnState(EMouseButton btn, bool ignoreLock)
{
    EMouseBtnState result = eMBS_Release;
    bool returnLockState = false;

    if (!ignoreLock)
    {
        std::map<EMouseButton, SLockMouseInfo>::iterator iter = m_lockMouseInfo.find(btn);
        if (iter != m_lockMouseInfo.end())
        {
            result = iter->second.m_lockState;
            returnLockState = true;
            //check unlock trigger.
            EMouseBtnState state = GetMouseBtnState(iter->second.m_unlockBtn, true);
            if (state == iter->second.m_unlockState)
            {
                m_lockMouseInfo.erase(iter);
            }
        }
    }

    if (!returnLockState)
    {
        if (m_curState.rgbButtons[btn] & 0x80)
        {
            bool isLastJustPressBtn = (m_lastJustPressState.rgbButtons[btn] & 0x80) != 0;
            if ((m_lastState.rgbButtons[btn] & 0x80) == 0)
            {
                bool inDoubleClickTime = GetTickCount() - m_lastJustPressTickCount  <=  GetDoubleClickTime();
                result = isLastJustPressBtn && inDoubleClickTime ? eMBS_DoubleClick : eMBS_JustPress;
            }
            else
            {
                result = eMBS_Press;
            }
        }
        else if (m_lastState.rgbButtons[btn] & 0x80)
        {
            result = eMBS_JustRelease;
        }
    }
    return result;
}

void CMouse::RecordLastJustPressState()
{    
    bool releaseLastState = ((m_lastState.rgbButtons[0] | m_lastState.rgbButtons[1] | m_lastState.rgbButtons[2]) & 0x80) == 0;
    bool pressThisState = ((m_curState.rgbButtons[0] | m_curState.rgbButtons[1] | m_curState.rgbButtons[2]) & 0x80) != 0;
    if (releaseLastState && pressThisState)
    {
        m_lastJustPressState = m_curState;
        m_lastJustPressTickCount = GetTickCount();
    }
}

long CMouse::GetRelativePosX()
{
    return m_xPos;
}

long CMouse::GetRelativePosY()
{
    return m_yPos;
}

long CMouse::GetScreenPosX()
{
    return m_xWindowPos;
}

long CMouse::GetScreenPosY()
{
    return m_yWindowPos;
}

bool CMouse::IsMoving()
{
    return (m_curState.lX != 0 || m_curState.lY != 0);
}

void CMouse::UpdateMouseInWindowFlag()
{
    m_isInWindowLastUpdate = m_isInWindow;

    POINT pt;
    pt.x = 0;
    pt.y = 0;
    ClientToScreen(m_globalHwnd, &pt);

    RECT rect;
    GetClientRect(m_globalHwnd, &rect);
    rect.left += pt.x;
    rect.right += pt.x;
    rect.top += pt.y;
    rect.bottom += pt.y;

    pt.x = m_xWindowPos;
    pt.y = m_yWindowPos;

    m_isInWindow = PtInRect(&rect, pt) == TRUE;
}

bool CMouse::IsJustEnter()
{
    return m_isInWindow && !m_isInWindowLastUpdate;
}

bool CMouse::IsJustLeave()
{
    return !m_isInWindow && m_isInWindowLastUpdate;
}

bool CMouse::IsInWindow()
{
    return m_isInWindow;
}

void CMouse::ClipMousePos()
{
    if (m_coopSetting & DISCL_NONEXCLUSIVE)
    {
        POINT point;
        GetCursorPos(&point);
        m_xWindowPos = point.x;
        m_yWindowPos = point.y;

        ScreenToClient(m_globalHwnd, &point);
        m_xPos = point.x;
        m_yPos = point.y;

        RECT rect;
        GetClientRect(m_globalHwnd, &rect);

        BEATS_CLIP_VALUE(m_xPos, 0, rect.right);
        BEATS_CLIP_VALUE(m_yPos, 0, rect.bottom);

    }
    else
    {
        RECT rect;
        GetClientRect(m_globalHwnd, &rect);
        long windowHeight = rect.bottom - rect.top;
        long windowWidth =rect.right - rect.left;

        m_xPos += m_curState.lX;
        m_yPos += m_curState.lY;

        BEATS_CLIP_VALUE(m_xPos, 0, windowWidth);
        BEATS_CLIP_VALUE(m_yPos, 0, windowHeight);

        //there is no difference when we are under exclusive mode.
        m_xWindowPos = m_xPos;
        m_yWindowPos = m_yPos;
    }
}

void CMouse::LockMouseBtnState( EMouseButton btn, EMouseBtnState state, EMouseButton unlockTriggerBtn /*= eMB_Count*/, EMouseBtnState unlockTriggerState /*= eMBS_Count*/ )
{
    BEATS_ASSERT(m_lockMouseInfo.find(btn) == m_lockMouseInfo.end(), _T("The button is already locked! btn:%d"), btn);
    SLockMouseInfo info;
    info.m_lockBtn = btn;
    info.m_lockState = state;
    info.m_unlockBtn = unlockTriggerBtn;
    info.m_unlockState = unlockTriggerState;
    m_lockMouseInfo[btn] = info;
}

void CMouse::UnLockMouseBtnState( EMouseButton btn )
{
    BEATS_ASSERT(m_lockMouseInfo.find(btn) != m_lockMouseInfo.end(), _T("The button has not been locked! btn:%d"), btn);
    m_lockMouseInfo.erase(m_lockMouseInfo.find(btn));
}