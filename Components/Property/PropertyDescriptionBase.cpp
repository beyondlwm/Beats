#include "stdafx.h"
#include "PropertyDescriptionBase.h"
#include "Utility/Serializer/Serializer.h"
#include "Utility/SharePtr/SharePtr.h"
#include "Component/ComponentProxy.h"
#include "Component/ComponentProxyManager.h"

CPropertyDescriptionBase::CPropertyDescriptionBase(EReflectPropertyType type)
: m_bInitialized(false)
, m_bHide(false)
, m_bNoSyncToHost(false)
, m_type(type)
, m_pOwner(NULL)
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(new SBasicPropertyInfo(true, 0xFFFFFFFF)))
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
{
    for (uint32_t i = 0; i < eVT_Count; ++i)
    {
        m_valueArray[i] = NULL;
    }
}

CPropertyDescriptionBase::CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef)
: m_bInitialized(false)
, m_bHide(rRef.IsHide())
, m_bNoSyncToHost(rRef.m_bNoSyncToHost)
, m_type(rRef.GetType())
, m_pOwner(NULL)
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(*rRef.m_pBasicInfo))
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
{
    for (uint32_t i = 0; i < eVT_Count; ++i)
    {
        m_valueArray[i] = NULL;
    }
}

CPropertyDescriptionBase::~CPropertyDescriptionBase()
{
    BEATS_SAFE_DELETE(m_pBasicInfo);
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        if ((*m_pChildren)[i]->IsInitialized())
        {
            (*m_pChildren)[i]->Uninitialize();
        }
        BEATS_SAFE_DELETE((*m_pChildren)[i]);
    }
    BEATS_SAFE_DELETE(m_pChildren);
    BEATS_ASSERT(!IsInitialized(), _T("Call uninitialize before delete a CPropertyDescriptionBase"));
}

EReflectPropertyType CPropertyDescriptionBase::GetType() const
{
    return m_type;
}

void CPropertyDescriptionBase::SetType(EReflectPropertyType type)
{
    m_type = type;
}

CComponentProxy* CPropertyDescriptionBase::GetOwner() const
{
    return m_pOwner;
}

