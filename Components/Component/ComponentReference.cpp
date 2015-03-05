#include "stdafx.h"
#include "ComponentReference.h"
#include "Utility/TinyXML/tinyxml.h"
#include "ComponentGraphic.h"

CComponentReference::CComponentReference(uint32_t uProxyId, uint32_t uProxyGuid, CComponentGraphic* pGraphics)
    : m_uReferenceId(uProxyId)
    , m_uReferenceGuid(uProxyGuid)
    , m_pHostProxy(NULL)
{
    SetGraphics(pGraphics);
    pGraphics->SetReferenceFlag(true);
}

CComponentReference::CComponentReference(CComponentProxy* pComponentHost)
    : m_pHostProxy(pComponentHost)
    , m_uReferenceGuid(m_pHostProxy->GetGuid())
    , m_uReferenceId(m_pHostProxy->GetId())
{
    BEATS_ASSERT(m_pHostProxy != NULL);
    SetGraphics(pComponentHost->GetGraphics()->Clone());
}

CComponentReference::~CComponentReference()
{
}

CComponentProxy* CComponentReference::GetHostProxy()
{
    return m_pHostProxy;
}

uint32_t CComponentReference::GetProxyId()
{
    return m_uReferenceId;
}

uint32_t CComponentReference::GetGuid() const
{
    return m_uReferenceGuid;
}

uint32_t CComponentReference::GetParentGuid() const
{
    uint32_t uParentGuid = 0xFFFFFFFF;
    if (m_pHostProxy != NULL)
    {
        uParentGuid = m_pHostProxy->GetParentGuid();
    }
    return uParentGuid;
}

const TCHAR* CComponentReference::GetClassStr() const
{
    const TCHAR* pszRet = _T("Reference");
    if (m_pHostProxy != NULL)
    {
        pszRet = m_pHostProxy->GetClassStr();
    }
    return pszRet;
}

void CComponentReference::SaveToXML(TiXmlElement* pNode, bool bSaveOnlyNoneNativePart/* = false*/)
{
    TiXmlElement* pInstanceElement = new TiXmlElement("Reference");
    pInstanceElement->SetAttribute("Id", (int)GetId());
    int posX = 0;
    int posY = 0;
    if (GetGraphics())
    {
        GetGraphics()->GetPosition(&posX, &posY);
    }
    pInstanceElement->SetAttribute("PosX", posX);
    pInstanceElement->SetAttribute("PosY", posY);
    pInstanceElement->SetAttribute("ReferenceId", m_uReferenceId);
    if (GetUserDefineDisplayName().length() > 0)
    {
        pInstanceElement->SetAttribute("UserDefineName", GetUserDefineDisplayName().c_str());
    }
    pNode->LinkEndChild(pInstanceElement);
}

void CComponentReference::LoadFromXML(TiXmlElement* pNode)
{
    int x = 0;
    int y = 0;
    if (pNode->Attribute("PosX", &x) && pNode->Attribute("PosY", &y))
    {
        GetGraphics()->SetPosition(x, y);
    }

    const char* pszUserDefineName = pNode->Attribute("UserDefineName");
    if (pszUserDefineName != NULL)
    {
        SetUserDefineDisplayName(pszUserDefineName);
    }
}

void CComponentReference::Initialize()
{
    CComponentBase::Initialize();
    // ComponentReference's host proxy could(maybe must) be NULL in exporting phase.
    bool bIsExporting = CComponentProxyManager::GetInstance()->IsExporting();
    if (m_pHostProxy == NULL)
    {
        m_pHostProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(m_uReferenceId, m_uReferenceGuid));
        BEATS_ASSERT(m_pHostProxy != NULL || bIsExporting);
    }
#ifdef _DEBUG
    else
    {
        BEATS_ASSERT(m_pHostProxy->GetGuid() == m_uReferenceGuid && m_pHostProxy->GetId() == m_uReferenceId);
    }
#endif
    if (m_pHostProxy != NULL)
    {
        m_pProperties = (std::vector<CPropertyDescriptionBase*>*)m_pHostProxy->GetPropertyPool();
        m_pHostComponent = m_pHostProxy->GetHostComponent();
        SetDisplayName(m_pHostProxy->GetDisplayName().c_str());
        SetCatalogName(m_pHostProxy->GetCatalogName().c_str());
    }
}

void CComponentReference::Uninitialize()
{
    m_pProperties = NULL;
    m_pHostComponent = NULL;
    super::Uninitialize();
    CComponentProxyManager::GetInstance()->UnregisterComponentReference(this);
}
