#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_WXWIDGETSPROPERTYBASE_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_WXWIDGETSPROPERTYBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Property/PropertyDescriptionBase.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "PropertyTrigger.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

class CWxwidgetsPropertyBase : public CPropertyDescriptionBase
{
    typedef CPropertyDescriptionBase super;
public:
    CWxwidgetsPropertyBase(EPropertyType type);
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
    virtual bool AnalyseUIParameter(const TCHAR* parameter, bool bSerializePhase = false);
    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false) = 0;

private:
    SharePtr<CPropertyTrigger> m_pVisibleWhenTrigger;
    std::set<CWxwidgetsPropertyBase*> m_effctProperties;
};


#endif