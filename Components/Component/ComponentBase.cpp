#include "stdafx.h"
#include "componentbase.h"

CComponentBase::CComponentBase()
: m_bInitialize(false)
, m_id(0xFFFFFFFF)
{
}

CComponentBase::~CComponentBase()
{
    BEATS_ASSERT(!m_bInitialize, _T("Call Uninitialize before delete the component!"));
}

size_t CComponentBase::GetId() const
{
    return m_id;
}

void CComponentBase::SetId(size_t id)
{
    m_id = id;
}

bool CComponentBase::IsInitialized()
{
    return m_bInitialize;
}

void CComponentBase::Initialize()
{
    m_bInitialize = true;
}

void CComponentBase::Uninitialize()
{
    m_bInitialize = false;
}

void CComponentBase::ReflectData(CSerializer& /*serializer*/)
{

}

bool CComponentBase::OnPropertyChange(void* /*pVariableAddr*/, void* /*pNewValueToBeSet*/)
{
    return false;
}