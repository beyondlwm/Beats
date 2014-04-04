#ifndef FCENGINEEDITOR_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE
#define FCENGINEEDITOR_ENGINEPROPERTYGRIDMANAGER_H__INCLUDE

#include <wx/propgrid/manager.h>

class CPropertyDescriptionBase;
class CComponentEditorProxy;
class CWxwidgetsPropertyBase;

class EnginePropertyGirdManager : public wxPropertyGridManager
{
public:
    EnginePropertyGirdManager();
    virtual ~EnginePropertyGirdManager();
    virtual wxPropertyGrid* CreatePropertyGrid() const;

    void InsertInPropertyGrid(const std::vector<CPropertyDescriptionBase*>& pProperties, wxPGProperty* pParent = NULL);
    void InsertComponentsInPropertyGrid(CComponentEditorProxy* pComponent, wxPGProperty* pParent = NULL);
    void UpdatePropertyVisiblity(CWxwidgetsPropertyBase* pPropertyBase);
    void RequestToUpdatePropertyGrid( );
    void InitComponentsPage();
    void OnComponentPropertyChanged(wxPropertyGridEvent& event);
    void OnComponentPropertyChangedImpl(wxPGProperty* pProperty);
    wxPGProperty* GetPGPropertyByBase(CPropertyDescriptionBase* pBase);

private:
//#error No place handle this flag, check BEATS::void CBDTWxFrame::OnComponentPropertyGridIdle( wxIdleEvent& /*event*/ )
    bool m_bNeedUpdatePropertyGrid;

DECLARE_EVENT_TABLE()
};

#endif