#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_INTPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_INTPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CIntPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CIntPropertyDescription(CSerializer* pSerializer);
    CIntPropertyDescription(const CIntPropertyDescription& rRef);

    virtual ~CIntPropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void Serialize(CSerializer& serializer);

private:
    int m_minValue;
    int m_maxValue;
    size_t m_spinStep;
};

#endif