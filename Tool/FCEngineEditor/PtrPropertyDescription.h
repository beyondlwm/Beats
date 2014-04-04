#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_PTRPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_PTRPROPERTYDESCRIPTION_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
class CComponentEditorProxy;

class CPtrPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CPtrPropertyDescription(CSerializer* serializer);
    CPtrPropertyDescription(const CPtrPropertyDescription& rRef);
    virtual ~CPtrPropertyDescription();

    size_t GetPtrGuid();
    void SetDerivedGuid(size_t uDerivedGuid);
    size_t GetDerivedGuid() const;
    CComponentEditorProxy* GetInstanceComponent();
    bool CreateInstance();
    bool DestroyInstance();

    virtual void SetValue(wxVariant& value, bool bSaveValue);

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);
    virtual wxPGProperty* CreateWxProperty();
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual bool IsDataSame(bool bWithDefaultOrXML);

    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();

    virtual void SetValue(void* pDefaultValue, EValueType type);
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void Serialize(CSerializer& serializer);
    virtual void Initialize();

private:
    void UpdateDisplayString(size_t uComponentId);

private:
    size_t m_uDerivedGuid;
    size_t m_uComponentGuid;
    CComponentEditorProxy* m_pInstance;
    // This flag indicate if this property should have instance according to XML data. 
    bool m_bHasInstance;
};

#endif