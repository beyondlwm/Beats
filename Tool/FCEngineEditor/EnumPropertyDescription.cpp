#include "stdafx.h"
#include "EnumPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentEditorProxy.h"

SEnumPropertyDataMapWrapper CEnumPropertyDescription::m_enumPropertyDataMap;

CEnumPropertyDescription::CEnumPropertyDescription(CSerializer* pSerializer)
: super(ePT_Enum)
, m_pPropertyData(NULL)
{
    int iValue = 0;
    if (pSerializer != NULL)
    {
        TString enumTypeStr;
        (*pSerializer) >> iValue;
        (*pSerializer) >> enumTypeStr;
        size_t selfDefineStrCount = 0;
        (*pSerializer) >> selfDefineStrCount;
        std::vector<TString> selfDefineStrPool;
        for (size_t i = 0; i < selfDefineStrCount; ++i)
        {
            TString selfDefineStr;
            (*pSerializer) >> selfDefineStr;
            selfDefineStrPool.push_back(selfDefineStr);
        }
        std::map<TString, SEnumPropertyData*>::iterator iter = m_enumPropertyDataMap.m_data.find(enumTypeStr);
        if (iter == m_enumPropertyDataMap.m_data.end())
        {
            const std::vector<SEnumData*>* pEnumValue = NULL;
            bool bFind = CEnumStrGenerator::GetInstance()->GetEnumValueData(enumTypeStr.c_str(), pEnumValue);
            BEATS_ASSERT(bFind, _T("Can't find the enum str for type : %s"), enumTypeStr.c_str());
            if (bFind)
            {
                BEATS_ASSERT(selfDefineStrCount <= pEnumValue->size(), _T("the self define string count :%d is too much for real count %d"), selfDefineStrCount, pEnumValue->size());
                m_pPropertyData = new SEnumPropertyData(pEnumValue);
                m_pPropertyData->m_displayString = selfDefineStrPool;
                m_enumPropertyDataMap.m_data[enumTypeStr] = m_pPropertyData;
            }
        }
        else
        {
            m_pPropertyData = iter->second;
        }
    }
    InitializeValue(iValue);
}

CEnumPropertyDescription::CEnumPropertyDescription(const CEnumPropertyDescription& rRef)
: super(rRef)
, m_pPropertyData(rRef.m_pPropertyData)
{
    InitializeValue(0);
}

CEnumPropertyDescription::~CEnumPropertyDescription()
{
    DestoryValue<int>();
}

bool CEnumPropertyDescription::AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase /*= false*/)
{
    for (size_t i = 0; i < parameterUnit.size(); ++i)
    {
        TString parameter = parameterUnit[i];
        std::vector<TString> result;
        CStringHelper::GetInstance()->SplitString(parameter.c_str(), _T(":"), result);
        if (result.size() == 2)
        {
            if ((_tcsicmp(result[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0))
            {
                int iValue = _tstoi(result[1].c_str());
                wxVariant var(iValue);
                SetValue(var, true);
                if (bSerializePhase)
                {
                    SetValue(&iValue, eVT_DefaultValue);
                }
            }
            else
            {
                BEATS_ASSERT(false, _T("Unknown parameter for Int property %s"), result[0].c_str());
            }
        }
    }
    return true;
}

wxPGProperty* CEnumPropertyDescription::CreateWxProperty()
{
    wxArrayString labels;
    wxArrayInt values;
    const std::vector<SEnumData*>& enumData = *m_pPropertyData->m_pData;
    for (size_t i = 0; i < enumData.size(); ++i)
    {
        labels.push_back(i < m_pPropertyData->m_displayString.size() ? m_pPropertyData->m_displayString[i].c_str() : enumData[i]->m_str.c_str());
        values.push_back(enumData[i]->m_value);
    }

    wxPGProperty* pProperty = new wxEnumProperty(wxPG_LABEL, wxPG_LABEL, labels, values, *(int*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetClientData(this);
    wxVariant var(*(int*)m_valueArray[eVT_DefaultValue]);
    pProperty->SetDefaultValue(var);
    wxVariant curVar(*(int*)m_valueArray[eVT_CurrentValue]);
    pProperty->SetValue(curVar);
    pProperty->SetModifiedStatus(!IsDataSame(true));
    return pProperty;
}

void CEnumPropertyDescription::SetValue(wxVariant& value, bool bSaveValue /*= true*/)
{
    int iNewValue = value.GetInteger();
    SetValue(&iNewValue, eVT_CurrentValue);
    if (bSaveValue)
    {
        SetValue(&iNewValue, eVT_SavedValue);
    }
}

void CEnumPropertyDescription::SetValue( void* pValue, EValueType type )
{
    *(int*)m_valueArray[type] = *(int*)pValue;
}

bool CEnumPropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = *(int*)m_valueArray[(bWithDefaultOrXML ? eVT_DefaultValue : eVT_SavedValue)] == *(int*)m_valueArray[eVT_CurrentValue];
    return bRet;
}

CPropertyDescriptionBase* CEnumPropertyDescription::CreateNewInstance()
{
    CEnumPropertyDescription* pNewProperty = new CEnumPropertyDescription(*this);
    return pNewProperty;
}

void CEnumPropertyDescription::GetValueAsChar( EValueType type, char* pOut )
{
    int iValue = *(int*)m_valueArray[type];
    sprintf(pOut, "%d", iValue);
}

void CEnumPropertyDescription::Serialize( CSerializer& serializer )
{
    serializer << *(int*)m_valueArray[(eVT_SavedValue)];
}
