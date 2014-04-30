#include "stdafx.h"
#include "InfoPagePanel.h"
#include "../BDTWxApp.h"
#include "../BDTWxFrame.h"
#include "LogListCtrl.h"
#include <wx/editlbox.h>
#include <wx/srchctrl.h>
#include <wx/listctrl.h>
#include <wx/propgrid/manager.h>
#include "../ComponentSystem/wxWidgetsProperty/wxWidgetsPropertyBase.h"

CInfoPagePanel::CInfoPagePanel( wxWindow *parent, 
                               wxWindowID winid /*= wxID_ANY*/, 
                               const wxPoint& pos /*= wxDefaultPosition*/, 
                               const wxSize& size /*= wxDefaultSize*/, 
                               long style /*= wxTAB_TRAVERSAL | wxNO_BORDER*/, 
                               const wxString& name /*= wxPanelNameStr*/ )
: super(parent, winid, pos, size, style, name)
, m_lastPropertySearchTextUpdateTime(0)
, m_bPropertySearchTextUpdated(false)
, m_lastLogSearchTextUpdateTime(0)
, m_bLogSearchTextUpdated(false)
, m_bNeedUpdatePropertyByFilter(false)
, m_bNeedUpdateLogByFilter(false)

{
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* pPropertySizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBoxSizer* pPropertyGridSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("属性信息")), wxHORIZONTAL);
    wxBoxSizer* pPropertyGridSubSizer = new wxBoxSizer(wxVERTICAL);

    style = wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLTIPS;
    int extraStyle = wxPG_EX_MODE_BUTTONS | wxPG_EX_MULTIPLE_SELECTION;

    m_pPropertyGridManager = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style );
    wxPropertyGrid* pPropGrid = m_pPropertyGridManager->GetGrid();
    m_pPropertyGridManager->SetExtraStyle(extraStyle);

    m_pPropertyGridManager->SetValidationFailureBehavior( wxPG_VFB_MARK_CELL | wxPG_VFB_SHOW_MESSAGEBOX );
    pPropGrid->SetVerticalSpacing( 2 );
    wxPGCell cell;
    cell.SetText("Unspecified");
    cell.SetFgCol(*wxLIGHT_GREY);
    pPropGrid->SetUnspecifiedValueAppearance(cell);
    wxPropertyGridPage* myPage = new wxPropertyGridPage();
    m_pPropertyGridManager->AddPage(wxT("Property"), wxNullBitmap, myPage);

    //HACK: select -1 first, because there is a bug in wxwidgets, to init current page -1 is necessary.
    m_pPropertyGridManager->SelectPage(-1);
    m_pPropertyGridManager->SelectPage(myPage);

    pPropertyGridSubSizer->Add(m_pPropertyGridManager, 1, wxEXPAND, 5 );
    wxBoxSizer* pSearchSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, wxT("在列表中搜索"));
    pSearchSizer->Add(pStaticText, 0, wxALIGN_CENTER, 5 );
    m_pPropertySearchCtrl = new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), 0);
    m_pPropertySearchCtrl->ShowSearchButton(true);
    m_pPropertySearchCtrl->ShowCancelButton(true);
    pSearchSizer->Add(m_pPropertySearchCtrl, 1, wxEXPAND| wxALL, 5 );
    pPropertyGridSubSizer->Add(pSearchSizer, 0, wxEXPAND, 5);
    pPropertyGridSizer->Add( pPropertyGridSubSizer, 1, wxEXPAND , 5 );

    pPropertySizer->Add( pPropertyGridSizer, 1, wxEXPAND, 5 );
    wxStaticBoxSizer* pPropFunctionSizer = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, wxT("扩展功能")), wxVERTICAL );
    m_pFilterList = new wxEditableListBox( this, wxID_ANY, _T("过滤列表"), wxDefaultPosition, wxSize(-1, 150), wxEL_DEFAULT_STYLE, _T("Name")); 
    pPropFunctionSizer->Add( m_pFilterList, 0, wxEXPAND, 5 );
    wxStaticBoxSizer* pApplyCheckBoxSizer = new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, wxT("过滤选项")), wxHORIZONTAL );
    m_pApplyToPropertyCheckBox = new wxCheckBox( this, wxID_ANY, wxT("应用于属性列表"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pApplyToLogCheckBox = new wxCheckBox( this, wxID_ANY, wxT("应用于日志列表"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pReverseFilterCheckBox = new wxCheckBox( this, wxID_ANY, wxT("逆向过滤"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pReverseFilterCheckBox->Enable(false);
    pApplyCheckBoxSizer->Add(m_pApplyToPropertyCheckBox, 1, wxEXPAND, 5);
    pApplyCheckBoxSizer->Add(m_pApplyToLogCheckBox, 1, wxEXPAND, 5);
    pApplyCheckBoxSizer->Add(m_pReverseFilterCheckBox, 1, wxEXPAND, 5 );

    pPropFunctionSizer->Add( pApplyCheckBoxSizer, 0, wxEXPAND, 5 );

    pPropertySizer->Add( pPropFunctionSizer, 1, wxEXPAND, 5 );
    pMainSizer->Add(pPropertySizer, 1, wxEXPAND, 5);
    wxStaticBoxSizer* pLogSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("日志信息")), wxVERTICAL);

    m_pLogListboxCtrl = new CLogListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxLC_SINGLE_SEL);
    wxListItem itemCol;
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);

    itemCol.SetText(_T("日志内容"));
    // TODO: try to remove this hard code.
    itemCol.SetWidth(600);
    itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
    m_pLogListboxCtrl->InsertColumn(0, itemCol);
    itemCol.SetWidth(80);
    itemCol.SetText(_T("时间"));
    m_pLogListboxCtrl->InsertColumn(1, itemCol);
    itemCol.SetText(_T("位置"));
    m_pLogListboxCtrl->InsertColumn(2, itemCol);
    pLogSizer->Add( m_pLogListboxCtrl, 1, wxEXPAND, 5 );
    pLogSizer->Add( 0, 5, 0, wxEXPAND, 5 );

    wxBoxSizer* pLogFuncSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pAutoScrollCheckBox = new wxCheckBox( this, wxID_ANY, wxT("自动滚动"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAutoScrollCheckBox->SetValue(true);
    pLogFuncSizer->Add( m_pAutoScrollCheckBox, 0, wxALL, 5 );
    m_pLogSearchCtrl = new wxSearchCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), 0);
    m_pLogSearchCtrl->ShowSearchButton(true);
    m_pLogSearchCtrl->ShowCancelButton(true);
    wxMenu* pLogSearchMenu = new wxMenu;
    pLogSearchMenu->Append(wxID_ANY, wxT("按内容搜索"), wxT(""), wxITEM_RADIO);
    pLogSearchMenu->Append(wxID_ANY, wxT("按颜色搜索(16进制且不加0X前缀)"), wxT(""), wxITEM_RADIO);
    pLogSearchMenu->Append(wxID_ANY, wxT("按时间搜索"), wxT(""), wxITEM_RADIO);
    pLogSearchMenu->Append(wxID_ANY, wxT("按位置ID搜索"), wxT(""), wxITEM_RADIO);
    m_pLogSearchCtrl->SetMenu(pLogSearchMenu);

    pLogFuncSizer->Add( m_pLogSearchCtrl, 0, wxEXPAND, 5 );
    pLogFuncSizer->Add( 0, 0, 1, wxEXPAND, 5 );
    m_pClearLogBtn = new wxButton( this, wxID_ANY, wxT("清空日志"), wxDefaultPosition, wxDefaultSize, 0 );
    pLogFuncSizer->Add(m_pClearLogBtn, 0, wxALIGN_RIGHT, 5);
    pLogSizer->Add(pLogFuncSizer, 0, wxEXPAND, 5);
    pMainSizer->Add(pLogSizer, 1, wxEXPAND, 5);

    SetSizer(pMainSizer);
    Layout();
    pMainSizer->Fit( this );
    this->Connect(wxEVT_IDLE, wxCommandEventHandler(CInfoPagePanel::OnIdle), NULL, this);
    m_pPropertySearchCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CInfoPagePanel::OnPropertySearchTextUpdate ), NULL, this );
    m_pPropertySearchCtrl->Connect( wxEVT_IDLE, wxCommandEventHandler( CInfoPagePanel::OnPropertySearchIdle ), NULL, this );
    m_pPropertySearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( CInfoPagePanel::OnClickPropertySearchCancel ), NULL, this );
    m_pFilterList->Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(CInfoPagePanel::OnFilterListUpdated), NULL,  this );
    m_pFilterList->Connect(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(CInfoPagePanel::OnFilterListUpdated), NULL,  this );
    m_pClearLogBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CInfoPagePanel::OnClearLogBtnClick ), NULL, this );
    m_pLogSearchCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CInfoPagePanel::OnLogSearchTextUpdate ), NULL, this );
    m_pLogSearchCtrl->Connect( wxEVT_IDLE, wxCommandEventHandler( CInfoPagePanel::OnLogSearchIdle ), NULL, this );
    m_pLogSearchCtrl->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( CInfoPagePanel::OnClickLogSearchCancel ), NULL, this );
    pLogSearchMenu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CInfoPagePanel::OnClickLogSearchMenu), NULL, this);
    m_pPropertyGridManager->Connect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CInfoPagePanel::OnPropertyChanged), NULL, this);
    m_pPropertyGridManager->Connect(wxEVT_IDLE, wxCommandEventHandler(CInfoPagePanel::OnPropertyGridIdle), NULL, this);
    m_pApplyToPropertyCheckBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CInfoPagePanel::OnApplyFilterChanged), NULL, this);
    m_pApplyToLogCheckBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CInfoPagePanel::OnApplyFilterChanged), NULL, this);
    m_pReverseFilterCheckBox->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CInfoPagePanel::OnReverseFilterClicked), NULL, this);
    InitializeCriticalSection(&m_propertyRequestSection);
}

 CInfoPagePanel::~CInfoPagePanel()
{
    DeleteCriticalSection(&m_propertyRequestSection);
    for (wxPropertyGridIterator it = m_pPropertyGridManager->GetCurrentPage()->GetIterator(); !it.AtEnd(); ++it)
    {
        void* pData = it.GetProperty()->GetClientData();
        BEATS_SAFE_DELETE_ARRAY(pData);
        it.GetProperty()->SetClientData(NULL);
    }
    this->Disconnect(wxEVT_IDLE, wxCommandEventHandler(CInfoPagePanel::OnIdle), NULL, this);
    m_pClearLogBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CInfoPagePanel::OnClearLogBtnClick ), NULL, this );
    m_pPropertySearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CInfoPagePanel::OnPropertySearchTextUpdate ), NULL, this );
    m_pPropertySearchCtrl->Disconnect( wxEVT_IDLE, wxCommandEventHandler( CInfoPagePanel::OnPropertySearchIdle ), NULL, this );
    m_pPropertySearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( CInfoPagePanel::OnClickPropertySearchCancel ), NULL, this );
    m_pFilterList->Disconnect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(CInfoPagePanel::OnFilterListUpdated), NULL,  this );
    m_pFilterList->Disconnect(wxEVT_COMMAND_LIST_DELETE_ITEM, wxListEventHandler(CInfoPagePanel::OnFilterListUpdated), NULL,  this );
    m_pLogSearchCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CInfoPagePanel::OnLogSearchTextUpdate ), NULL, this );
    m_pLogSearchCtrl->Disconnect( wxEVT_IDLE, wxCommandEventHandler( CInfoPagePanel::OnLogSearchIdle ), NULL, this );
    m_pLogSearchCtrl->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( CInfoPagePanel::OnClickLogSearchCancel ), NULL, this );
    m_pLogSearchCtrl->GetMenu()->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(CInfoPagePanel::OnClickLogSearchMenu), NULL, this);
    m_pPropertyGridManager->Disconnect(wxEVT_PG_CHANGED, wxPropertyGridEventHandler(CInfoPagePanel::OnPropertyChanged), NULL, this);
    m_pApplyToPropertyCheckBox->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CInfoPagePanel::OnApplyFilterChanged), NULL, this);
    m_pApplyToLogCheckBox->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CInfoPagePanel::OnApplyFilterChanged), NULL, this);
}

