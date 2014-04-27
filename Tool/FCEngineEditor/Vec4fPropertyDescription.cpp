#include "stdafx.h"
#include "Vec4fPropertyDescription.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/Serializer.h"
#define VEC_COUNT 4

CVec4PropertyDescription::CVec4PropertyDescription(CSerializer* pSerializer)
    : super(NULL)
{
    SetType(ePT_Vec4F);
    SetMaxCount(VEC_COUNT);
    SetFixed(true);
    SetChildType(ePT_Float);
    TString InitValue = _T("0,0,0,0");
    if (pSerializer != NULL)
    {
        InitValue.clear();
        for (size_t i = 0; i < VEC_COUNT; ++i)
        {
            float fValue = 0;
            (*pSerializer) >> fValue;
            InitValue.append(wxString::Format(_T("%f"),fValue));
            if (i != VEC_COUNT - 1)
            {
                InitValue.append(_T(","));
            }
            CPropertyDescriptionBase* pChild = AddChild(NULL);
            pChild->InitializeValue(fValue);
        }
        ResetChildName();
    }
    InitializeValue(InitValue);
}

CVec4PropertyDescription::CVec4PropertyDescription(const CVec4PropertyDescription& rRef)
    : super(rRef)
{
    InitializeValue(*(TString*)rRef.GetValue(eVT_DefaultValue));
}

CVec4PropertyDescription::~CVec4PropertyDescription()
{
}

bool CVec4PropertyDescription::AnalyseUIParameterImpl( const std::vector<TString>& result, bool /*bSerializePhase*//* = false */)
{
    std::vector<TString> cache;
    for (size_t i = 0; i < result.size(); ++i)
    {
        cache.clear();
        CStringHelper::GetInstance()->SplitString(result[i].c_str(), _T(":"), cache);
        BEATS_ASSERT(cache.size() == 2);
        if (_tcsicmp(cache[0].c_str(), UIParameterAttrStr[eUIPAT_DefaultValue]) == 0)
        {
            std::vector<TString> values;
            CStringHelper::GetInstance()->SplitString(cache[1].c_str(), _T("@"), values);
            BEATS_ASSERT(values.size() == VEC_COUNT);
            for (size_t j = 0; j < VEC_COUNT; ++j)
            {
                m_pChildren->at(j)->InitializeValue((float)_tstof(values[j].c_str()));
            }
            ResetName();
        }
        else
        {
            BEATS_ASSERT(false, _T("Unknown parameter for float property %s"), cache[0].c_str());
        }
    }
    return true;
}

bool CVec4PropertyDescription::IsDataSame( bool bWithDefaultOrXML )
{
    bool bRet = true;
    if (m_pChildren->size() == VEC_COUNT)
    {
        for (size_t i = 0; i < VEC_COUNT; ++i)
        {
            bRet = bRet && m_pChildren->at(i)->IsDataSame(bWithDefaultOrXML);
        }
        ResetName();
    }
    return bRet;
}

CPropertyDescriptionBase* CVec4PropertyDescription::CreateNewInstance()
{
    CPropertyDescriptionBase* pNewProperty = new CVec4PropertyDescription(*this);
    return pNewProperty;
}

void CVec4PropertyDescription::ResetChildName()
{
    if (m_pChildren->size() == VEC_COUNT)
    {
        for (size_t i = 0; i < VEC_COUNT; ++i)
        {
            if (i == 0)
            {
                (*m_pChildren)[0]->GetBasicInfo().m_displayName.assign(_T("x"));
                (*m_pChildren)[0]->GetBasicInfo().m_variableName.assign(_T("x"));
            }
            else if (i == 1)
            {
                (*m_pChildren)[1]->GetBasicInfo().m_displayName.assign(_T("y"));
                (*m_pChildren)[1]->GetBasicInfo().m_variableName.assign(_T("y"));
            }
            else if (i == 2)
            {
                (*m_pChildren)[2]->GetBasicInfo().m_displayName.assign(_T("z"));
                (*m_pChildren)[2]->GetBasicInfo().m_variableName.assign(_T("z"));
            }
            else if (i == 3)
            {
                (*m_pChildren)[3]->GetBasicInfo().m_displayName.assign(_T("w"));
                (*m_pChildren)[3]->GetBasicInfo().m_variableName.assign(_T("w"));
            }
            else
            {
                BEATS_ASSERT(false, _T("Unkonwn Count!"));
            }
        }
    }
}

void CVec4PropertyDescription::GetCurrentName(wxString& strName)
{
    if (m_pChildren->size() == VEC_COUNT)
    {
        char szBuffer[16];
        for (size_t i = 0; i < VEC_COUNT; ++i)
        {
            m_pChildren->at(i)->GetValueAsChar(eVT_CurrentValue, szBuffer);
            strName.Append(szBuffer);
            if (i != VEC_COUNT - 1)
            {
                strName.Append(_T(", "));
            }
        }
    }
}

CPropertyDescriptionBase* CVec4PropertyDescription::Clone(bool /*bCloneValue*/)
{
    CVec4PropertyDescription* pNewProperty = static_cast<CVec4PropertyDescription*>(super::Clone(true));
    return pNewProperty;
}

bool CVec4PropertyDescription::IsContainerProperty()
{
    return false;
}