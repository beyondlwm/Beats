#include "stdafx.h"
#include "BoolPropertyDescription.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"

CBoolPropertyDescription::CBoolPropertyDescription(CSerializer* pSerializer)
: super(eRPT_Bool)
{
    bool bDefaultValue = false;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> bDefaultValue;
    }
    InitializeValue(bDefaultValue);
}

CBoolPropertyDescription::CBoolPropertyDescription(const CBoolPropertyDescription& rRef)
: super(rRef)
{
    InitializeValue(false);
}

CBoolPropertyDescription::~CBoolPropertyDescription()
{
    DestoryValue<bool>();
}

bool CBoolPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result)
{
    BEATS_ASSERT(result.size() <= 1);
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            bool bValue = false;
            GetValueByTChar(cache[1].c_str(), &bValue);
            wxVariant var(bValue);
            SetValue(var, true);
            SetValue(&bValue, eVT_DefaultValue);
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for bool property %s"), cache[0].c_str());
        }
    }
    return false;
}

wxPGProperty* CBoolPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxBoolProperty(wxPG_LABEL, wxPG_LABEL, *(bool*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var(*(bool*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CBoolPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/)
{
    bool bNewValue = value.GetBool();
    SetValue(&bNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&bNewValue, eVT_SavedValue);
    }
}

void CBoolPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(bool*)m_valueArray[type] = *(bool*)pValue;
}

bool CBoolPropertyDescription::IsDataSame(bool bWithDefaultOrXML)
{
    bool bRet = *(bool*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(bool*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CBoolPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CBoolPropertyDescription(*this);
    return pNewProperty;
}

void CBoolPropertyDescription::GetValueAsChar(EValueType type, char* pOut)
{
    bool bValue = *(bool*)m_valueArray[type];
    sprintf(pOut, "%s", bValue ? "True" : "False");
}

bool CBoolPropertyDescription::GetValueByTChar(const TCHAR* pIn, void* pOutValue)
{
    BEATS_ASSERT(pOutValue != NULL);
    bool bRet = false;
    if (_tcsicmp(pIn, _T("true")) == 0)
    {
        *(bool*)pOutValue = true;
        bRet = true;
    }
    else
    {
        *(bool*)pOutValue = false;
        bRet = _tcsicmp(pIn, _T("false")) == 0;
        BEATS_ASSERT(bRet, _T("Unknown value for bool property %s"), pIn);
    }
    return bRet;
}

void CBoolPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << *(bool*)m_valueArray[(eVT_SavedValue)];
}
