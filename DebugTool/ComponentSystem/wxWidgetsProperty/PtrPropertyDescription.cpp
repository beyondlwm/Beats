#include "stdafx.h"
#include "PtrPropertyDescription.h"
#include "../../../Utility/Serializer/Serializer.h"
#include "../../../Utility/StringHelper/StringHelper.h"
#include "../../BDTWxApp.h"
#include "../../BDTWxFrame.h"
#include "../../Components/Component/ComponentEditorProxy.h"
#include "../../Components/Component/ComponentProxyManager.h"

#define POINTER_SPLIT_SYMBOL _T("@")

CPtrPropertyDescription::CPtrPropertyDescription(CSerializer* pSerializer)
    : super(eRPT_Ptr)
    , m_uComponentGuid(0)
    , m_uDerivedGuid(0)
    , m_pInstance(NULL)
    , m_bHasInstance(false)
{
    if (pSerializer != NULL)
    { 
        (*pSerializer) >> m_uComponentGuid;
        TString emptyStr = _T("");
        InitializeValue(emptyStr);
    }
}

CPtrPropertyDescription::CPtrPropertyDescription(const CPtrPropertyDescription& rRef)
    : super(rRef)
    , m_uComponentGuid(rRef.m_uComponentGuid)
    , m_uDerivedGuid(rRef.m_uDerivedGuid)
    , m_pInstance(NULL)
    , m_bHasInstance(false)
{
    TString emptyStr = _T("");
    InitializeValue(emptyStr);
}

CPtrPropertyDescription::~CPtrPropertyDescription()
{
    if (GetInstanceComponent() != NULL)
    {
        DestroyInstance();
    }
    DestoryValue<TString>();
}

bool CPtrPropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& paramUnit)
{
    for (size_t i = 0; i < paramUnit.size(); ++i)
    {
        TString parameter = paramUnit[i];
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(parameter.c_str(), PROPERTY_KEYWORD_SPLIT_STR, result);
        BEATS_ASSERT(result.size() == 2);
        if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            parameter = result[1].c_str();
        }
        m_bHasInstance = parameter[0] == _T('+');
    }
    return true;
}

wxPGProperty* CPtrPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxStringProperty(wxPG_LABEL, wxPG_LABEL, ((TString*)m_valueArray[eVT_CurrentValue])->c_str());
    wxVariant defaultValue(((TString*)m_valueArray[eVT_DefaultValue])->c_str());
    pProperty->SetDefaultValue(defaultValue);
    pProperty->SetClientData(this);
    pProperty->SetModifiedStatus(GetInstanceComponent() != NULL);
    pProperty->SetEditor(CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetPtrEditor());
    return pProperty;
}


void CPtrPropertyDescription::SetValue( wxVariant& value , bool bSaveValue)
{
    TString bNewValue = value.GetString();
    SetValueWithType(&bNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValueWithType(&bNewValue, eVT_SavedValue);
    }
}

bool CPtrPropertyDescription::CopyValue(void* pSourceValue, void* pTargetValue)
{
    TString* pStrValue = (TString*)pSourceValue;
    bool bRet = *(TString*)pTargetValue != *pStrValue;
    if (bRet)
    {
        *(TString*)pTargetValue = *pStrValue;
    }
    return bRet;
}

size_t CPtrPropertyDescription::GetPtrGuid()
{
    return m_uComponentGuid;
}

void CPtrPropertyDescription::SetDerivedGuid(size_t uDerivedGuid)
{
    m_uDerivedGuid = uDerivedGuid;
}

size_t CPtrPropertyDescription::GetDerivedGuid() const
{
    return m_uDerivedGuid;
}

CComponentEditorProxy* CPtrPropertyDescription::GetInstanceComponent()
{
    return m_pInstance;
}

bool CPtrPropertyDescription::CreateInstance()
{
    bool bRet = false;
    if ( m_pInstance == NULL)
    {
        size_t uInstanceGuid = m_uDerivedGuid == 0 ? m_uComponentGuid : m_uDerivedGuid;
        m_pInstance = static_cast<CComponentEditorProxy*>(CComponentProxyManager::GetInstance()->CreateComponent(uInstanceGuid, false, true));
        const std::vector<CPropertyDescriptionBase*>* propertyPool = m_pInstance->GetPropertyPool();
        for (size_t i = 0; i < propertyPool->size(); ++i)
        {
            AddChild((*propertyPool)[i]);
        }
        UpdateDisplayString(uInstanceGuid);
        bRet = true;
    }

    return bRet;
}

