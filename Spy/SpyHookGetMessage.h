#ifndef BEATS_SPY_HOOK_SPYHOOKGETMESSAGE_H__INCLUDE
#define BEATS_SPY_HOOK_SPYHOOKGETMESSAGE_H__INCLUDE

#include "SpyHookBase.h"

class CSpyHookGetMessage : public CSpyHookBase
{
public:
    CSpyHookGetMessage();
    virtual ~CSpyHookGetMessage();

    virtual bool StartImpl();

    static LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam);
    static bool RecordTextMessage(const MSG* pMsg, CSerializer& serializer);
    static bool FlushSameCharInfo(TCHAR& cLastChar, size_t& uSameCount, TCHAR* pFlushString);
private:
    HANDLE m_hWriteFileSemaphore;

};
#endif