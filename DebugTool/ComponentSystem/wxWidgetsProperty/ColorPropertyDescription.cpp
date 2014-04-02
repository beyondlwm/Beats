#include "stdafx.h"
#include "ColorPropertyDescription.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"

CColorPropertyDescription::CColorPropertyDescription(CSerializer* pSerializer)
    : super(ePT_UInt)
{
    size_t value = 0;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> value;
    }
    InitializeValue(value);
}

CColorPropertyDescription::CColorPropertyDescription(const CColorPropertyDescription& rRef)
    : super(rRef)
{
    size_t value = 0;
    InitializeValue(value);
}

CColorPropertyDescription::~CColorPropertyDescription()
{
    DestoryValue<size_t>();
}

bool CColorPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result, bool bSerializePhase/* = false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            size_t uValue = _tstoi(cache[1].c_str());
            wxVariant var((wxLongLong)uValue);
            SetValue(var, true);
            if (bSerializePhase)
            {
                uValue = var.GetULongLong().GetLo();
                SetValue(&uValue, eVT_DefaultValue);
            }
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for UInt property %s"), cache[0].c_str());
        }
    }
    return true;
}

wxPGProperty* CColorPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxColourProperty(wxPG_LABEL, wxPG_LABEL, *(size_t*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var((wxLongLong)(*(size_t*)m_valueArray[eVT_CurrentValue]));
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));

    return pProperty;
}

void CColorPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    wxColor colourValue;
    colourValue << value ;
    size_t uNewValue = colourValue.GetRGB();
    uNewValue = uNewValue << 8;
    uNewValue |= colourValue.Alpha();
    SetValue(&uNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&uNewValue, eVT_SavedValue);
    }
}

void CColorPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(size_t*)m_valueArray[type] = *(size_t*)pValue;
}

bool CColorPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(size_t*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(size_t*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CColorPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CColorPropertyDescription(*this);
    return pNewProperty;
}

void CColorPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    size_t iValue = *(size_t*)m_valueArray[type];
    sprintf(pOut, "%ud", iValue);
}

void CColorPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << *(size_t*)m_valueArray[(eVT_SavedValue)];
}
