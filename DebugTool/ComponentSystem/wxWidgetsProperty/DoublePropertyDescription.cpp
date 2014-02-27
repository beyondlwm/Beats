#include "stdafx.h"
#include "DoublePropertyDescription.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/Serializer/Serializer.h"
#include <float.h>

CDoublePropertyDescription::CDoublePropertyDescription(CSerializer* pSerializer)
: super(ePT_Double)
, m_dMinValue(-DBL_MIN)
, m_dMaxValue(DBL_MAX)
, m_dSpinStep(0)
{
    double bDefaultValue = 0.0f;
    if (pSerializer != NULL)
    {
        (*pSerializer) >> bDefaultValue;
    }
    InitializeValue(bDefaultValue);
}

CDoublePropertyDescription::CDoublePropertyDescription(const CDoublePropertyDescription& rRef)
: super(rRef)
, m_dMinValue(rRef.m_dMinValue)
, m_dMaxValue(rRef.m_dMaxValue)
, m_dSpinStep(rRef.m_dSpinStep)
{
    InitializeValue(0.0);
}

CDoublePropertyDescription::~CDoublePropertyDescription()
{
    DestoryValue<double>();
}

bool CDoublePropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& result, bool bSerializePhase/* = false */)
{
    std::vector<TString> cache;
    BEATS_ASSERT(result.size() <= 4);
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            double doubleValue = (double)_tstof(cache[1].c_str());
            wxVariant var(doubleValue);
            SetValue(var, true);
            if (bSerializePhase)
            {
                SetValue(&doubleValue, eVT_DefaultValue);
            }
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MinValue]) == 0)
        {
            m_dMinValue = (double)_tstof(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_MaxValue]) == 0)
        {
            m_dMaxValue = (double)_tstof(cache[1].c_str());
        }
        else if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_SpinStep]) == 0)
        {
            m_dSpinStep = (double)_tstof(cache[1].c_str());
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for double property %s"), cache[0].c_str());
        }
    }
    BEATS_ASSERT(*(double*)m_valueArray[eVT_CurrentValue] < m_dMaxValue && *(double*)m_valueArray[eVT_CurrentValue] > m_dMinValue);
    return true;
}

wxPGProperty* CDoublePropertyDescription::CreateWxProperty()
{
    wxFloatProperty* pProperty = new wxFloatProperty(wxPG_LABEL, wxPG_LABEL, *(double*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var(*(double*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(var);        
    pProperty->SetModifiedStatus(!IsDataSame(true));

    pProperty->SetAttribute(wxPG_ATTR_MIN, m_dMinValue);
    pProperty->SetAttribute(wxPG_ATTR_MAX, m_dMaxValue );
    if (m_dSpinStep > 0)
    {
        pProperty->SetEditor(wxPGEditor_SpinCtrl);
        pProperty->SetAttribute( wxT("Step"), m_dSpinStep );
        pProperty->SetAttribute( wxT("MotionSpin"), true );
    }

    return pProperty;
}

void CDoublePropertyDescription::SetValue( wxVariant& value, bool bSaveValue /*= true*/ )
{
    double bNewValue = value.GetDouble();
    SetValue(&bNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&bNewValue, eVT_SavedValue);
    }
}

void CDoublePropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(double*)m_valueArray[type] = *(double*)pValue;
}

bool CDoublePropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = BEATS_FLOAT_EQUAL((*(double*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)]), (*(double*)m_valueArray[eVT_CurrentValue]));
    return bRet;
}

CPropertyDescriptionBase* CDoublePropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CDoublePropertyDescription(*this);
    return pNewProperty;
}

void CDoublePropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    double* pValue = (double*)m_valueArray[type];
    sprintf(pOut, "%.17f", *pValue);
}

void CDoublePropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << *(double*)m_valueArray[(eVT_SavedValue)];
}
