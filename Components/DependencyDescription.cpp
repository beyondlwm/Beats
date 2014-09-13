#include "stdafx.h"
#include "Component/ComponentInstanceManager.h"
#include "Component/ComponentProxyManager.h"
#include "Component/ComponentProxy.h"
#include "DependencyDescription.h"
#include "DependencyDescriptionLine.h"
#include "Utility/TinyXML/tinyxml.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Utility/Serializer/Serializer.h"
#include "Component/ComponentInstance.h"
#include "Component/ComponentReference.h"
#include "Component/ComponentProject.h"

CDependencyDescription::CDependencyDescription(EDependencyType type, size_t dependencyGuid, CComponentProxy* pOwner, size_t uIndex, bool bIsList)
: m_type(type)
, m_changeAction(eDCA_Count)
, m_pChangeActionProxy(NULL)
, m_uIndex(uIndex)
, m_pOwner(pOwner)
, m_uDependencyGuid(dependencyGuid)
, m_bHideRender(false)
, m_bIsListType(bIsList)
{
    m_pOwner->AddDependencyDescription(this);
}

CDependencyDescription::~CDependencyDescription()
{
    // In CDependencyDescriptionLine's destructor function, it will reduce the number in m_pBeConnectedDependencyLines.
    while (m_dependencyLine.size() > 0)
    {
        CDependencyDescriptionLine* pLine = m_dependencyLine[0];
        BEATS_SAFE_DELETE(pLine);
    }
}

CDependencyDescriptionLine* CDependencyDescription::GetDependencyLine( size_t uIndex /*= 0*/ ) const
{
    BEATS_ASSERT(uIndex < m_dependencyLine.size());
    BEATS_ASSERT(uIndex == 0  || (uIndex > 0 && m_bIsListType), _T("Get denpendency by index %d is only available for list type!"), uIndex);
    return m_dependencyLine[uIndex];
}

CDependencyDescriptionLine* CDependencyDescription::SetDependency( size_t uIndex, CComponentProxy* pComponent )
{
    CDependencyDescriptionLine* pRet = NULL;
    BEATS_ASSERT(uIndex < m_dependencyLine.size());
    BEATS_ASSERT(pComponent != m_pOwner, _T("Component can't depends on itself!"));
    BEATS_ASSERT(uIndex == 0  || (uIndex > 0 && m_bIsListType), _T("Set denpendency by index %d is only available for list type!"), uIndex);
    if (pComponent != m_pOwner)
    {
        m_dependencyLine[uIndex]->SetConnectComponent(pComponent);
        pRet = m_dependencyLine[uIndex];
    }
    if (!m_bIsListType)
    {
        m_changeAction = eDCA_Change;
        m_pChangeActionProxy = pComponent;
        OnDependencyChanged();
    }
    return pRet;
}

size_t CDependencyDescription::GetDependencyLineCount() const
{
    BEATS_ASSERT(m_dependencyLine.size() <= 1 || m_bIsListType);
    return m_dependencyLine.size();
}

CDependencyDescriptionLine* CDependencyDescription::AddDependency( CComponentProxy* pComponentInstance )
{
    CDependencyDescriptionLine* pRet = NULL;

#ifdef _DEBUG
    for (size_t i = 0; i < m_dependencyLine.size(); ++i)
    {
        BEATS_ASSERT(pComponentInstance == NULL || pComponentInstance != m_dependencyLine[i]->GetConnectedComponent(), _T("The dependency is already in its list!"));
    }
#endif
    BEATS_ASSERT(pComponentInstance != m_pOwner);
    if (pComponentInstance != m_pOwner)
    {
        BEATS_ASSERT(m_dependencyLine.size() == 0 || m_bIsListType);
        pRet = new CDependencyDescriptionLine(this, m_dependencyLine.size(), pComponentInstance);
        m_dependencyLine.push_back(pRet);
        m_changeAction = eDCA_Add;
        m_pChangeActionProxy = pComponentInstance;
        OnDependencyChanged();
    }
    return pRet;
}

void CDependencyDescription::RemoveDependencyLine(CDependencyDescriptionLine* pLine)
{
    BEATS_ASSERT(pLine->GetOwnerDependency() == this);
    size_t uPos = pLine->GetIndex();
    RemoveDependencyByIndex(uPos);
}

