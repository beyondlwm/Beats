#include "stdafx.h"
#include "SpyCommandBase.h"

CSpyCommandBase::CSpyCommandBase( ESpyCommandType type, size_t uFeedbackFunc, size_t uUserDataPtr )
: m_type(type)
, m_uFeedbackFunc(uFeedbackFunc)
, m_uUserDataPtr(uUserDataPtr)
{
}

CSpyCommandBase::~CSpyCommandBase()
{

}

ESpyCommandType CSpyCommandBase::GetType() const
{
    return m_type;
}

size_t CSpyCommandBase::GetFeedbackFunc() const
{
    return m_uFeedbackFunc;
}

size_t CSpyCommandBase::GetUserDataPtr() const
{
    return m_uUserDataPtr;
}

void CSpyCommandBase::SetFeedBackFuncAddr(size_t uFeedbackFunc, size_t uUserDataPtr)
{
    m_uFeedbackFunc = uFeedbackFunc;
    m_uUserDataPtr = uUserDataPtr;
}

bool CSpyCommandBase::Execute( SharePtr<SSocketContext>& pSocketContext )
{
    return !pSocketContext->GetBitFlag(eSCFB_Invalid) && ExecuteImpl(pSocketContext);
}

void CSpyCommandBase::Deserialize(CSerializer& serializer)
{
    serializer << m_uFeedbackFunc;
    serializer << m_uUserDataPtr;
}

void CSpyCommandBase::Serialize(CSerializer& serializer)
{
    serializer >> m_uFeedbackFunc;
    serializer >> m_uUserDataPtr;
}
