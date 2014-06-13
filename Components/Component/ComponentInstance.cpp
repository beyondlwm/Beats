#include "stdafx.h"
#include "ComponentInstance.h"
#include "ComponentInstanceManager.h"
#include "IdManager/IdManager.h"

CComponentInstance::CComponentInstance()
{

}

CComponentInstance::~CComponentInstance()
{
}

void CComponentInstance::Uninitialize()
{
    super::Uninitialize();
    size_t uComponentId = GetId();
    if (uComponentId != 0xFFFFFFFF)
    {
        CComponentInstanceManager::GetInstance()->UnregisterInstance(this);
        CComponentInstanceManager::GetInstance()->GetIdManager()->RecycleId(uComponentId);
    }
}