void CInfoPagePanel::AppendLogText( const TCHAR* pLog, size_t logPos, const wxTextAttr* pTextAttr )
{
    BEATS_ASSERT(pLog != NULL);
    m_pLogListboxCtrl->AddLog(pLog, logPos, pTextAttr);
    if (m_pAutoScrollCheckBox->IsChecked())
    {
        m_pLogListboxCtrl->EnsureVisible(m_pLogListboxCtrl->GetItemCount() - 1);
    }
}

// TODO: Here is really a big trouble, Should try to refactor here for a better solution. the problem is:
// When we want to communicate with the main program, we don't know how to send data to it or receive data from it in a automatical way.
// so I use the simplest way: mark the type, convert data by the type, then operate.
void CInfoPagePanel::UpdateProperty( const TCHAR* propertyIdStr, size_t propertyId, void* value, EReflectPropertyType type )
{
    EnterCriticalSection(&m_propertyRequestSection);
    SPropertyRequest request;
    switch (type)
    {
    case eRPT_Bool:
        request.m_var = (*(bool*)value);
        break;
    case eRPT_Float:
        request.m_var = (double)(*(float*)value);
        break;
    case eRPT_Double:
        request.m_var = (*(double*)value);
        break;
    case eRPT_Str:
        request.m_var = (TCHAR*)value;
        break;
    case eRPT_Int:
        request.m_var = (*(long*)value);
        break;
    case eRPT_UInt:
        request.m_var = wxULongLong(*(long*)value);
        break;
    default:
        BEATS_ASSERT(false, _T("Not Implement yet!"));
        break;
    }
    request.m_type = type;
    request.m_id = propertyId;
    request.m_idStr.assign(propertyIdStr);
    m_request.push_back(request);
    LeaveCriticalSection(&m_propertyRequestSection);
}

