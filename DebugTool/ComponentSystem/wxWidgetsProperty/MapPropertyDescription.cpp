#include "stdafx.h"
#include "../../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"
#include "../../BDTWxApp.h"
#include "../../BDTWxFrame.h"
#include "MapPropertyDescription.h"
#include "../../Components/Component/ComponentManager.h"

static const TString EMPTY_STRING = _T("Empty");

CMapPropertyDescription::CMapPropertyDescription(CSerializer* pSerializer)
    : super(ePT_Map)
{
    if (pSerializer != NULL)
    {
        (*pSerializer) >> m_keyType;
        BEATS_ASSERT(m_keyType != ePT_Ptr, _T("Key can't be ePT_Ptr! It's not implemented yet!"));
        (*pSerializer) >> m_valueType;
        if (m_valueType == ePT_Ptr)
        {
            (*pSerializer) >> m_uValuePtrGuid;
        }
    }
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::CMapPropertyDescription(const CMapPropertyDescription& rRef)
    : super(rRef)
    , m_keyType(rRef.m_keyType)
    , m_valueType(rRef.m_valueType)
    , m_uValuePtrGuid(rRef.m_uValuePtrGuid)
{
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::~CMapPropertyDescription()
{
    DestoryValue<TString>();
}

wxPGProperty* CMapPropertyDescription::CreateWxProperty()
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


void CMapPropertyDescription::SetValue( wxVariant& value, bool bSaveValue/* = true*/ )
{
    TString strNewValue = value.GetString();
    SetValue(&strNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&strNewValue, eVT_SavedValue);
    }
}

void CMapPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(TString*)m_valueArray[type] = *(TString*)pValue;
}

bool CMapPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
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


CPropertyDescriptionBase* CMapPropertyDescription::AddMapChild()
{
    CPropertyDescriptionBase* pNewChild = CreateInstance();
    if (pNewChild)
    {
        AddChild(pNewChild);
        ResetName();
    }
    return pNewChild;
}

void CMapPropertyDescription::AddMapChild(CPropertyDescriptionBase* pChild)
{
    if (pChild)
    {
        AddChild(pChild);
        ResetName();
    }
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateInstance()
{
    TCHAR szChildName[32];
    _stprintf(szChildName, _T("Child_%d"), m_pChildren->size());
    SBasicPropertyInfo basicInfo = GetBasicInfo();
    basicInfo.m_displayName.assign(szChildName);
    basicInfo.m_variableName.assign(szChildName);

    CPropertyDescriptionBase* pRet = CComponentManager::GetInstance()->CreateProperty(ePT_Str, NULL);
    pRet->Initialize();
    pRet->SetBasicInfo(basicInfo);
    pRet->SetOwner(this->GetOwner());

    CPropertyDescriptionBase* pKey = CComponentManager::GetInstance()->CreateProperty(m_keyType, NULL);
    pKey->Initialize();
    basicInfo.m_displayName.assign(_T("Key"));
    basicInfo.m_variableName.assign(_T("Key"));
    pKey->SetBasicInfo(basicInfo);
    pKey->SetOwner(this->GetOwner());

    CSerializer serializer;
    serializer << m_uValuePtrGuid;
    CPropertyDescriptionBase* pValue = CComponentManager::GetInstance()->CreateProperty(m_valueType, m_valueType == ePT_Ptr ? &serializer : NULL);
    pValue->Initialize();
    basicInfo.m_displayName.assign(_T("Value"));
    basicInfo.m_variableName.assign(_T("Value"));
    pValue->SetBasicInfo(basicInfo);
    pValue->SetOwner(this->GetOwner());

    pRet->AddChild(pKey);
    pRet->AddChild(pValue);
    return pRet;
}


void CMapPropertyDescription::DeleteMapChild(CPropertyDescriptionBase* pProperty)
{
    BEATS_ASSERT((*m_pChildren).size() > 0 && pProperty != NULL);
    bool bRet = DeleteChild(pProperty, true);
    if (bRet)
    {
        ResetChildName();
        ResetName();
    }
    BEATS_ASSERT(bRet, _T("Can't Find the property to delete!"));
}


void CMapPropertyDescription::DeleteAllMapChild()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_SAFE_DELETE((*m_pChildren)[i]);
    }
    m_pChildren->clear();
    ResetName();
}


void CMapPropertyDescription::ResetChildName()
{
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        TCHAR szChildName[32];
        _stprintf(szChildName, _T("Child_%d"), i);
        (*m_pChildren)[i]->GetBasicInfo().m_displayName.assign(szChildName);
        (*m_pChildren)[i]->GetBasicInfo().m_variableName.assign(szChildName);
    }
}


void CMapPropertyDescription::ResetName()
{
    TCHAR szName[64];
    GetCurrentName(szName);
    wxVariant var(szName);
    SetValue(var, false);
}

void CMapPropertyDescription::GetCurrentName( TCHAR* pszName )
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

void CMapPropertyDescription::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    while (pVarElement != NULL)
    {
        int iVarType = 0;
        pVarElement->Attribute("Type", &iVarType);
        if (iVarType == m_valueType)
        {
            CPropertyDescriptionBase* pNewProperty = AddMapChild();
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

CPropertyDescriptionBase* CMapPropertyDescription::Clone(bool bCloneValue)
{
    CMapPropertyDescription* pNewProperty = static_cast<CMapPropertyDescription*>(super::Clone(bCloneValue));
    if (bCloneValue)
    {
        for (size_t i = 0; i < m_pChildren->size(); ++i)
        {
            CPropertyDescriptionBase* pPropertyBase = (*m_pChildren)[i];
            CPropertyDescriptionBase* pNewChildPropertyBase = pPropertyBase->Clone(true);
            pNewProperty->AddMapChild(pNewChildPropertyBase);
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateNewInstance()
{
    CMapPropertyDescription* pNewProperty = new CMapPropertyDescription(*this);
    return pNewProperty;
}

void CMapPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    CStringHelper::GetInstance()->ConvertToCHAR(((TString*)GetValue(type))->c_str(), pOut, 128);
}

void CMapPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << m_keyType;
    serializer << m_valueType;
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->Serialize(serializer);
    }
}

bool CMapPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/, bool /*bSerializePhase = false*/)
{
    return true;
}