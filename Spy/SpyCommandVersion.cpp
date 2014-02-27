#include "stdafx.h"
#include "SpyCommandVersion.h"
#include "SpyCommandMessageBox.h"
#include "SpyCommandManager.h"

CSpyCommandVersion::CSpyCommandVersion()
: super(eSCT_Version)
, m_iCurVersion(BEATS_SPY_VERSION)
, m_bRequst(true)
{

}

CSpyCommandVersion::~CSpyCommandVersion()
{

}

bool CSpyCommandVersion::ExecuteImpl(SharePtr<SSocketContext>& pSocketContext)
{
    if (m_bRequst)
    {
        m_bRequst = false;
        CSpyCommandManager::GetInstance()->SendCommand(pSocketContext, this);
    }
    else
    {
        TCHAR szInfo[MAX_PATH];
        _stprintf(szInfo, _T("Version is %d"), m_iCurVersion);
        MessageBox(NULL, szInfo, _T("Version"), MB_OK);
    }
    return true;
}

void CSpyCommandVersion::Deserialize(CSerializer& serializer)
{
    super::Deserialize(serializer);

    serializer << m_bRequst;
    if (!m_bRequst)
    {
        serializer << m_iCurVersion;
    }
}

void CSpyCommandVersion::Serialize(CSerializer& serializer)
{
    super::Serialize(serializer);
    serializer >> m_bRequst;
    if (!m_bRequst)
    {
        serializer >> m_iCurVersion;
    }
}
