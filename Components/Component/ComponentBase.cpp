#include "stdafx.h"
#include "componentbase.h"
#include "../../Utility/Serializer/Serializer.h"
#include "../DependencyDescription.h"
#include "../DependencyDescriptionLine.h"
#include "../Property/PropertyDescriptionBase.h"

CComponentBase::CComponentBase()
: m_id(0xFFFFFFFF)
{
}

CComponentBase::CComponentBase(CSerializer& /*serializer*/)
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
