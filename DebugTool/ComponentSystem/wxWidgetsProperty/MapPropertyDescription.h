#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_MAPPROPERTYDESCRIPTIONEX_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_MAPPROPERTYDESCRIPTIONEX_H__INCLUDE

#include "wxwidgetsPropertyBase.h"
#include "../../../Utility/TinyXML/tinyxml.h"

class CMapPropertyDescription : public CWxwidgetsPropertyBase
{
    typedef CWxwidgetsPropertyBase super;
public:
    CMapPropertyDescription(CSerializer* pSerializer);
    CMapPropertyDescription(const CMapPropertyDescription& rRef);
    virtual ~CMapPropertyDescription();

    virtual bool IsContainerProperty() override;
    virtual CPropertyDescriptionBase* AddChild(CPropertyDescriptionBase* pChild) override;
    virtual bool DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepOrder = false) override;
    virtual void DeleteAllChild() override;

    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual bool CopyValue(void* pSourceValue, void* pTargetValue) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut) const override;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) override;
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void Serialize(CSerializer& serializer, EValueType eValueType = eVT_SavedValue) override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit);

private:
    void ResetChildName();
    void ResetName();
    void GetCurrentName(TCHAR* pName);
    CPropertyDescriptionBase* CreateInstance();

private:
    CPropertyDescriptionBase* m_pKeyPropertyTemplate;
    CPropertyDescriptionBase* m_pValuePropertyTemplate;
    size_t m_uValuePtrGuid;
};


#endif