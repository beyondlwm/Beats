#ifndef BEATS_SPY_COMMAND_SPYCOMMANDMESSAGEBOX_H__INCLUDE
#define BEATS_SPY_COMMAND_SPYCOMMANDMESSAGEBOX_H__INCLUDE

#include "SpyCommandBase.h"

class CSpyCommandMessageBox : public CSpyCommandBase
{
    typedef CSpyCommandBase super;
public:
    CSpyCommandMessageBox();
    virtual ~CSpyCommandMessageBox();

    void SetMessage(const TCHAR* pszMsg);
    void SetCaption(const TCHAR* pszCaption);

    virtual bool ExecuteImpl(SharePtr<SSocketContext>&  pSocketContext);
    virtual void Deserialize(CSerializer& serializer);
    virtual void Serialize(CSerializer& serializer);

private:
    TString m_message;
    TString m_caption;
};
#endif