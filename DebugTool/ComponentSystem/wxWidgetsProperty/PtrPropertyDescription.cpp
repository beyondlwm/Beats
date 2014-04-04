#include "stdafx.h"
#include "PtrPropertyDescription.h"
#include "../../../Utility/Serializer/Serializer.h"
#include "../../../Utility/StringHelper/StringHelper.h"
#include "../../BDTWxApp.h"
#include "../../BDTWxFrame.h"
#include "../../Components/Component/ComponentEditorProxy.h"
#include "../../Components/Component/ComponentManager.h"

CPtrPropertyDescription::CPtrPropertyDescription(CSerializer* pSerializer)
    : super(ePT_Ptr)
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

bool CPtrPropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& paramUnit, bool /*bSerializePhase = false */)
{
    for (size_t i = 0; i < paramUnit.size(); ++i)
    {
        TString parameter = paramUnit[i];
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(parameter.c_str(), _T(":"), result);
        BEATS_ASSERT(result.size() == 2);
        if (_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            parameter = result[1].c_str();
        }
        m_bHasInstance = parameter[0] == _T('+');
        wxVariant var(parameter);
        SetValue(var, true);
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
    SetValue(&bNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&bNewValue, eVT_SavedValue);
    }
}

void CPtrPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(TString*)m_valueArray[type] = *(TString*)pValue;
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
        m_pInstance = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->CreateComponent(uInstanceGuid, false, true));
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
    if (m_bHasInstance)
    {
        CPtrPropertyDescription* pPtrProperty = static_cast<CPtrPropertyDescription*>(this);
        if (pPtrProperty->CreateInstance())
        {
            pPtrProperty->GetInstanceComponent()->LoadFromXML(pNode);
        }
    }
}

void CPtrPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    const TCHAR* pValue = ((TString*)m_valueArray[type])->c_str();
    CStringHelper::GetInstance()->ConvertToCHAR(pValue, pOut, 10240);
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

void CPtrPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << (m_pInstance != NULL);
    if (m_pInstance != NULL)
    {
        m_pInstance->Serialize(serializer);
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

void CPtrPropertyDescription::UpdateDisplayString(size_t uComponentId)
{
    CComponentEditorProxy* pComponent = static_cast<CComponentEditorProxy*>(CComponentManager::GetInstance()->GetComponentTemplate(uComponentId));
    wxString value = wxString::Format(_T("%s%s@0x%x"), _T(""), pComponent == NULL ? _T("Unknown") : pComponent->GetClassStr(), uComponentId);
    TString valueStr(value);
    for (size_t i = eVT_DefaultValue; i < eVT_Count; ++i)
    {
        SetValue((void*)&valueStr, (EValueType)i);
    }

}