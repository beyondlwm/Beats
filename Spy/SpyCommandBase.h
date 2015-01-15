#ifndef BEATS_SPY_COMMAND_SPYCOMMANDBASE_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDBASE_H__INCLUDE

#include "spy.h"

enum ESpyCommandType
{
    eSCT_Invalid = 0,
    eSCT_Version,
    eSCT_MessageBox,
    eSCT_Download,
    eSCT_Upload,
    eSCT_SystemCmd,
    eSCT_GetDirectoryInfo,
    eSCT_Update,
    eSCT_ShotScreen,

    eSCT_Count,
    eSCT_Force32Bit = 0xFFFFFFFF
};


class CSpyCommandBase
{
public:
    CSpyCommandBase(ESpyCommandType type, uint32_t uFeedbackFunc = 0, uint32_t uUserDataPtr = 0);
    virtual ~CSpyCommandBase();

    ESpyCommandType GetType() const;
    uint32_t GetFeedbackFunc() const;
    uint32_t GetUserDataPtr() const;
    void SetFeedBackFuncAddr(uint32_t uFeedbackFunc, uint32_t uUserDataPtr);

    bool Execute(SharePtr<SSocketContext>& pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    virtual bool ExecuteImpl(SharePtr<SSocketContext>& pSocketContext) = 0;

protected:

    ESpyCommandType m_type;
    uint32_t m_uFeedbackFunc;
    uint32_t m_uUserDataPtr;
};
#endif