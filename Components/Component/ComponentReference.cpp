#include "stdafx.h"
#include "ComponentReference.h"
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

void CComponentReference::Save()
{
    // Do nothing.
}

void CComponentReference::SaveToXML(rapidxml::xml_node<>* pNode, bool bSaveOnlyNoneNativePart/* = false*/)
{
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* pInstanceElement = doc.allocate_node(rapidxml::node_element, "Reference");
    pInstanceElement->append_attribute(doc.allocate_attribute("Id", std::to_string(GetId()).c_str()));
    int posX = 0;
    int posY = 0;
    if (GetGraphics())
    {
        GetGraphics()->GetPosition(&posX, &posY);
    }
    pInstanceElement->append_attribute(doc.allocate_attribute("PosX", std::to_string(posX).c_str()));
    pInstanceElement->append_attribute(doc.allocate_attribute("PosY", std::to_string(posY).c_str()));
    pInstanceElement->append_attribute(doc.allocate_attribute("ReferenceId", std::to_string(m_uReferenceId).c_str()));
    if (GetUserDefineDisplayName().length() > 0)
    {
        pInstanceElement->append_attribute(doc.allocate_attribute("UserDefineName", GetUserDefineDisplayName().c_str()));
    }
    pNode->append_node(pInstanceElement);
}

void CComponentReference::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    int x = 0;
    int y = 0;
    x = atoi((char*)pNode->first_attribute("PosX"));
    y = atoi((char*)pNode->first_attribute("PosY"));
    if (x && y)
    {
        GetGraphics()->SetPosition(x, y);
    }

    const char* pszUserDefineName = (char*)pNode->first_attribute("UserDefineName");
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