void CPropertyDescriptionBase::SetOwner(CComponentProxy* pOwner)
{
    m_pOwner = pOwner;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::GetParent() const
{
    return m_pParent;
}

void CPropertyDescriptionBase::SetParent(CPropertyDescriptionBase* pParent)
{
    BEATS_ASSERT(m_pParent == NULL);
    m_pParent = pParent;
}

void CPropertyDescriptionBase::SetHide(bool bHide)
{
    m_bHide = bHide;
}

bool CPropertyDescriptionBase::IsHide() const
{
    return m_bHide;
}

void CPropertyDescriptionBase::ResetBasicInfo(const SharePtr<SBasicPropertyInfo>& pInfo)
{
    *m_pBasicInfo = pInfo;
}

void CPropertyDescriptionBase::SetBasicInfo(const SBasicPropertyInfo& info)
{
    *m_pBasicInfo->Get() = info;
}

const SharePtr<SBasicPropertyInfo>& CPropertyDescriptionBase::GetBasicInfo() const
{
    return *m_pBasicInfo;
}

bool CPropertyDescriptionBase::DeserializeBasicInfo(CSerializer& serializer)
{
    TCHAR* buff = NULL;
    TCHAR** pbuff = &buff;
    serializer >> (*m_pBasicInfo)->m_bEditable;
    serializer >> (*m_pBasicInfo)->m_color;
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_displayName.assign(*pbuff);
    serializer.Read(pbuff); 
    (*m_pBasicInfo)->m_catalog.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_tip.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_variableName.assign(*pbuff);

    return true;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::InsertChild(CPropertyDescriptionBase* pProperty, uint32_t uPreIndex)
{
#ifdef _DEBUG
    for (uint32_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_ASSERT((*m_pChildren)[i] != pProperty);
    }
#endif
    BEATS_ASSERT(pProperty != NULL);
    pProperty->m_pParent = this;
    if (uPreIndex == 0xFFFFFFFF)
    {
        m_pChildren->push_back(pProperty);
    }
    else
    {
        BEATS_ASSERT(uPreIndex < m_pChildren->size());
        std::vector<CPropertyDescriptionBase*>::iterator preIter = m_pChildren->begin();
        std::advance(preIter, uPreIndex);
        m_pChildren->insert(preIter, pProperty);
    }
    return pProperty;
}

bool CPropertyDescriptionBase::RemoveChild(CPropertyDescriptionBase* pProperty, bool bDelete)
{
    auto iter = std::find(m_pChildren->begin(), m_pChildren->end(), pProperty);
    BEATS_ASSERT(iter != m_pChildren->end());
    m_pChildren->erase(iter);
    if (bDelete)
    {
        pProperty->Uninitialize();
        BEATS_SAFE_DELETE(pProperty);
    }
    return true;
}

void CPropertyDescriptionBase::RemoveAllChild(bool bDelete)
{
    if (bDelete)
    {
        for (uint32_t i = 0; i < m_pChildren->size(); ++i)
        {
            (*m_pChildren)[i]->Uninitialize();
            BEATS_SAFE_DELETE((*m_pChildren)[i]);
        }
    }
    m_pChildren->clear();
}

const std::vector<CPropertyDescriptionBase*>& CPropertyDescriptionBase::GetChildren() const
{
    return *m_pChildren;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::Clone(bool bCloneValue)
{
    CPropertyDescriptionBase* pNewProperty = this->CreateNewInstance();
    pNewProperty->SetValueWithType(m_valueArray[eVT_DefaultValue], eVT_DefaultValue);
    pNewProperty->SetValueWithType(bCloneValue ? m_valueArray[eVT_SavedValue] : m_valueArray[eVT_DefaultValue], eVT_SavedValue);
    pNewProperty->SetValueWithType(bCloneValue ? m_valueArray[eVT_CurrentValue] : m_valueArray[eVT_DefaultValue], eVT_CurrentValue);
    return pNewProperty;
}

void* CPropertyDescriptionBase::GetValue( EValueType type ) const
{
    return m_valueArray[type];
}

bool CPropertyDescriptionBase::CanSyncToHost() const
{
    bool bRet = !m_bNoSyncToHost;
    if (bRet && GetParent() != NULL)
    {
        bRet = GetParent()->CanSyncToHost();
    }
    return bRet;
}

void CPropertyDescriptionBase::Save()
{
    SetValueWithType(m_valueArray[eVT_CurrentValue], eVT_SavedValue);
}

void CPropertyDescriptionBase::Initialize()
{
    BEATS_ASSERT(!m_bInitialized, _T("Can't initialize a property twice!"));
    m_bInitialized = true;
}

void CPropertyDescriptionBase::Uninitialize()
{
    BEATS_ASSERT(m_bInitialized, _T("Can't Uninitialize a property twice!"));
    m_bInitialized = false;
}

bool CPropertyDescriptionBase::IsInitialized() const
{
    return m_bInitialized;
}

bool CPropertyDescriptionBase::IsContainerProperty()
{
    return false;
}

void CPropertyDescriptionBase::SetValueList(const std::vector<TString>& /*valueList*/)
{
    // Do nothing.
}

CComponentProxy* CPropertyDescriptionBase::GetInstanceComponent() const
{
    return NULL;
}

void CPropertyDescriptionBase::SetValueWithType(void* pValue, EValueType type, bool bForceUpdateHostComponent/* = false*/)
{
    if (CopyValue(pValue, m_valueArray[type]) || bForceUpdateHostComponent)
    {
        // We don't need to sync the property in loading phase, since after all components are loaded from XML
        // we will call CComponentProxy::UpdateHostComponent for total.
        bool bLoadingPhase = CComponentProxyManager::GetInstance()->IsLoadingFile();
        if (!bLoadingPhase && CanSyncToHost() && type == eVT_CurrentValue && GetOwner())
        {
            // Some property is not the real property, such as container property.
            CPropertyDescriptionBase* pRealProperty = this;
            while (pRealProperty->GetParent() != NULL && pRealProperty->GetOwner() == pRealProperty->GetParent()->GetOwner())
            {
                pRealProperty = pRealProperty->GetParent();
            }

            CComponentBase* pHostComponent = pRealProperty->GetOwner()->GetHostComponent();
            CPropertyDescriptionBase* pRootProperty = this;
            while (pRootProperty->GetParent() != NULL)
            {
                pRootProperty = pRootProperty->GetParent();
            }
            // Don't reflect for template component because we only want to change the value of it.
            // Template component will never take effect on logic.
            bool bIsTemplateProperty = pRootProperty->GetOwner()->GetTemplateFlag();
            if (pHostComponent && !bIsTemplateProperty)
            {
                CSerializer serializer;
                serializer.Reset();
                pRealProperty->Serialize(serializer, eVT_CurrentValue);

                // Record the original value to avoid wrong set in recursive call.
                CPropertyDescriptionBase* pOriginalCheckProperty = CComponentProxyManager::GetInstance()->GetCurrReflectDescription();
                bool bOriginalCheckFlag = CComponentProxyManager::GetInstance()->GetReflectCheckFlag();

                CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pRealProperty);
                CComponentProxyManager::GetInstance()->SetReflectCheckFlag(true);
                pHostComponent->ReflectData(serializer);
                const std::vector<CComponentInstance*>& syncComponents = pRealProperty->GetOwner()->GetSyncComponents();
                for (uint32_t i = 0; i < syncComponents.size(); ++i)
                {
                    serializer.SetReadPos(0);
                    syncComponents[i]->ReflectData(serializer);
                }
                // Restore the content.
                CComponentProxyManager::GetInstance()->SetCurrReflectDescription(pOriginalCheckProperty);
                CComponentProxyManager::GetInstance()->SetReflectCheckFlag(bOriginalCheckFlag);
            }
        }
    }
}

void CPropertyDescriptionBase::SetNoSyncHost(bool bValue)
{
    m_bNoSyncToHost = bValue;
}
