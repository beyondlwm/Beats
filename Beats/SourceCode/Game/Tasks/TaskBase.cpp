#include "stdafx.h"
#include "TaskBase.h"
#include "../Game.h"

CTaskBase::CTaskBase(ETaskType type)
: m_type(type)
{
}

CTaskBase::~CTaskBase()
{

}

ETaskType CTaskBase::GetType()
{
    return m_type;
}