void CInfoPagePanel::DeleteProperty(const TCHAR* pPropertyName)
{
    EnterCriticalSection(&m_propertyRequestSection);
    m_deleteRequest.push_back(pPropertyName);
    LeaveCriticalSection(&m_propertyRequestSection);
}

void CInfoPagePanel::OnPropertySearchTextUpdate( wxCommandEvent& /*event*/ )
{
    m_lastPropertySearchTextUpdateTime = GetTickCount();
    m_bPropertySearchTextUpdated = true;
}

void CInfoPagePanel::OnPropertySearchIdle( wxCommandEvent& /*event*/ )
{
    // Delay Handling search task to avoid UI input get stuck with lots of data.
    if (m_bPropertySearchTextUpdated && GetTickCount() - m_lastPropertySearchTextUpdateTime > 700)
    {
        m_bPropertySearchTextUpdated = false;
        m_bNeedUpdatePropertyByFilter = true;
    }
}

void CInfoPagePanel::OnClickPropertySearchCancel( wxCommandEvent& /*event*/ )
{
    m_pPropertySearchCtrl->Clear();
}

void CInfoPagePanel::OnFilterListUpdated( wxListEvent& event)
{
    m_bNeedUpdatePropertyByFilter = m_pApplyToPropertyCheckBox->IsChecked();
    m_bNeedUpdateLogByFilter = m_pApplyToLogCheckBox->IsChecked();
    event.Skip();
}

