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
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;

private:
    uint32_t m_minValue;
    uint32_t m_maxValue;
    uint32_t m_spinStep;
};

#endif