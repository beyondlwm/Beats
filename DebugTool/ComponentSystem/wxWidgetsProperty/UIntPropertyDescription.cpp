#include "stdafx.h"
#include "UIntPropertyDescription.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"

CUIntPropertyDescription::CUIntPropertyDescription(CSerializer* pSerializer)
: super(ePT_UInt)
, m_minValue(0)
, m_maxValue(ULONG_MAX)
, m_spinStep(0)
{
    size_t value = 0;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> value;
    }
    InitializeValue(value);
}

CUIntPropertyDescription::CUIntPropertyDescription(const CUIntPropertyDescription& rRef)
: super(rRef)
, m_minValue(rRef.m_minValue)
, m_maxValue(rRef.m_maxValue)
, m_spinStep(rRef.m_spinStep)

{
    size_t value = 0;
    InitializeValue(value);
}

CUIntPropertyDescription::~CUIntPropertyDescription()
{
    DestoryValue<size_t>();
}

bool CUIntPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result, bool bSerializePhase/* = false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            TCHAR* pEndChar = NULL;
            size_t uValue = _tcstoul(cache[1].c_str(), &pEndChar, 10);
            BEATS_ASSERT(_tcslen(pEndChar) == 0, _T("Read uint from string %s error, stop at %s"), cache[1].c_str(), pEndChar);
            BEATS_ASSERT(errno == 0, _T("Call _tcstoul failed! string %s radix: 10"), cache[1].c_str());
            wxVariant var((wxLongLong)uValue);
            SetValue(var, true);
            if (bSerializePhase)
            {
                uValue = var.GetULongLong().GetLo();
                SetValue(&uValue, eVT_DefaultValue);
            }
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MinValue]) == 0)
        {
            m_minValue = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxValue]) == 0)
        {
            m_maxValue = _tstoi(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_SpinStep]) == 0)
        {
            m_spinStep = _tstoi(cache[1].c_str());
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for UInt property %s"), cache[0].c_str());
        }
    }
    BEATS_ASSERT(*(size_t*)m_valueArray[eVT_CurrentValue] <= m_maxValue && *(size_t*)m_valueArray[eVT_CurrentValue] >= m_minValue && m_minValue >= 0);
    return true;
}

wxPGProperty* CUIntPropertyDescription::CreateWxProperty()
{
    wxPGProperty* pProperty = new wxUIntProperty(wxPG_LABEL, wxPG_LABEL, *(size_t*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var((wxLongLong)(*(size_t*)m_valueArray[eVT_CurrentValue]));
    pProperty->SetDefaultValue(var);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    pProperty->SetAttribute( wxPG_ATTR_MIN, (wxULongLong)m_minValue );
    pProperty->SetAttribute( wxPG_ATTR_MAX, (wxULongLong)m_maxValue );
    if (m_spinStep > 0)
    {
        pProperty->SetEditor( wxPGEditor_SpinCtrl );
        pProperty->SetAttribute( wxT("Step"), (long)m_spinStep );
        pProperty->SetAttribute( wxT("MotionSpin"), true );
    }
    return pProperty;
}

void CUIntPropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    size_t uNewValue = value.GetULongLong().GetLo();
    SetValue(&uNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&uNewValue, eVT_SavedValue);
    }
}

void CUIntPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(size_t*)m_valueArray[type] = *(size_t*)pValue;
}

bool CUIntPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(size_t*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(size_t*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CUIntPropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CUIntPropertyDescription(*this);
    return pNewProperty;
}

void CUIntPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    size_t uValue = *(size_t*)m_valueArray[type];
    sprintf(pOut, "%u", uValue);
}

void CUIntPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << *(size_t*)m_valueArray[(eVT_SavedValue)];
}