void CInfoPagePanel::OnClearLogBtnClick(wxCommandEvent& /*event*/)
{
    m_pLogListboxCtrl->ClearAllLog();
    m_pLogListboxCtrl->Refresh();
}
void CInfoPagePanel::OnLogSearchTextUpdate( wxCommandEvent& /*event*/ )
{
    m_lastLogSearchTextUpdateTime = GetTickCount();
    m_bLogSearchTextUpdated = true;
}

void CInfoPagePanel::OnLogSearchIdle( wxCommandEvent& /*event*/ )
{
    if (m_bLogSearchTextUpdated && GetTickCount() - m_lastLogSearchTextUpdateTime > 700)
    {
        m_bLogSearchTextUpdated = false;
        m_bNeedUpdateLogByFilter = true;
    }
}

void CInfoPagePanel::OnClickLogSearchCancel(wxCommandEvent& /*event*/)
{
    m_pLogSearchCtrl->Clear();
}

bool CInfoPagePanel::ShouldHideProperty( wxPGProperty* property, const wxArrayString& filters, const wxString& searchStr, bool& hideByFilterOrSearch)
{
    // Hide a property when it doesn't contain search text or in filters(while no reverse).
    bool bShouldHide = false;
    if (searchStr.Length() > 0)
    {
        bShouldHide = property->GetLabel().Find(m_pPropertySearchCtrl->GetValue()) == -1;
        hideByFilterOrSearch = false;
    }
    if (!bShouldHide)
    {
        for (size_t i = 0; i < filters.size(); ++i)
        {
            hideByFilterOrSearch = property->GetLabel().Find(filters[i]) != -1;
            if (hideByFilterOrSearch)
            {
                bShouldHide = true;
                break;
            }
        }
        // When reverse result is true / false, we will show / hide those item which contains any one of the filters.
        bool bGetReverseResult = m_pApplyToPropertyCheckBox->IsChecked() && m_pReverseFilterCheckBox->IsChecked();
        if (bGetReverseResult && filters.size() > 0)
        {
            bShouldHide = !bShouldHide;
        }
    }
    return bShouldHide;
}

