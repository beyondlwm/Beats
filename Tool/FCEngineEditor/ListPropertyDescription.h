#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"

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
    size_t GetMaxCount() const;
    void SetMaxCount(size_t uMaxCount);
    EPropertyType GetChildType() const;
    void SetChildType(EPropertyType type);
    virtual CPropertyDescriptionBase* AddChild(CPropertyDescriptionBase* pProperty) override;
    virtual bool DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder = false) override;
    virtual void DeleteAllChild() override;

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);
    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void Serialize(CSerializer& serializer);

protected:
    virtual void ResetChildName();
    virtual void GetCurrentName(wxString& pName);
    void ResetName();
    CPropertyDescriptionBase* CreateInstance();

private:
    bool m_bFixCount;
    size_t m_uMaxCount;
    EPropertyType m_childType;
    size_t m_childPtrGuid;
};

#endif