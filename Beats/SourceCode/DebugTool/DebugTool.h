#ifndef BEATS_DEBUGTOOL_DEBUGTOOL_H__INCLUDE
#define BEATS_DEBUGTOOL_DEBUGTOOL_H__INCLUDE

#define DEBUG_MSG_BUFF_LENGTH 1024

enum EPrintMsgType
{
    ePMT_MouseState,
    ePMT_WindowState,

    ePMT_Count,
    ePMT_Force32Bit = 0xffffffff
};

class CDebugTool
{
    BEATS_DECLARE_SINGLETON(CDebugTool);
public:
    void PrintDebugMsg();
    void SwitchDebugMsgPrinting(EPrintMsgType type, bool enable);
    void SwitchDebugMsgPrinting(bool enable);

private:
    void InitPrintFuncPool();
    void PrintMouseState();
    void PrintWindowState();

private:
    TCHAR m_msgStrBuff[DEBUG_MSG_BUFF_LENGTH];
    typedef void(CDebugTool::*TPrintFunc)(void);
    TPrintFunc m_printFuncPool[ePMT_Count];
    std::vector<TPrintFunc> m_activePrintFunc;
};



#endif