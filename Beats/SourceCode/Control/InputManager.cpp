#include "stdafx.h"
#include <dinput.h>
#include "InputManager.h"
#include "Keyboard.h"
#include "Mouse.h"

CInputManager* CInputManager::m_pInstance = NULL;


CInputManager::CInputManager()
: m_pDirectInput(NULL)
{

}

CInputManager::~CInputManager()
{
    CMouse::Destroy();
    CKeyboard::Destroy();

    BEATS_SAFE_DELETE_MAP(m_callBacks, TCallbackMap);
    for (TCallbackStack::iterator iter = m_stack.begin(); iter != m_stack.end(); ++iter)
    {
        BEATS_SAFE_DELETE_VECTOR(iter->second);
    }
    m_stack.clear();
    if (m_pDirectInput != NULL)
    {
        m_pDirectInput->Release();
    }
}

void CInputManager::Init(const HWND& hwnd, const HINSTANCE& instance)
{
    HRESULT hr = DirectInput8Create( instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDirectInput, NULL );
    BEATS_ASSERT(hr == S_OK, _T("Create Input Device Failed!"));

    CKeyboard::GetInstance()->Init(hwnd, m_pDirectInput);
    CMouse::GetInstance()->Init(hwnd, m_pDirectInput);
  
}

void CInputManager::Update()
{
    BEATS_PERFORMDETECT_START(NBDT::ePT_CaptureKeyboard);
    CKeyboard::GetInstance()->Capture();
    BEATS_PERFORMDETECT_STOP(NBDT::ePT_CaptureKeyboard);

    BEATS_PERFORMDETECT_START(NBDT::ePT_CaptureMouse);
    CMouse::GetInstance()->Capture();
    BEATS_PERFORMDETECT_STOP(NBDT::ePT_CaptureMouse);

    BEATS_PERFORMDETECT_START(NBDT::ePT_InputCallback);
    CheckCallback();
    BEATS_PERFORMDETECT_STOP(NBDT::ePT_InputCallback);
}

void CInputManager::RegisterKeyBoardCallback( SKeyTriggerType type, const Function<void(void)>& function, bool exclusive)
{
    WORD id = (WORD)((type.m_code << 8) + type.m_state);
    Signal<void(void)>* pSignal = NULL;
    TCallbackMap::iterator iter = m_callBacks.find(id);
    bool canAddNewSignal = iter == m_callBacks.end() || exclusive;
    BEATS_ASSERT(canAddNewSignal, _T("Can't register keybaord callback twice!"));
    if (iter != m_callBacks.end())
    {
        m_stack[id].push_back(iter->second);
    }
    pSignal = new Signal<void(void)>();
    m_callBacks[id] = pSignal;

    pSignal->Connect(function);
}

void CInputManager::UnRegisterKeyBoardCallback( SKeyTriggerType type, const Function<void(void)>& function)
{
    WORD id = type.GetUniqueId();
    TCallbackMap::iterator iter = m_callBacks.find(id);
    BEATS_ASSERT(iter != m_callBacks.end(), _T("Can't Unregister a call back before register it!"));
    iter->second->Disconnect(function);
    TCallbackStack::iterator stackIter = m_stack.find(id);
    if (stackIter != m_stack.end() && iter->second->GetConnectedCount() == 0)
    {
        BEATS_SAFE_DELETE(iter->second);
        PopStack(id);
    }
}

void CInputManager::CheckCallback()
{
    for (TCallbackMap::iterator iter = m_callBacks.begin(); iter != m_callBacks.end(); ++iter)
    {
        SKeyTriggerType triggerType(iter->first);
        if (CKeyboard::GetInstance()->GetKeyState(triggerType.m_code) == triggerType.m_state)
        {
            (*iter->second)();
        }
    }
}

void CInputManager::PopStack(WORD id)
{
    std::vector<Signal<void(void)>*>& stackData = m_stack[id];
    m_callBacks[id] = stackData.back();
    stackData.pop_back();
    if (stackData.size() == 0)
    {
        m_stack.erase(id);
    }
}

void CInputManager::Clear()
{
    BEATS_SAFE_DELETE_MAP(m_callBacks, TCallbackMap);
    for (TCallbackStack::iterator iter = m_stack.begin(); iter != m_stack.end(); ++iter)
    {
        BEATS_SAFE_DELETE_VECTOR(iter->second);
    }
    m_stack.clear();
}