void CDependencyDescription::RemoveDependencyByIndex( size_t uIndex )
{
    std::vector<CDependencyDescriptionLine*>::iterator iter = m_dependencyLine.begin();
    advance(iter, uIndex);
    BEATS_ASSERT(*iter == m_dependencyLine[uIndex]);
    m_pChangeActionProxy = (*iter)->GetConnectedComponent();
    m_dependencyLine.erase(iter);

    for (size_t i = uIndex; i < m_dependencyLine.size(); ++i)
    {
        m_dependencyLine[i]->SetIndex(i);
    }
    m_changeAction = eDCA_Delete;
    OnDependencyChanged();
}

void CDependencyDescription::SwapLineOrder(size_t uSourceIndex, size_t uTargetIndex)
{
    BEATS_ASSERT(uSourceIndex != uTargetIndex);
    BEATS_ASSERT(uSourceIndex < m_dependencyLine.size());
    BEATS_ASSERT(uTargetIndex < m_dependencyLine.size());

    CDependencyDescriptionLine* pSourceLine = m_dependencyLine[uSourceIndex];
    CDependencyDescriptionLine* pTargetLine = m_dependencyLine[uTargetIndex];
    BEATS_ASSERT(pSourceLine != pTargetLine);
    CComponentProxy* pSourceProxy = pSourceLine->GetConnectedComponent(false);
    pSourceLine->SetConnectComponent(pTargetLine->GetConnectedComponent(false));
    pTargetLine->SetConnectComponent(pSourceProxy);
    m_changeAction = eDCA_Ordered;
    m_pChangeActionProxy = pSourceProxy;
    OnDependencyChanged();
}

