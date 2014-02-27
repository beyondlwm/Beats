#ifndef BEATS_CONTROL_MOUSE_H__INCLUDE
#define BEATS_CONTROL_MOUSE_H__INCLUDE

#include <dinput.h>
#include "MousePublicDef.h"

struct SLockMouseInfo
{
    EMouseButton m_lockBtn;
    EMouseButton m_unlockBtn;
    EMouseBtnState m_lockState;
    EMouseBtnState m_unlockState;
    SLockMouseInfo()
    : m_lockBtn(eMB_Count)
    , m_unlockBtn(eMB_Count)
    , m_lockState(eMBS_Count)
    , m_unlockState(eMBS_Count)
    {

    }
};

class CMouse
{
    BEATS_DECLARE_SINGLETON(CMouse);
public:

    void Init(const HWND& hwnd, IDirectInput8* device);
    void Capture();

    EMouseBtnState GetMouseBtnState(EMouseButton btn, bool ignoreLock = false);

    /*
    Lock the button state, after this you will always get the lock state when you call GetMouseBtnState with ignoreLock == false.
    It's a way to simulate mouse state, and you can unlock it by set unlock trigger or call UnlockMouseBtnState directly.
    */
    void  LockMouseBtnState(EMouseButton btn, EMouseBtnState state, EMouseButton unlockTriggerBtn = eMB_Count, EMouseBtnState unlockTriggerState = eMBS_Count);
    void  UnLockMouseBtnState(EMouseButton btn);

    long GetRelativePosX();
    long GetRelativePosY();

    long GetScreenPosX();
    long GetScreenPosY();

    bool IsMoving();
    bool IsInWindow();
    bool IsJustEnter();
    bool IsJustLeave();

private:
    void RecordLastJustPressState();
    void UpdateMouseInWindowFlag();
    void ClipMousePos();

private:
    long m_xPos;
    long m_yPos;

    long m_xWindowPos;
    long m_yWindowPos;

    bool m_isMoving;
    bool m_isInWindow;
    bool m_isInWindowLastUpdate;
    DWORD m_lastJustPressTickCount;
    DWORD m_coopSetting;
    std::map<EMouseButton, SLockMouseInfo> m_lockMouseInfo;

    HWND m_globalHwnd;
    IDirectInputDevice8* m_pMouseDevice;
    DIMOUSESTATE m_curState;
    DIMOUSESTATE m_lastState;
    DIMOUSESTATE m_lastJustPressState;
};

#endif