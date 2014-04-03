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

    virtual CPropertyDescriptionBase* AddMapChild();
    void AddMapChild(CPropertyDescriptionBase* pChild);
    virtual void DeleteMapChild(CPropertyDescriptionBase* pProperty);
    virtual void DeleteAllMapChild();

    virtual wxPGProperty* CreateWxProperty();
    virtual void SetValue(wxVariant& value, bool bSaveValue = true);
    virtual void SetValue(void* pValue, EValueType type);
    virtual bool IsDataSame(bool bWithDefaultOrXML);
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance();
    virtual void GetValueAsChar(EValueType type, char* pOut);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void Serialize(CSerializer& serializer);
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false);

private:
    void ResetChildName();
    void ResetName();
    void GetCurrentName(TCHAR* pName);
    CPropertyDescriptionBase* CreateInstance();

private:
    EPropertyType m_keyType;
    EPropertyType m_valueType;
    size_t m_uValuePtrGuid;
};


#endif