void CInfoPagePanel::OnClickLogSearchMenu( wxMenuEvent& event )
{
    m_pLogSearchCtrl->Clear();
    m_bNeedUpdateLogByFilter = true;
    event.Skip();
}

void CInfoPagePanel::OnPropertyChanged( wxPropertyGridEvent& event )
{
    // TODO: Add a reminder to confirm if user really want to change property, with cancel logic(seems hard to do).
    TPropertyChangedHandler pFuncHandler = CBDTWxApp::GetBDTWxApp()->GetMainFrame()->GetPropertyChangedHandler();
    wxPGProperty* pProperty = event.GetProperty();
    if (pFuncHandler != NULL)
    {
        wxString pStr = pProperty->GetValueAsString();
        wxNotebookBase* pNoteBookBase = (wxNotebookBase*)GetParent();
        size_t* pClientData = (size_t*)pProperty->GetClientData();
        pFuncHandler((EReflectPropertyType)pClientData[0], pClientData[1], pNoteBookBase->GetPageText(pNoteBookBase->GetSelection()), (void*)pStr.ToStdWstring().c_str());
    }
}

void CInfoPagePanel::OnPropertyGridIdle(wxCommandEvent& /*event*/)
{
    EnterCriticalSection(&m_propertyRequestSection);
    for (size_t i = 0; i < m_request.size(); ++i)
    {
        wxPGProperty* pExistingProperty = m_pPropertyGridManager->GetGrid()->GetProperty(m_request[i].m_idStr);
        if (pExistingProperty != NULL)
        {
             pExistingProperty->SetValue(m_request[i].m_var);
        }
        else
        {
            wxPGProperty* pProperty = NULL;
            switch (m_request[i].m_type)
            {
            case eRPT_Bool:
                pProperty = new wxBoolProperty;
                break;
            case eRPT_Float:
            case eRPT_Double:
                pProperty = new wxFloatProperty;
                break;
            case eRPT_Str:
                pProperty = new wxStringProperty;
                break;
            case eRPT_Int:
                pProperty = new wxIntProperty;
                break;
            case eRPT_UInt:
                pProperty = new wxUIntProperty;
                break;
            default:
                BEATS_ASSERT(false, _T("Not Implement yet!"));
                break;
            }
            pProperty->SetValue(m_request[i].m_var);
            // Change string value is not allowed for now.
            pProperty->ChangeFlag(wxPG_PROP_READONLY, m_request[i].m_type == eRPT_Str);
            pProperty->SetName(m_request[i].m_idStr);
            pProperty->SetLabel(m_request[i].m_idStr);

            m_pPropertyGridManager->GetGrid()->Append(pProperty);
            wxArrayString filters;
            m_pFilterList->GetStrings(filters);
            size_t* pClientData = new size_t[2];
            pClientData[0] = m_request[i].m_type;
            pClientData[1] = m_request[i].m_id;
            pProperty->SetClientData(pClientData);
            bool bHideByFilter = false;
            bool bShouldHide = ShouldHideProperty(pProperty, filters, m_pPropertySearchCtrl->GetValue(), bHideByFilter);
            pProperty->Hide(bShouldHide);
            if (bHideByFilter)
            {
                m_propertyHiddenByFilter.insert(pProperty);
            }
        }
    }
    m_request.clear();

    for (size_t i = 0; i < m_deleteRequest.size(); ++i)
    {
        wxPropertyGrid* pGrid = m_pPropertyGridManager->GetGrid();
        wxPGProperty* pProperty = pGrid->GetProperty(m_deleteRequest[i]);
        if (pProperty != NULL)
        {
            std::set<wxPGProperty*>::iterator iter = m_propertyHiddenByFilter.find(pProperty);
            if (iter != m_propertyHiddenByFilter.end())
            {
                BEATS_ASSERT(pProperty->HasFlag(wxPG_PROP_HIDDEN) != 0);
                m_propertyHiddenByFilter.erase(iter);
            }
            void* pData = pProperty->GetClientData();
            BEATS_SAFE_DELETE_ARRAY(pData);
            pProperty->SetClientData(NULL);
            pGrid->DeleteProperty(m_deleteRequest[i].c_str());
        }
    }
    m_deleteRequest.clear();
    LeaveCriticalSection(&m_propertyRequestSection);
}

