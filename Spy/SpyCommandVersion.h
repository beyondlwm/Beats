#ifndef BEATS_SPY_COMMAND_SPYCOMMANDVERSION_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDVERSION_H__INCLUDE

#include "SpyCommandBase.h"

class CSpyCommandVersion : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandVersion();
    virtual ~CSpyCommandVersion();

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    int m_iCurVersion;
    bool m_bRequst;

};

#endif