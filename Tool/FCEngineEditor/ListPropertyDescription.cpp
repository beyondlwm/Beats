#include "stdafx.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "EngineEditor.h"
#include "ListPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/TinyXML/tinyxml.h"

static const TString EMPTY_STRING = _T("Empty");

CListPropertyDescription::CListPropertyDescription(CSerializer* pSerializer)
: super(ePT_List)
, m_uMaxCount(INT_MAX)
, m_bFixCount(false)
{
    if (pSerializer != NULL)
    {
        (*pSerializer) >> m_childType;
        if (m_childType == ePT_Ptr)
        {
            (*pSerializer) >> m_childPtrGuid;
        }
    }
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescription::CListPropertyDescription(const CListPropertyDescription& rRef)
: super(rRef)
, m_uMaxCount(rRef.m_uMaxCount)
, m_bFixCount(rRef.m_bFixCount)
, m_childPtrGuid(rRef.m_childPtrGuid)
, m_childType(rRef.m_childType)
{
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescription::~CListPropertyDescription()
{
    DestoryValue<TString>();
}

bool CListPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result, bool /* bSerializePhase= false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache, false);
        BEATS_ASSERT(cache.size() == 2 || cache.size() == 1);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxCount]) == 0)
        {
            m_uMaxCount = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_FixCount]) == 0)
        {
            if (_tcsicmp(cache[1].c_str(), _T("true")) == 0)
            {
                m_bFixCount = true;
            }
            else if (_tcsicmp(cache[1].c_str(), _T("false")) == 0)
            {
                m_bFixCount = false;
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown value for List property %s"), cache[1].c_str());
            }
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            // Do nothing, because it is only for UI text display.
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for List property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CListPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pProperty->SetClientData(this);
    if (!m_bFixCount)
    {
        pProperty->SetEditor(static_cast<CEngineEditor*>(wxApp::GetInstance())->GetPtrEditor());
    }
    wxString strName;
    GetCurrentName(strName);
    wxVariant var(strName);
    pProperty->SetValue(var);
    wxVariant defaultVar(EMPTY_STRING);
    pProperty->SetDefaultValue(defaultVar);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}


void CListPropertyDescription::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValue(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&strNewValue, eVT_SavedValue);
    }
}

void CListPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(TString*)m_valueArray[type] = *(TString*)pValue;
}

bool CListPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = false;
    if (bWithDefaultOrXML)
    {
        bRet = m_pChildren->size() == 0;
    }
    else
    {
        bRet = false;
    }
    ResetName();
    return bRet;
}


bool CListPropertyDescription::IsFixed() const
{
    return m_bFixCount;
}

void CListPropertyDescription::SetFixed(bool bFixedFlag)
{
    m_bFixCount = bFixedFlag;
}

size_t CListPropertyDescription::GetMaxCount() const
{
    return m_uMaxCount;
}

void CListPropertyDescription::SetMaxCount(size_t uMaxCount)
{
    m_uMaxCount = uMaxCount;
}

EPropertyType CListPropertyDescription::GetChildType() const
{
    return m_childType;
}

void CListPropertyDescription::SetChildType(EPropertyType type)
{
    m_childType = type;
}
CPropertyDescriptionBase* CListPropertyDescription::CreateInstance()
{
    CPropertyDescriptionBase* bRet = NULL;
    if (m_pChildren->size() < m_uMaxCount)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), m_pChildren->size());
        SBasicPropertyInfo basicInfo = GetBasicInfo();
        basicInfo.m_displayName.assign(szChildName);
        basicInfo.m_variableName.assign(szChildName);
        CSerializer serializer;
        serializer << m_childPtrGuid;
        CPropertyDescriptionBase* pProperty = CComponentManager::GetInstance()->CreateProperty(m_childType, m_childType == ePT_Ptr ? &serializer : NULL);
        pProperty->Initialize();
        pProperty->SetBasicInfo(basicInfo);
        pProperty->SetOwner(this->GetOwner());
        pProperty->SetParent(this);
        bRet = pProperty;
    }
    return bRet;
}

bool CListPropertyDescription::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CListPropertyDescription::AddChild(CPropertyDescriptionBase* pProperty)
{
    if (pProperty == NULL)
    {
        pProperty = CreateInstance();
    }
    super::AddChild(pProperty);
    ResetName();
    return pProperty;
}

bool CListPropertyDescription::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder)
{
    BEATS_ASSERT((*m_pChildren).size() > 0 && pProperty != NULL);
    bool bRet = super::DeleteChild(pProperty, bKeepOrder);
    if (bRet)
    {
        ResetChildName();
        ResetName();
    }
    BEATS_ASSERT(bRet, _T("Can't Find the property to delete!"));
    return bRet;
}

void CListPropertyDescription::DeleteAllChild()
{
    super::DeleteAllChild();
    ResetName();
}

void CListPropertyDescription::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo().m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo().m_variableName.assign(szChildName);
    }
}

void CListPropertyDescription::ResetName()
{
    wxString strName;
    GetCurrentName(strName);
    wxVariant var(strName);
    SetValue(var, false);
}

void CListPropertyDescription::GetCurrentName( wxString& strName )
{
    if (m_pChildren->size() == 0)
    {
        strName = EMPTY_STRING;
    }
    else
    {
        strName = wxString::Format(_T("%dx%s"), m_pChildren->size(), szPropertyTypeStr[(*m_pChildren)[0]->GetType()]);
    }
}

void CListPropertyDescription::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        if (iVarType == m_childType)
        {
            CPropertyDescriptionBase* pNewProperty = AddChild(NULL);
            BEATS_ASSERT(pNewProperty != 0, _T("Create property failed when load from xml for list property description."));
            if (pNewProperty != NULL)
            {
                pNewProperty->LoadFromXML(pVarElement);
            }
        }
        else
        {
            BEATS_WARNING(false, _T("UnMatch type of property!"));
        }
        pVarElement = pVarElement->NextSiblingElement("VariableNode");
    }
}

CPropertyDescriptionBase* CListPropertyDescription::Clone(bool bCloneValue)
{
    CListPropertyDescription* pNewProperty = static_cast<CListPropertyDescription*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (size_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            pNewProperty->AddChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CListPropertyDescription::CreateNewInstance()
{
    CListPropertyDescription* pNewProperty = new CListPropertyDescription(*this);
    return pNewProperty;
}

void CListPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

void CListPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << m_childType;
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer);
    }
}
