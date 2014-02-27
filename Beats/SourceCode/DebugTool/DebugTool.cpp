#include "stdafx.h"
#include "DebugTool.h"
#include "../Control/Mouse.h"
#include "../game/Game.h"

CDebugTool* CDebugTool::m_pInstance = NULL;

CDebugTool::CDebugTool()
{
    memset(m_msgStrBuff, 0, DEBUG_MSG_BUFF_LENGTH);
    InitPrintFuncPool();
}

CDebugTool::~CDebugTool()
{

}

void CDebugTool::InitPrintFuncPool()
{
    TPrintFunc printFuncPool[] =
    {
        &CDebugTool::PrintMouseState,
        &CDebugTool::PrintWindowState
    };
    memcpy(m_printFuncPool, printFuncPool, 4 * ePMT_Count);
}

void CDebugTool::PrintDebugMsg()
{
    for (size_t i = 0; i < m_activePrintFunc.size(); ++i)
    {
        (this->*m_activePrintFunc[i])();
    }
}

void CDebugTool::SwitchDebugMsgPrinting( EPrintMsgType type, bool enable )
{
    BEATS_ASSERT(type >= 0 && type < ePMT_Count, _T("Message type %d is out of bound! max type number is %d"), type, ePMT_Count);
    bool msgFuncExisted = false;
    for (std::vector<TPrintFunc>::iterator iter = m_activePrintFunc.begin(); iter != m_activePrintFunc.end(); ++iter)
    {
        if ((*iter) == m_printFuncPool[type])
        {
            if (enable)
            {
                msgFuncExisted = true;
            }
            else
            {
                m_activePrintFunc.erase(iter);
            }
            break;
        }
    }
    if (enable && !msgFuncExisted)
    {
        m_activePrintFunc.push_back(m_printFuncPool[type]);
    }
}

void CDebugTool::SwitchDebugMsgPrinting( bool enable )
{
    m_activePrintFunc.clear();
    if (enable)
    {
        for (size_t i = 0; i < ePMT_Count; ++i)
        {
            m_activePrintFunc.push_back(m_printFuncPool[i]);
        }
    }        
}

void CDebugTool::PrintMouseState()
{
    CMouse* pMouse = CMouse::GetInstance();
    _stprintf_s(m_msgStrBuff, _T("[MousePos]\nScreenPos x:%d, y:%d   ClientPos x:%d, y:%d\n"), 
        pMouse->GetScreenPosX(), 
        pMouse->GetScreenPosY(), 
        pMouse->GetRelativePosX(), 
        pMouse->GetRelativePosY());
    OutputDebugString(m_msgStrBuff);
}

void CDebugTool::PrintWindowState()
{
    HWND pWnd = CGame::GetInstance()->GetHwnd();
    RECT windowRect, clientRect;
    GetWindowRect(pWnd, &windowRect);
    GetClientRect(pWnd, &clientRect);
    _stprintf_s(m_msgStrBuff, _T("[WindowState]\nWindow left:%d, right:%d, top:%d, bottom:%d\nClient left:%d, right:%d, top:%d, bottom:%d\n"), 
        windowRect.left, 
        windowRect.right, 
        windowRect.top, 
        windowRect.bottom,
        
        clientRect.left, 
        clientRect.right, 
        clientRect.top, 
        clientRect.bottom);
    OutputDebugString(m_msgStrBuff);
}