bool CPtrPropertyDescription::DestroyInstance()
{
    bool bRet = false;

    if (m_pInstance != NULL)
    {
        BEATS_SAFE_DELETE(m_pInstance);
        m_pChildren->clear();
        bRet = true;
    }
    SetDerivedGuid(0);
    UpdateDisplayString(m_uComponentGuid);

    return bRet;
}

bool CPtrPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = false;
    if (bWithDefaultOrXML)
    {
        bRet = m_pInstance == NULL;
    }
    else
    {
        bRet = m_bHasInstance ? m_pInstance != NULL : m_pInstance == NULL;
    }
    return bRet;
}

void CPtrPropertyDescription::LoadFromXML( TiXmlElement* pNode )
{
    super::LoadFromXML(pNode);
    TString* pStrValue = (TString*)m_valueArray[eVT_CurrentValue];
    m_bHasInstance = pStrValue->length() > 1 && pStrValue->at(0) ==_T('+');
    if (m_bHasInstance)
    {
        int nPos = CStringHelper::GetInstance()->FindFirstString(pStrValue->c_str(), POINTER_SPLIT_SYMBOL, false);
        const TCHAR* pszValueString = &(pStrValue->c_str()[nPos + _tcslen(POINTER_SPLIT_SYMBOL)]);
        TCHAR* pEndChar = NULL;
        int nRadix = 16;
        size_t uDerivedValue = _tcstoul(pszValueString, &pEndChar, nRadix);
        BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), pszValueString, pEndChar);
        BEATS_ASSERT(errno == 0, _T("Call _tcstoul failed! string %s radix: %d"), pszValueString, nRadix);
        if (uDerivedValue != m_uComponentGuid)
        {
            m_uDerivedGuid = uDerivedValue;
        }
        if (this->CreateInstance())
        {
            this->GetInstanceComponent()->LoadFromXML(pNode);
        }
    }
}

void CPtrPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    const TCHAR* pValue = ((TString*)m_valueArray[type])->c_str();
    CStringHelper::GetInstance()->ConvertToCHAR(pValue, pOut, 10240);
}

bool CPtrPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    ((TString*)pOutValue)->assign(pIn);
    return true;
}

CPropertyDescriptionBase* CPtrPropertyDescription::Clone(bool bCloneValue)
{
    CPtrPropertyDescription* pNewProperty = static_cast<CPtrPropertyDescription*>(super::Clone(bCloneValue));
    if (m_pInstance != NULL && bCloneValue)
    {
        pNewProperty->m_pInstance = static_cast<CComponentEditorProxy*>(m_pInstance->Clone(true, NULL));
        for (size_t i = 0; i < m_pInstance->GetPropertyPool()->size(); ++i)
        {
            CPropertyDescriptionBase* pProperty = (*m_pInstance->GetPropertyPool())[i];
            pNewProperty->AddChild(pProperty->Clone(true));
        }
    }
    return pNewProperty;
}

CPropertyDescriptionBase* CPtrPropertyDescription::CreateNewInstance()
{
    CPtrPropertyDescription* pNewProperty = new CPtrPropertyDescription(*this);
    return pNewProperty;
}

void CPtrPropertyDescription::Serialize( CSerializer& serializer , EValueType eValueType/* = eVT_SavedValue*/)
{
    serializer << (m_pInstance != NULL);
    if (m_pInstance != NULL)
    {
        m_pInstance->Serialize(serializer, eValueType);
    }
}

void CPtrPropertyDescription::Initialize()
{
    super::Initialize();
    // If we are creating a brand new ptrproperty, we need to initialize the string value.
    // If we are cloning from another ptrproperty, the value is already assigned.
    TString* pDefaultValue = (TString*)m_valueArray[eVT_DefaultValue];
    if (pDefaultValue->length() == 0)
    {
        UpdateDisplayString(m_uComponentGuid);
    }
}

void CPtrPropertyDescription::UpdateDisplayString(size_t uComponentGuid)
{
    TString strComponentName = CComponentProxyManager::GetInstance()->QueryComponentName(uComponentGuid);
    BEATS_ASSERT(strComponentName.length() > 0, _T("Can't Find the component name of GUID: 0x%x"), uComponentGuid);
    wxString value = wxString::Format(_T("%s%s%s0x%x"), m_bHasInstance ? _T("+") : _T(""), strComponentName.c_str(), POINTER_SPLIT_SYMBOL, uComponentGuid);
    TString valueStr(value);
    for (size_t i = eVT_DefaultValue; i < eVT_Count; ++i)
    {
        SetValueWithType(&valueStr, (EValueType)i);
    }
}