void CInfoPagePanel::OnApplyFilterChanged(wxCommandEvent& event)
{    
    m_bNeedUpdateLogByFilter = event.GetEventObject() == m_pApplyToLogCheckBox;
    m_bNeedUpdatePropertyByFilter = event.GetEventObject() == m_pApplyToPropertyCheckBox;
    bool bReverseCheckBoxEnable = m_pApplyToLogCheckBox->IsChecked() || m_pApplyToPropertyCheckBox->IsChecked();
    m_pReverseFilterCheckBox->Enable(bReverseCheckBoxEnable);
    event.Skip();
}

void CInfoPagePanel::UpdatePropertyVisibility(const wxArrayString& filterStr)
{
    m_propertyHiddenByFilter.clear();
    for (wxPropertyGridIterator it = m_pPropertyGridManager->GetCurrentPage()->GetIterator(); !it.AtEnd(); ++it)
    {
        wxPGProperty* p = *it;
        bool bHideByFilterOrSearch = false;
        bool bShouldHide = ShouldHideProperty(p, filterStr, m_pPropertySearchCtrl->GetValue(), bHideByFilterOrSearch);
        p->Hide(bShouldHide);
        if (bShouldHide && bHideByFilterOrSearch)
        {
            m_propertyHiddenByFilter.insert(p);
        }
    }
}

