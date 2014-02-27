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
    CSpyCommandBase(ESpyCommandType type, size_t uFeedbackFunc = 0, size_t uUserDataPtr = 0);
    virtual ~CSpyCommandBase();

    ESpyCommandType GetType() const;
    size_t GetFeedbackFunc() const;
    size_t GetUserDataPtr() const;
    void SetFeedBackFuncAddr(size_t uFeedbackFunc, size_t uUserDataPtr);

    bool Execute(SharePtr<SSocketContext>& pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    virtual bool ExecuteImpl(SharePtr<SSocketContext>& pSocketContext) = 0;

protected:

    ESpyCommandType m_type;
    size_t m_uFeedbackFunc;
    size_t m_uUserDataPtr;
};
#endif