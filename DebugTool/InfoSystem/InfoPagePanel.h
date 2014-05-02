#ifndef BEATS_DEBUGTOOL_INFOSYSTEM_INFOPAGEPANEL_H__INCLUDE
#define BEATS_DEBUGTOOL_INFOSYSTEM_INFOPAGEPANEL_H__INCLUDE

class wxPropertyGridManager;
class wxTextCtrl;
class wxPGProperty;
class wxEditableListBox;
class wxSearchCtrl;
class wxListEvent;
class CLogListCtrl;
class wxPropertyGridEvent;
class CWxwidgetsPropertyBase;
struct SFilterCondition;
enum EReflectPropertyType;

struct SPropertyRequest
{
    size_t m_id;
    wxVariant m_var;
    TString m_idStr;
    EReflectPropertyType m_type;
};

class CInfoPagePanel : public wxPanel
{
    typedef wxPanel super;
public:
    CInfoPagePanel(wxWindow *parent,
                    wxWindowID winid = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);


    virtual ~CInfoPagePanel();
    void AppendLogText(const TCHAR* pLog, size_t logPos, const wxTextAttr* textAttr = NULL);
    void UpdateProperty(const TCHAR* propertyIdStr, size_t propertyId, void* value, EReflectPropertyType type);
    void DeleteProperty(const TCHAR* pPropertyName);
private:
    bool ShouldHideProperty(wxPGProperty* property, const wxArrayString& filters, const wxString& searchStr, bool& hideByFilterOrSearch);
    void UpdatePropertyVisibility(const wxArrayString& filterStr);
    void UpdateLogVisibility(const wxArrayString& filterStr);
    void AddSearchCondition();

    virtual void OnPropertySearchTextUpdate( wxCommandEvent& event );
    virtual void OnPropertySearchIdle( wxCommandEvent& event );
    virtual void OnClickPropertySearchCancel(wxCommandEvent& event);
    virtual void OnFilterListUpdated(wxListEvent& event);
    virtual void OnClearLogBtnClick( wxCommandEvent& event );
    virtual void OnLogSearchTextUpdate( wxCommandEvent& event );
    virtual void OnLogSearchIdle( wxCommandEvent& event );
    virtual void OnClickLogSearchCancel(wxCommandEvent& event);
    virtual void OnClickLogSearchMenu(wxMenuEvent& event);
    virtual void OnPropertyChanged(wxPropertyGridEvent& event);
    virtual void OnPropertyGridIdle(wxCommandEvent& event);
    virtual void OnApplyFilterChanged(wxCommandEvent& event);
    virtual void OnReverseFilterClicked(wxCommandEvent& event);
    virtual void OnIdle(wxCommandEvent& event);
private:
    bool m_bPropertySearchTextUpdated;
    bool m_bLogSearchTextUpdated;
    bool m_bNeedUpdatePropertyByFilter;
    bool m_bNeedUpdateLogByFilter;
    wxPropertyGridManager* m_pPropertyGridManager;
    CLogListCtrl* m_pLogListboxCtrl;
    wxEditableListBox* m_pFilterList;
    wxSearchCtrl* m_pPropertySearchCtrl;
    wxButton* m_pClearLogBtn;
    wxCheckBox* m_pAutoScrollCheckBox;
    wxCheckBox* m_pApplyToPropertyCheckBox;
    wxCheckBox* m_pApplyToLogCheckBox;
    wxCheckBox* m_pReverseFilterCheckBox;
    wxSearchCtrl* m_pLogSearchCtrl;
    size_t m_lastPropertySearchTextUpdateTime;
    size_t m_lastLogSearchTextUpdateTime;
    std::set<wxPGProperty*> m_propertyHiddenByFilter;
    std::vector<SPropertyRequest> m_request;
    std::vector<TString> m_deleteRequest;
    CRITICAL_SECTION m_propertyRequestSection;
};
#endif
