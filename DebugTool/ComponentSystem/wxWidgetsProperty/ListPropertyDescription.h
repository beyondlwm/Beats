#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
#include "../../../Utility/TinyXML/tinyxml.h"

class CListPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CListPropertyDescription(CSerializer* pSerializer);
    CListPropertyDescription(const CListPropertyDescription& rRef);
    virtual ~CListPropertyDescription();

    virtual bool IsContainerProperty() override;
    bool IsFixed() const;
    void SetFixed(bool bFixedFlag);
    uint32_t GetMaxCount() const;
    void SetMaxCount(uint32_t uMaxCount);
    void SetTemplateProperty(CPropertyDescriptionBase* pTemplateProperty);
    CPropertyDescriptionBase* GetTemplateProperty() const;

    virtual CPropertyDescriptionBase* AddChild(CPropertyDescriptionBase* pProperty) override;
    virtual bool DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder = false) override;
    virtual void DeleteAllChild() override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void LoadFromXML(TiXmlElement* pNode) override;
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual void Deserialize(CSerializer& serializer, EValueType eValueType = eVT_CurrentValue) override;

private:
    void ResetChildName();
    void ResetName();
    void GetCurrentName(wxString& pName);
    CPropertyDescriptionBase* CreateInstance();

private:
    bool m_bFixCount;
    uint32_t m_uMaxCount;
    CPropertyDescriptionBase* m_pChildTemplate;
};

#endif