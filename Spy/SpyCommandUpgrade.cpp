#include "stdafx.h"
#include "SpyCommandUpgrade.h"
#include "SpyCommandManager.h"

CSpyCommandUpgrade::CSpyCommandUpgrade()
: super(eSCT_Update)
, m_bUpdateClientOrServer(true)
, m_iClientVersion(BEATS_SPY_VERSION)
{

}

CSpyCommandUpgrade::~CSpyCommandUpgrade()
{

}

void CSpyCommandUpgrade::SetUpdateSelfFlag(bool bClientOrServer)
{
    m_bUpdateClientOrServer = bClientOrServer;
}

bool CSpyCommandUpgrade::ExecuteImpl(SharePtr<SSocketContext>& /*pSocketContext*/)
{
    if (m_iClientVersion == BEATS_SPY_VERSION)
    {
        // Client version is the same with server, no need to update.
    }
    else
    {
        if (m_bUpdateClientOrServer)
        {
            // Update client
        }
        else
        {
            // Update self
        }
    }
    return true;
}

void CSpyCommandUpgrade::Deserialize(CSerializer& serializer)
{
    super::Deserialize(serializer);

    serializer << m_bUpdateClientOrServer;
    serializer << m_iClientVersion;
}

void CSpyCommandUpgrade::Serialize(CSerializer& serializer)
{
    super::Serialize(serializer);
    serializer >> m_bUpdateClientOrServer;
    serializer >> m_iClientVersion;
}