void CInfoPagePanel::UpdateLogVisibility(const wxArrayString& filters)
{
    m_pLogListboxCtrl->DeleteAllFilterConditions();
    // Add Search condition.
    if (m_pLogSearchCtrl->GetValue().Length() > 0)
    {
        AddSearchCondition();
    }
    if (m_pApplyToLogCheckBox->IsChecked())
    {
        if (m_pReverseFilterCheckBox->IsChecked())
        {
            std::vector<SFilterCondition*> filterVec;
            for (size_t i = 0; i < filters.size(); ++i)
            {
                SFilterCondition* condition = new SFilterCondition;
                condition->SetType(eLFT_Str);
                condition->m_pStrFilter->assign(filters[i]);
                condition->m_bReverse = false;
                filterVec.push_back(condition);
            }
            m_pLogListboxCtrl->AddFilterCondition(filterVec);
        }
        else
        {
            for (size_t i = 0; i < filters.size(); ++i)
            {
                SFilterCondition* condition = new SFilterCondition;
                condition->SetType(eLFT_Str);
                condition->m_pStrFilter->assign(filters[i]);
                condition->m_bReverse = true;
                std::vector<SFilterCondition*> filterVec;
                filterVec.push_back(condition);
                m_pLogListboxCtrl->AddFilterCondition(filterVec);
            }
        }
    }
    m_pLogListboxCtrl->UpdateList();
}

void CInfoPagePanel::OnIdle( wxCommandEvent& event )
{
    if (m_bNeedUpdatePropertyByFilter)
    {
        wxArrayString filters;
        if (m_pApplyToPropertyCheckBox->IsChecked())
        {
            m_pFilterList->GetStrings(filters);
        }
        UpdatePropertyVisibility(filters);
        m_bNeedUpdatePropertyByFilter = false;
    }
    if (m_bNeedUpdateLogByFilter)
    {
        wxArrayString filters;
        if (m_pApplyToLogCheckBox->IsChecked())
        {
            m_pFilterList->GetStrings(filters);
        }
        UpdateLogVisibility(filters);
        m_bNeedUpdateLogByFilter = false;
    }
    event.Skip();
}

void CInfoPagePanel::OnReverseFilterClicked( wxCommandEvent& event )
{
    m_bNeedUpdatePropertyByFilter = m_pApplyToPropertyCheckBox->IsChecked();
    m_bNeedUpdateLogByFilter = m_pApplyToLogCheckBox->IsChecked();
    event.Skip();
}

void CInfoPagePanel::AddSearchCondition()
{
    SFilterCondition* pLogSearchCondition = new SFilterCondition;
    for (size_t i = 0; i < eLFT_Count; ++i)
    {
        if (m_pLogSearchCtrl->GetMenu()->GetMenuItems()[i]->IsChecked())
        {
            pLogSearchCondition->SetType((ELogFilterType)(1 << i));
            break;
        }
    }
    ELogFilterType conditionType = pLogSearchCondition->GetType();
    if (conditionType == eLFT_Str)
    {
        pLogSearchCondition->m_pStrFilter->assign(m_pLogSearchCtrl->GetValue());
    }
    else if (conditionType == eLFT_Color)
    {
        unsigned long color = 0;
        m_pLogSearchCtrl->GetValue().ToULong(&color, 16);
        pLogSearchCondition->m_colorFilter = color;
    }
    else if (conditionType == eLFT_Time)
    {
        unsigned long time = 0;
        m_pLogSearchCtrl->GetValue().ToULong(&time);
        pLogSearchCondition->m_timeFilter = time;
    }
    else if (conditionType == eLFT_Pos)
    {
        unsigned long pos = 0;
        m_pLogSearchCtrl->GetValue().ToULong(&pos);
        pLogSearchCondition->m_posFilter = pos;
    }
    std::vector<SFilterCondition*> searchCondition;
    searchCondition.push_back(pLogSearchCondition);
    m_pLogListboxCtrl->AddFilterCondition(searchCondition);
}
