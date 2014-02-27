#ifndef BEATS_SPY_COMMAND_SPYCOMMANDUPGRADE_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDUPGRADE_H__INCLUDE

#include "SpyCommandBase.h"

class CSpyCommandUpgrade : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandUpgrade();
    virtual ~CSpyCommandUpgrade();
    void SetUpdateSelfFlag(bool bSelfOrRemote);

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    bool m_bUpdateClientOrServer;
    int m_iClientVersion;

};

#endif