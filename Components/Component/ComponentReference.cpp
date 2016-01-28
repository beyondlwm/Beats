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
    rapidxml::xml_document<>* doc = pNode->document();
    rapidxml::xml_node<>* pInstanceElement = doc->allocate_node(rapidxml::node_element, "Reference");
    _stprintf(szBeatsDialogBuffer, "%d", GetId());
    pInstanceElement->append_attribute(doc->allocate_attribute("Id", doc->allocate_string(szBeatsDialogBuffer)));
    int posX = 0;
    int posY = 0;
    if (GetGraphics())
    {
        GetGraphics()->GetPosition(&posX, &posY);
    }
    _stprintf(szBeatsDialogBuffer, "%d", posX);
    pInstanceElement->append_attribute(doc->allocate_attribute("PosX", doc->allocate_string(szBeatsDialogBuffer)));
    _stprintf(szBeatsDialogBuffer, "%d", posY);
    pInstanceElement->append_attribute(doc->allocate_attribute("PosY", doc->allocate_string(szBeatsDialogBuffer)));
    _stprintf(szBeatsDialogBuffer, "%d", m_uReferenceId);
    pInstanceElement->append_attribute(doc->allocate_attribute("ReferenceId", doc->allocate_string(szBeatsDialogBuffer)));
    if (GetUserDefineDisplayName().length() > 0)
    {
        pInstanceElement->append_attribute(doc->allocate_attribute("UserDefineName", doc->allocate_string(GetUserDefineDisplayName().c_str())));
    }
    pNode->append_node(pInstanceElement);
}

void CComponentReference::LoadFromXML(rapidxml::xml_node<>* pNode)
{
    if (pNode->first_attribute("PosX") && pNode->first_attribute("PosY"))
    {
        GetGraphics()->SetPosition(atoi(pNode->first_attribute("PosX")->value()), atoi(pNode->first_attribute("PosY")->value()));
    }
    if (pNode->first_attribute("UserDefineName") != nullptr)
    {
        SetUserDefineDisplayName(pNode->first_attribute("UserDefineName")->value());
    }
}

void CComponentReference::Initialize()
{
    CComponentBase::Initialize();
    if (m_pHostProxy == NULL)
    {
        m_pHostProxy = static_cast<CComponentProxy*>(CComponentProxyManager::GetInstance()->GetComponentInstance(m_uReferenceId, m_uReferenceGuid));
        // ComponentReference's host proxy could(maybe must) be NULL in exporting phase.
        BEATS_ASSERT(m_pHostProxy != NULL || CComponentProxyManager::GetInstance()->IsExporting());
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
