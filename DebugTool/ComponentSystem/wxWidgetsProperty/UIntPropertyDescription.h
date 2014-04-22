#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_UINTPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_UINTPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

class CUIntPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CUIntPropertyDescription(CSerializer* pSerializer);
    CUIntPropertyDescription(const CUIntPropertyDescription& rRef);
    virtual ~CUIntPropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer);

private:
    size_t m_minValue;
    size_t m_maxValue;
    size_t m_spinStep;
};

#endif