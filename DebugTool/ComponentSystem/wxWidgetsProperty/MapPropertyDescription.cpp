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
    , m_pKeyPropertyTemplate(NULL)
    , m_pValuePropertyTemplate(NULL)
    , m_uValuePtrGuid(0xFFFFFFFF)
{
    if (pSerializer != NULL)
    {
        EPropertyType keyType;
        (*pSerializer) >> keyType;
        BEATS_ASSERT(keyType != ePT_Ptr, _T("Key can't be ePT_Ptr! It's not implemented yet!"));
        m_pKeyPropertyTemplate = CComponentManager::GetInstance()->CreateProperty(keyType, pSerializer);
        EPropertyType valueType;
        (*pSerializer) >> valueType;
        m_pValuePropertyTemplate = CComponentManager::GetInstance()->CreateProperty(valueType, pSerializer);
    }
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::CMapPropertyDescription(const CMapPropertyDescription& rRef)
    : super(rRef)
    , m_pKeyPropertyTemplate(rRef.m_pKeyPropertyTemplate->Clone(true))
    , m_pValuePropertyTemplate(rRef.m_pValuePropertyTemplate->Clone(true))
    , m_uValuePtrGuid(rRef.m_uValuePtrGuid)
{
    InitializeValue(EMPTY_STRING);
}

CMapPropertyDescription::~CMapPropertyDescription()
{
    BEATS_SAFE_DELETE(m_pKeyPropertyTemplate);
    BEATS_SAFE_DELETE(m_pValuePropertyTemplate);
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
    return bWithDefaultOrXML && m_pChildren->size() == 0;
}

bool CMapPropertyDescription::IsContainerProperty()
{
    return true;
}

CPropertyDescriptionBase* CMapPropertyDescription::AddChild(CPropertyDescriptionBase* pChild)
{
    if (pChild == NULL)
    {
        pChild = CreateInstance();
    }
    super::AddChild(pChild);
    ResetName();

    return pChild;
}

CPropertyDescriptionBase* CMapPropertyDescription::CreateInstance()
{
    TCHAR szChildName[32];
    _stprintf(szChildName, _T("Child_%d"), m_pChildren->size());
    SBasicPropertyInfo basicInfo = GetBasicInfo();
    basicInfo.m_displayName.assign(szChildName);
    basicInfo.m_variableName.assign(szChildName);
    basicInfo.m_bEditable = false;

    CPropertyDescriptionBase* pRet = CComponentManager::GetInstance()->CreateProperty(ePT_Str, NULL);
    pRet->Initialize();
    pRet->SetBasicInfo(basicInfo);
    pRet->SetOwner(this->GetOwner());

    basicInfo.m_bEditable = true;// Only label can't be changed.
    CPropertyDescriptionBase* pKey = m_pKeyPropertyTemplate->Clone(false);
    pKey->Initialize();
    basicInfo.m_displayName.assign(_T("Key"));
    basicInfo.m_variableName.assign(_T("Key"));
    pKey->SetBasicInfo(basicInfo);
    pKey->SetOwner(this->GetOwner());

    CPropertyDescriptionBase* pValue = m_pValuePropertyTemplate->Clone(false);
    pValue->Initialize();
    basicInfo.m_displayName.assign(_T("Value"));
    basicInfo.m_variableName.assign(_T("Value"));
    pValue->SetBasicInfo(basicInfo);
    pValue->SetOwner(this->GetOwner());

    pRet->AddChild(pKey);
    pRet->AddChild(pValue);
    return pRet;
}

bool CMapPropertyDescription::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder)
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

void CMapPropertyDescription::DeleteAllChild()
{
    super::DeleteAllChild();
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
        BEATS_ASSERT(iVarType == ePT_Str);
        if (iVarType == ePT_Str)
        {
            CPropertyDescriptionBase* pNewProperty = AddChild(NULL);
            BEATS_ASSERT(pNewProperty != 0, _T("Create property failed when load from xml for list property description."));
            if (pNewProperty != NULL)
            {
                TiXmlElement* pChildVarElement = pVarElement->FirstChildElement("VariableNode");
                pNewProperty->GetChild(0)->LoadFromXML(pChildVarElement);
                pChildVarElement = pChildVarElement->NextSiblingElement("VariableNode");
                pNewProperty->GetChild(1)->LoadFromXML(pChildVarElement);
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
            BEATS_ASSERT(pPropertyBase->GetChildrenCount() == 2, _T("Map property must contain two property childern for each element."));
            CPropertyDescriptionBase* pKeyProperty = pPropertyBase->GetChild(0);
            CPropertyDescriptionBase* pNewKeyProperty = pKeyProperty->Clone(true);
            CPropertyDescriptionBase* pValueProperty = pPropertyBase->GetChild(1);
            CPropertyDescriptionBase* pNewValueProperty = pValueProperty->Clone(true);
            pNewChildPropertyBase->AddChild(pNewKeyProperty);
            pNewChildPropertyBase->AddChild(pNewValueProperty);
            pNewProperty->AddChild(pNewChildPropertyBase);
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
    serializer << m_pKeyPropertyTemplate->GetType();
    serializer << m_pValuePropertyTemplate->GetType();
    serializer << m_pChildren->size();
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_ASSERT((*m_pChildren)[i]->GetChildrenCount() == 2, _T("An element of map must contain two children!"));
        (*m_pChildren)[i]->GetChild(0)->Serialize(serializer);
        (*m_pChildren)[i]->GetChild(1)->Serialize(serializer);
    }
}

bool CMapPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& /*parameterUnit*/, bool /*bSerializePhase = false*/)
{
    return true;
}