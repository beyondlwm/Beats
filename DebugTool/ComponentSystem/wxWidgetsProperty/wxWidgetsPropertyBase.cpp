#include "stdafx.h"
#include "wxWidgetsPropertyBase.h"
#include "PtrPropertyDescription.h"
#include "../../../Components/Component/ComponentEditorProxy.h"
#include "../../../Utility/Serializer/Serializer.h"
#include "../../../Utility/StringHelper/StringHelper.h"
#include "../../../Utility/TinyXML/tinyxml.h"

CWxwidgetsPropertyBase::CWxwidgetsPropertyBase(EReflectPropertyType type)
: super(type)
{
}

CWxwidgetsPropertyBase::CWxwidgetsPropertyBase(const CWxwidgetsPropertyBase& rRef)
: super(rRef)
, m_pVisibleWhenTrigger(rRef.m_pVisibleWhenTrigger)
{

}

CWxwidgetsPropertyBase::~CWxwidgetsPropertyBase()
{
}

void CWxwidgetsPropertyBase::Initialize()
{
    super::Initialize();
    if (m_pVisibleWhenTrigger.Get() != NULL)
    {
        m_pVisibleWhenTrigger->Initialize();
        const std::vector<STriggerContent*>& triggerContent = m_pVisibleWhenTrigger->GetContent();
        for (size_t i = 0; i < triggerContent.size(); ++i)
        {
            // 0 means or 1 means and.
            if(triggerContent[i] != NULL && (int)triggerContent[i] != 1)
            {
                const std::vector<CPropertyDescriptionBase*>* pPropertyPool = m_pOwner->GetPropertyPool();
                for (size_t j = 0; j < pPropertyPool->size(); ++j)
                {
                    const TString& name = (*pPropertyPool)[j]->GetBasicInfo()->m_variableName;
                    if (name.compare(triggerContent[i]->GetPropertyName()) == 0)
                    {
                        CWxwidgetsPropertyBase* pProperty = static_cast<CWxwidgetsPropertyBase*>((*pPropertyPool)[j]);
                        pProperty->AddEffectProperties(this);
                        break;
                    }
                }
            }
        }
    }
}

void CWxwidgetsPropertyBase::AddEffectProperties(CWxwidgetsPropertyBase* pProperty)
{
    std::set<CWxwidgetsPropertyBase*>::iterator iter = m_effctProperties.find(pProperty);
    if (iter == m_effctProperties.end())
    {
        m_effctProperties.insert(pProperty);
    }
}

std::set<CWxwidgetsPropertyBase*>& CWxwidgetsPropertyBase::GetEffectProperties()
{
    return m_effctProperties;
}

bool CWxwidgetsPropertyBase::CheckVisibleTrigger()
{
    return m_pVisibleWhenTrigger.Get() == NULL || m_pVisibleWhenTrigger->IsOk(GetOwner());
}

void CWxwidgetsPropertyBase::SaveToXML( TiXmlElement* pParentNode )
{
    TiXmlElement* pVariableElement = new TiXmlElement("VariableNode");
    pVariableElement->SetAttribute("Type", m_type);
    char variableName[MAX_PATH] = {0};
    CStringHelper::GetInstance()->ConvertToCHAR(GetBasicInfo()->m_variableName.c_str(), variableName, MAX_PATH);
    pVariableElement->SetAttribute("Variable", variableName);
    char szValue[102400];
    GetValueAsChar(eVT_CurrentValue, szValue);
    pVariableElement->SetAttribute("SavedValue", szValue);
    pParentNode->LinkEndChild(pVariableElement);
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        (*m_pChildren)[i]->SaveToXML(pVariableElement);
    }
}

void CWxwidgetsPropertyBase::LoadFromXML( TiXmlElement* pNode )
{
    BEATS_ASSERT(pNode != NULL);
    const char* pValue = pNode->Attribute("SavedValue");
    TCHAR pTCHARValue[10240];
    CStringHelper::GetInstance()->ConvertToTCHAR(pValue, pTCHARValue, 10240);
    GetValueByTChar(pTCHARValue, m_valueArray[eVT_CurrentValue]);
    GetValueByTChar(pTCHARValue, m_valueArray[eVT_SavedValue]);
}

void CWxwidgetsPropertyBase::AnalyseUIParameter( const TCHAR* parameter )
{
    if (parameter != NULL && parameter[0] != 0)
    {
        CStringHelper* pStringHelper = CStringHelper::GetInstance();
        std::vector<TString> paramUnit;
        // Don't Ignore the space, because the string value may contains space.
        pStringHelper->SplitString(parameter, PROPERTY_PARAM_SPLIT_STR, paramUnit, false);
        std::vector<TString> cache;
        for (std::vector<TString>::iterator iter = paramUnit.begin(); iter != paramUnit.end();)
        {
            cache.resize(0);
            pStringHelper->SplitString(iter->c_str(), PROPERTY_KEYWORD_SPLIT_STR, cache, true);
            bool bHandled = false;
            if(cache.size() == 2)
            {
                if (cache[0].compare(UIParameterAttrStr[eUIPAT_VisibleWhen]) == 0)
                {
                    m_pVisibleWhenTrigger = new CPropertyTrigger(cache[1]);
                    bHandled = true;
                }
            }
            if (bHandled)
            {
                iter = paramUnit.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
        AnalyseUIParameterImpl(paramUnit);
    }
}
