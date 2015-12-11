#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_WXWIDGETSPROPERTYBASE_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_WXWIDGETSPROPERTYBASE_H__INCLUDE

#include "../../../Components/ComponentPublic.h"
#include "../../../Components/Property/PropertyDescriptionBase.h"
#include "../../Utility/SharePtr/SharePtr.h"
#include "PropertyTrigger.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

class CWxwidgetsPropertyBase : public CPropertyDescriptionBase
{
    typedef CPropertyDescriptionBase super;
public:
    CWxwidgetsPropertyBase(EReflectPropertyType type);
    CWxwidgetsPropertyBase(const CWxwidgetsPropertyBase& rRef);

    virtual ~CWxwidgetsPropertyBase();

    virtual void Initialize();

    void AddEffectProperties(CWxwidgetsPropertyBase* pProperty);
    std::set<CWxwidgetsPropertyBase*>& GetEffectProperties();
    bool CheckVisibleTrigger();
    wxEnumProperty* GetComboProperty() const;
    wxEnumProperty* CreateComboProperty() const;

    virtual void SetValueList(const std::vector<TString>& valueList) override;
    virtual wxPGProperty* CreateWxProperty() = 0;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) = 0;
    virtual void SaveToXML(xml_node<>* pParentNode) override;
    virtual void LoadFromXML(xml_node<>* pNode) override;
    virtual void AnalyseUIParameter(const TCHAR* parameter) override;
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) = 0;

private:
    wxEnumProperty* m_pComboProperty;
    SharePtr<CPropertyTrigger> m_pVisibleWhenTrigger;
    std::set<CWxwidgetsPropertyBase*> m_effctProperties;
};


#endif