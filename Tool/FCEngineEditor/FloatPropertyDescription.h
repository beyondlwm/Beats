#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_FLOATPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_FLOATPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CFloatPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CFloatPropertyDescription(CSerializer* pSerializer);
    CFloatPropertyDescription(const CFloatPropertyDescription& rRef);

    virtual ~CFloatPropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void Serialize(CSerializer& serializer);

private:
    float m_fMinValue;
    float m_fMaxValue;
    float m_fSpinStep;
};

#endif