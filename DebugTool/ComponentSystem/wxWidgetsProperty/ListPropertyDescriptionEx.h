#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_LISTPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
#include "../../../Utility/TinyXML/tinyxml.h"

class CListPropertyDescriptionEx : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CListPropertyDescriptionEx(CSerializer* pSerializer);
    CListPropertyDescriptionEx(const CListPropertyDescriptionEx& rRef);
    virtual ~CListPropertyDescriptionEx();

    virtual bool IsContainerProperty() override;
    bool IsFixed() const;
    void SetFixed(bool bFixedFlag);
    size_t GetMaxCount() const;
    void SetMaxCount(size_t uMaxCount);
    void SetTemplateProperty(CPropertyDescriptionBase* pTemplateProperty);
    CPropertyDescriptionBase* GetTemplateProperty() const;

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

private:
    void ResetChildName();
    void ResetName();
    void GetCurrentName(TCHAR* pName);
    CPropertyDescriptionBase* CreateInstance();

private:
    bool m_bFixCount;
    size_t m_uMaxCount;
    CPropertyDescriptionBase* m_pChildTemplate;
};

#endif