void CDependencyDescription::SaveToXML( TiXmlElement* pParentNode )
{
    if (m_dependencyLine.size() == 0)
    {
        if (m_type == eDT_Strong && m_pOwner->GetId() != -1)
        {
            TCHAR szInfo[10240];
            _stprintf(szInfo, _T("Component %s with Id:%d, Unset strong dependency!"), m_pOwner->GetClassStr(), m_pOwner->GetId());
            MessageBox(NULL, szInfo, _T("Unset strong dependency"), MB_OK);
        }
    }
    else
    {
        TiXmlElement* pDependencyElement = new TiXmlElement("Dependency");
        char szVariableName[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToCHAR(m_variableName.c_str(), szVariableName, MAX_PATH);
        pDependencyElement->SetAttribute("VariableName", szVariableName);

        for (size_t i = 0; i < m_dependencyLine.size(); ++i)
        {
            // Most of the time, we need to get the real connected component of the reference, as if the reference doesn't exists.
            // However, we need the reference info when we save the component info.
            TiXmlElement* pDependencyNodeElement = new TiXmlElement("DependencyNode");
            CComponentProxy* pProxy = m_dependencyLine[i]->GetConnectedComponent(false);
            pDependencyNodeElement->SetAttribute("Id", (int)pProxy->GetId());
            char szGUIDHexStr[32] = {0};
            sprintf(szGUIDHexStr, "0x%lx", pProxy->GetGuid());
            pDependencyNodeElement->SetAttribute("Guid", szGUIDHexStr);
            pDependencyElement->LinkEndChild(pDependencyNodeElement);
        }

        pParentNode->LinkEndChild(pDependencyElement);
    }
}

void CDependencyDescription::LoadFromXML( TiXmlElement* pNode )
{
    TiXmlElement* pDependencyElement = pNode->FirstChildElement("Dependency");
    while (pDependencyElement != NULL)
    {
        const char* szVariableName = pDependencyElement->Attribute("VariableName");
        TCHAR szVariableNameTCHAR[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(szVariableName, szVariableNameTCHAR, MAX_PATH);

        if (_tcscmp(szVariableNameTCHAR, m_variableName.c_str()) != 0)
        {
            pDependencyElement = pDependencyElement->NextSiblingElement("Dependency");
        }
        else
        {
            TiXmlElement* pDependencyNodeElement = pDependencyElement->FirstChildElement("DependencyNode");
            while (pDependencyNodeElement != NULL)
            {
                const char* szGuid = pDependencyNodeElement->Attribute("Guid");
                char* pEnd = NULL;
                size_t uGuid = strtoul(szGuid, &pEnd, 16);
                const char* szId = pDependencyNodeElement->Attribute("Id");
                size_t uId = (size_t)atoi(szId);
                bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(m_uDependencyGuid, uGuid);
                BEATS_ASSERT(bIsParent, _T("Dependency Not match in component %d"), m_pOwner->GetId());
                if (bIsParent)
                {
                    CComponentProxy* pComponent = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(uId, uGuid));
                    AddDependency(pComponent);
                    if (pComponent == NULL)
                    {
                        CComponentProxyManager::GetInstance()->AddDependencyResolver(this, m_dependencyLine.size() - 1, uGuid, uId, NULL, m_bIsListType);
                    }
                }
                pDependencyNodeElement = pDependencyNodeElement->NextSiblingElement("DependencyNode");
            }
            break;
        }
    }
}

void CDependencyDescription::SetOwner( CComponentProxy* pOwner )
{
    BEATS_ASSERT(m_pOwner == NULL);
    m_pOwner = pOwner;
}

const TCHAR* CDependencyDescription::GetDisplayName()
{
    return m_displayName.c_str();
}

void CDependencyDescription::SetDisplayName( const TCHAR* pszName )
{
    m_displayName.assign(pszName);
}

const TCHAR* CDependencyDescription::GetVariableName()
{
    return m_variableName.c_str();
}

void CDependencyDescription::SetVariableName(const TCHAR* pszName)
{
    m_variableName.assign(pszName);
}

EDependencyType CDependencyDescription::GetType()
{
    return m_type;
}

CComponentProxy* CDependencyDescription::GetOwner()
{
    return m_pOwner;
}
void CDependencyDescription::Hide()
{
    m_bHideRender = true;
}

void CDependencyDescription::Show()
{
    m_bHideRender = false;
}

bool CDependencyDescription::IsVisible() const
{
    return !m_bHideRender;
}

bool CDependencyDescription::IsListType() const
{
    return m_bIsListType;
}

bool CDependencyDescription::IsInDependency( CComponentProxy* pComponentInstance )
{
    bool bRet = false;
    for (size_t i = 0; i < m_dependencyLine.size(); ++i)
    {
        if (m_dependencyLine[i]->GetConnectedComponent() == pComponentInstance)
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool CDependencyDescription::IsMatch( CComponentProxy* pDependencyComponent )
{
    size_t uCurGuid = pDependencyComponent->GetGuid();
    bool bMatch = uCurGuid == m_uDependencyGuid;
    if (!bMatch)
    {
        std::vector<size_t> result;
        CComponentProxyManager::GetInstance()->QueryDerivedClass(m_uDependencyGuid, result, true);
        for (size_t i = 0; i < result.size(); ++i)
        {
            if (result[i] == uCurGuid)
            {
                bMatch = true;
                break;
            }
        }
    }
    return bMatch;
}

void CDependencyDescription::Serialize(CSerializer& serializer)
{
    size_t uLineCount = this->GetDependencyLineCount();
    serializer << uLineCount;
    for (size_t j = 0; j < uLineCount; ++j)
    {
        // No matter if we get proxy or reference, it doesn't matter, so we get the proxy exactly.
        CComponentProxy* pConnectedComponent = this->GetDependencyLine(j)->GetConnectedComponent(false);
        BEATS_ASSERT(pConnectedComponent != NULL);
        serializer << pConnectedComponent->GetProxyId();
        serializer << pConnectedComponent->GetGuid();
    }
}

void CDependencyDescription::GetCurrActionParam(EDependencyChangeAction& action, CComponentProxy*& pProxy)
{
    action = m_changeAction;
    pProxy = m_pChangeActionProxy;
}

void CDependencyDescription::OnDependencyChanged()
{
    if (this->GetOwner()->IsInitialized())
    {
        bool bIsReady = true;
        for (size_t i = 0; i < m_dependencyLine.size(); ++i)
        {
            if (m_dependencyLine[i]->GetConnectedComponent() == NULL)
            {
                bIsReady = false;
                break;
            }
        }
        if (bIsReady)
        {
            if (GetOwner()->GetHostComponent())
            {
                static CSerializer serializer;
                serializer.Reset();
                Serialize(serializer);
                CComponentProxyManager::GetInstance()->SetCurrReflectDependency(this);
                BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrReflectDescription() == NULL);
                CComponentProxyManager::GetInstance()->SetReflectCheckFlag(true);
                GetOwner()->GetHostComponent()->ReflectData(serializer);
                CComponentInstanceManager::GetInstance()->ResolveDependency();
                CComponentProxyManager::GetInstance()->SetReflectCheckFlag(false);
                CComponentProxyManager::GetInstance()->SetCurrReflectDependency(NULL);
            }
        }
    }
}

size_t CDependencyDescription::GetDependencyGuid() const
{
    return m_uDependencyGuid;
}

size_t CDependencyDescription::GetIndex() const
{
    return m_uIndex;
}
