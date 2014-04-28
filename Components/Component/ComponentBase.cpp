#include "stdafx.h"
#include "componentbase.h"

CComponentBase::CComponentBase()
: m_id(0xFFFFFFFF)
{
}

CComponentBase::~CComponentBase()
{
}

size_t CComponentBase::GetId() const
{
    return m_id;
}

void CComponentBase::SetId(size_t id)
{
    m_id = id;
}

void CComponentBase::Initialize()
{

}

void CComponentBase::ReflectData(CSerializer& /*serializer*/)
{

}
