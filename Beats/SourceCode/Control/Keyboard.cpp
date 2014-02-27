#include "stdafx.h"
#include <dinput.h>
#include "Keyboard.h"

CKeyboard* CKeyboard::m_pInstance = NULL;

CKeyboard::CKeyboard()
: m_funcKeyModifiers(0)
, m_pKeyBoardDevice(NULL)
{
    memset(m_keyBuffer, 0, 256);
    memset(m_lastKeyBuffer, 0, 256);
}

CKeyboard::~CKeyboard()
{
    m_pKeyBoardDevice->Unacquire();
    m_pKeyBoardDevice->Release();
    m_pKeyBoardDevice = NULL;
}

void CKeyboard::Init(const HWND& hwnd, IDirectInput8* device)
{
    HRESULT hr = device->CreateDevice(GUID_SysKeyboard, &m_pKeyBoardDevice, NULL);
    BEATS_ASSERT( hr == S_OK, _T("Could not init device!"));

    hr = m_pKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard);
    BEATS_ASSERT( hr == S_OK, _T("format error!"));

    hr = m_pKeyBoardDevice->SetCooperativeLevel( hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    BEATS_ASSERT( hr == S_OK, _T("coop error!"));

    hr = m_pKeyBoardDevice->Acquire();
}

void CKeyboard::Capture()
{
    BEATS_ASSERT(m_pKeyBoardDevice != NULL, _T("Keyboard device is not initialized!"));
    memcpy_s(m_lastKeyBuffer, sizeof(m_lastKeyBuffer), m_keyBuffer, sizeof(m_keyBuffer));

    HRESULT  hr = m_pKeyBoardDevice->GetDeviceState( sizeof(m_keyBuffer), &m_keyBuffer );

    if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
    {
        memset(m_keyBuffer, 0, 256);
        hr = m_pKeyBoardDevice->Acquire();
        if (hr != DIERR_OTHERAPPHASPRIO)
        {
            m_pKeyBoardDevice->GetDeviceState(sizeof(m_keyBuffer), &m_keyBuffer);
        }
    }
    
    //Set Shift, Ctrl, Alt
    m_funcKeyModifiers = 0;
    if( IsKeyDown(eKC_LCONTROL) || IsKeyDown(eKC_RCONTROL) )
        m_funcKeyModifiers |= eM_Ctrl;
    if( IsKeyDown(eKC_LSHIFT) || IsKeyDown(eKC_RSHIFT) )
        m_funcKeyModifiers |= eM_Shift;
    if( IsKeyDown(eKC_LMENU) || IsKeyDown(eKC_RMENU) )
        m_funcKeyModifiers |= eM_Alt;

}

bool CKeyboard::IsKeyDown( const EKeyCode keyCode ) const
{
    return (m_keyBuffer[keyCode] & 0x80) != 0;
}

EKeyState CKeyboard::GetKeyState(const EKeyCode keyCode) const
{
    EKeyState result = eKS_Release;

    bool isDownNow = IsKeyDown(keyCode);
    bool isDownLastFrame = (m_lastKeyBuffer[keyCode] & 0x80) != 0;
    bool isReleaseLastFrame = (m_lastKeyBuffer[keyCode] & 0x80) == 0;

    if (isDownNow)
    {
        result = isDownLastFrame ? eKS_Press : eKS_JustPress;
    }
    else if (!isReleaseLastFrame)
    {
        result = eKS_JustRelease;
    }
    return result;
}