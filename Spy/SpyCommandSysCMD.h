#ifndef BEATS_SPY_COMMAND_SPYCOMMANDSYSCMD_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDSYSCMD_H__INCLUDE

#include "Spy.h"

enum ECommandSysCMDType
{
    eCSCT_Send,
    eCSCT_Recv,
    eCSCT_SelfDefineFeedback,

    eCSCT_Count,
    eCSCT_Force32Bit = 0xFFFFFFFF
};

#include "SpyCommandBase.h"

class CSpyCommandSysCMD : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandSysCMD();
    virtual ~CSpyCommandSysCMD();

    void SetSystemCMDData(const char* pData);
    void SetType(ECommandSysCMDType type);

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    void SendSelfDefineFeedBack(SharePtr<SSocketContext>&  pSocketContext, const char* pszErrorString);
    // There are some command which p_open can't execute well(p_open start a cmd.exe process, execute command, fetch the result, terminate process.
    // so it can't hold the environment which was changed by command such as "cd..", "cd\". and some other command like "cls" also)
    bool HandleSpecificCommand(const char* pCommand, std::string& strFeedback);

private:
    CSerializer* m_pSerializer;
    ECommandSysCMDType m_type;
};


#endif