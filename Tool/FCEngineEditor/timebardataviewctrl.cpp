#include "stdafx.h"
#include "timebardataviewctrl.h"
#include "timebarframe.h"
#include "timebarlistmodel.h"
#include "timebarcheckrenderer.h"
#include <wx/dcbuffer.h>

#define COLUMNWIDTH 25

BEGIN_EVENT_TABLE(TimeBarDataViewCtrl, wxDataViewCtrl)
EVT_SCROLLWIN(TimeBarDataViewCtrl::OnScroll)
EVT_DATAVIEW_SELECTION_CHANGED(wxID_ANY, TimeBarDataViewCtrl::OnSelect)
END_EVENT_TABLE()

ArrayOfAnimation::~ArrayOfAnimation()
{

}

TimeBarDataViewCtrl::TimeBarDataViewCtrl( wxWindow *parent, wxWindowID id,
                     const wxPoint& pos,const wxSize& size, long style,
                     const wxValidator& validator,const wxString& name)
                     :wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    m_pSyncWnd = NULL;
    m_list_model = new TimeBarListModel();
    AssociateModel( m_list_model.get() );
    InitCtrl();
}


int TimeBarDataViewCtrl::GetRow(const wxDataViewItem & item)
{
    return GetRowByItem(item);
}

void TimeBarDataViewCtrl::SyncWith(wxScrollHelperBase *win)
{
    m_pSyncWnd = win;
}

void TimeBarDataViewCtrl::AddItem( const wxString &text, bool visible, bool islock )
{
    m_list_model->AddItem(text, visible, islock);
}

TimeBarListModel* TimeBarDataViewCtrl:: GetUserModel()const
{
    return m_list_model.get();
}

bool TimeBarDataViewCtrl::InitCtrl()
{
    ShowScrollbars(wxSHOW_SB_NEVER,wxSHOW_SB_NEVER);
    AppendTextColumn(_T("Layer"),
        TimeBarListModel::Col_EditableText,
        wxDATAVIEW_CELL_EDITABLE,
        wxCOL_WIDTH_AUTOSIZE);

    AppendColumn(
        new wxDataViewColumn(_T("V"),
        new TimeBarCheckRenderer(TimeBarListModel::Col_IsVisible),
        TimeBarListModel::Col_IsVisible,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0)
        );

    AppendColumn(
        new wxDataViewColumn(_T("L"),
        new TimeBarCheckRenderer(TimeBarListModel::Col_Islocked),
        TimeBarListModel::Col_Islocked,
        COLUMNWIDTH,
        wxALIGN_CENTER,
        0)
        );

    return true;
}

void TimeBarDataViewCtrl::OnScroll(wxScrollWinEvent& /*event*/)
{
    if (m_pSyncWnd != NULL)
    {
        m_pSyncWnd->Scroll(GetViewStart());
    }
}

void TimeBarDataViewCtrl::OnSelect(wxDataViewEvent& event)
{
    wxDataViewItem item = this->GetSelection();
    int row = this->GetRowByItem(item);
    ((TimeBarFrame*)(GetParent()->GetParent()))->SetCurrentSelect(row);
    event.Skip(false);
}
