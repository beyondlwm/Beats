#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_KMDATAPROPERTY_VEC2FPROPERTYDESCRIPTION_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_KMDATAPROPERTY_VEC2FPROPERTYDESCRIPTION_H__INCLUDE

#include "ListPropertyDescription.h"

class CVec2PropertyDescription : public CListPropertyDescription
{
    typedef CListPropertyDescription super;
public:
    CVec2PropertyDescription(CSerializer* pSerializer);
    CVec2PropertyDescription(const CVec2PropertyDescription& rRef);

    virtual ~CVec2PropertyDescription();

    virtual bool AnalyseUIParameterImpl(const std::vector<TString>& parameterUnit, bool bSerializePhase = false) override;
    virtual bool IsDataSame(bool bWithDefaultOrXML) override;
    virtual CPropertyDescriptionBase* CreateNewInstance() override;
    virtual CPropertyDescriptionBase* Clone(bool bCloneValue) override;
    virtual bool IsContainerProperty() override;

private:
    virtual void ResetChildName() override;
    virtual void GetCurrentName(wxString& strName) override;

};

#endif