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

    virtual wxPGProperty* CreateWxProperty() = 0;
    virtual void SetValue(wxVariant& value, bool bSaveValue = true) = 0;
    virtual void SaveToXML(TiXmlElement* pParentNode);
    virtual void LoadFromXML(TiXmlElement* pNode);
    virtual void AnalyseUIParameter(const TCHAR* parameter);
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit) = 0;

private:
    SharePtr<CPropertyTrigger> m_pVisibleWhenTrigger;
    std::set<CWxwidgetsPropertyBase*> m_effctProperties;
};


#endif