#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_COLORPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_COLORPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CColorPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CColorPropertyDescription(CSerializer* pSerializer);
    CColorPropertyDescription(const CColorPropertyDescription& rRef);

    virtual ~CColorPropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void Serialize(CSerializer& serializer);
};

#endif