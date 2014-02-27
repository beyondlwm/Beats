#ifndef BEATS_SPY_SPYCOMMANDMANAGER_H__INCLUDE
#define BEATS_SPY_SPYCOMMANDMANAGER_H__INCLUDE

#include "SpyCommandBase.h"


class CSpyCommandManager
{
    BEATS_DECLARE_SINGLETON(CSpyCommandManager);
public:
    bool Initialize();
    bool Uninitialize();

    bool RegisterCommand(CSpyCommandBase* pCommand);
    bool SendCommand(SharePtr<SSocketContext>& pSocketContext, CSpyCommandBase* pCommandBase);
    bool ExecuteCommand(SharePtr<SSocketContext>& pSocketContext, CSerializer& serializer);

private:
    std::map<ESpyCommandType, CSpyCommandBase*> m_commandMap;
};

#endif