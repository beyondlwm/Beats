#include "stdafx.h"
#include "../../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"
#include "../../BDTWxApp.h"
#include "../../BDTWxFrame.h"
#include "ListPropertyDescriptionEx.h"
#include "../../Components/Component/ComponentManager.h"

static const TString EMPTY_STRING = _T("Empty");

CListPropertyDescriptionEx::CListPropertyDescriptionEx(CSerializer* pSerializer)
: super(ePT_List)
, m_maxCount(INT_MAX)
, m_bFixCount(false)
{
    if (pSerializer != NULL)
    {
        EPropertyType childType;
        (*pSerializer) >> childType;
        m_pChildTemplate = CComponentManager::GetInstance()->CreateProperty(childType, pSerializer);
    }
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescriptionEx::CListPropertyDescriptionEx(const CListPropertyDescriptionEx& rRef)
: super(rRef)
, m_maxCount(rRef.m_maxCount)
, m_bFixCount(rRef.m_bFixCount)
, m_pChildTemplate(rRef.m_pChildTemplate->Clone(true))
{
    InitializeValue(EMPTY_STRING);
}

CListPropertyDescriptionEx::~CListPropertyDescriptionEx()
{
    DestoryValue<TString>();
}

bool CListPropertyDescriptionEx::AnalyseUIParameterImpl(const std::vector<TString>& result, bool /* bSerializePhase= false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache, false);
        BEATS_ASSERT(cache.size() == 2 || cache.size() == 1);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxCount]) == 0)
        {
            m_maxCount = _tstoi(cache[1].c_str());
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

wxPGProperty* CListPropertyDescriptionEx::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL);
    pProperty->SetClientData(this);
    pProperty->SetEditor(CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetPtrEditor());
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    pProperty->SetValue(var);
    wxVariant defaultVar(EMPTY_STRING);
    pProperty->SetDefaultValue(defaultVar);
    pProperty->SetModifiedStatus(m_pChildren->size() > 0);
    return pProperty;
}


void CListPropertyDescriptionEx::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValue(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&strNewValue, eVT_SavedValue);
    }
}

void CListPropertyDescriptionEx::SetValue( void* pValue, EValueType type )
{
    *(TString*)m_valueArray[type] = *(TString*)pValue;
}

bool CListPropertyDescriptionEx::IsDataSame( bool bWithDefaultOrXML )
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
    return bRet;
}

CPropertyDescriptionBase* CListPropertyDescriptionEx::CreateInstance()
{
    CPropertyDescriptionBase* bRet = NULL;
    if (m_pChildren->size() < m_maxCount)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), m_pChildren->size());
        SBasicPropertyInfo basicInfo = GetBasicInfo();
        basicInfo.m_displayName.assign(szChildName);
        basicInfo.m_variableName.assign(szChildName);
        CPropertyDescriptionBase* pProperty = m_pChildTemplate->Clone(true);
        pProperty->Initialize();
        pProperty->SetBasicInfo(basicInfo);
        pProperty->SetOwner(this->GetOwner());
        pProperty->SetParent(this);
        bRet = pProperty;
    }
    return bRet;
}

bool CListPropertyDescriptionEx::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CListPropertyDescriptionEx::AddChild(CPropertyDescriptionBase* pProperty)
{
    if (pProperty == NULL)
    {
        pProperty = CreateInstance();
    }
    super::AddChild(pProperty);
    ResetName();
    return pProperty;
}

bool CListPropertyDescriptionEx::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder)
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

void CListPropertyDescriptionEx::DeleteAllChild()
{
    super::DeleteAllChild();
    ResetName();
}

void CListPropertyDescriptionEx::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo().m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo().m_variableName.assign(szChildName);
    }
}

void CListPropertyDescriptionEx::ResetName()
{
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    SetValue(var, false);
}

void CListPropertyDescriptionEx::GetCurrentName( TCHAR* pszName )
{
    if (m_pChildren->size() == 0)
    {
        _stprintf(pszName, EMPTY_STRING.c_str());
    }
    else
    {
        _stprintf(pszName, _T("%dx%s"), m_pChildren->size(), szPropertyTypeStr[(*m_pChildren)[0]->GetType()]);
    }
}

void CListPropertyDescriptionEx::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        if (iVarType == m_pChildTemplate->GetType())
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

CPropertyDescriptionBase* CListPropertyDescriptionEx::Clone(bool bCloneValue)
{
    CListPropertyDescriptionEx* pNewProperty = static_cast<CListPropertyDescriptionEx*>(super::Clone(bCloneValue));
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

CPropertyDescriptionBase* CListPropertyDescriptionEx::CreateNewInstance()
{
    CListPropertyDescriptionEx* pNewProperty = new CListPropertyDescriptionEx(*this);
    return pNewProperty;
}

void CListPropertyDescriptionEx::GetValueAsChar( EValueType type, char* pOut )
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

void CListPropertyDescriptionEx::Serialize( CSerializer& serializer )
{
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer);
    }
}
