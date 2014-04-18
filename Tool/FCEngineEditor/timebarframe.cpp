#include "stdafx.h"
#include "timebarframe.h"
#include "wx/frame.h"
#include "wx/dcbuffer.h"
#include "wx/sizer.h"
#include "Render/Animation.h"
#include "timebardataviewctrl.h"
#include "timebarlistmodel.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "EditAnimationDialog.h"
#include "Render/AnimationController.h"
#include "GLCanvas.h"

#define BUTTONSIZE 24
#define SASHPOSITION 130

BEGIN_EVENT_TABLE(TimeBarFrame, wxSplitterWindow)
    EVT_SIZE(TimeBarFrame::OnSize)
    EVT_SPLITTER_SASH_POS_CHANGED(wxID_ANY, TimeBarFrame::OnPositionChanged)
    EVT_SPLITTER_SASH_POS_CHANGING(wxID_ANY, TimeBarFrame::OnPositionChanging)
    EVT_SPINCTRL(wxID_ANY, TimeBarFrame::OnSpinCtrl)
    EVT_TEXT_ENTER(wxID_ANY, TimeBarFrame::OnSpinTextEnter)
    EVT_DATAVIEW_SELECTION_CHANGED(wxID_ANY, TimeBarFrame::OnSelect)
END_EVENT_TABLE()

TimeBarFrame::TimeBarFrame(wxWindow *parent)
: wxSplitterWindow(parent,
                   wxID_ANY,
                   wxDefaultPosition, 
                   wxDefaultSize,
                   wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN)
{
    SetMinimumPaneSize(150);
    SetSashGravity(0.1);
    m_iCursorPositionX = 1;
    m_iCurrentSelect = -1;
    m_pFrame = parent;
    m_iItemId = 1;

    wxBoxSizer* rightpanelsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* righttopsizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* leftpanelsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* leftdownsizer = new wxBoxSizer(wxHORIZONTAL);
    wxPanel* rightpanelbase = new wxPanel(this);
    wxPanel* leftpanelbase = new wxPanel(this);
    wxStaticText* pText = new wxStaticText(leftpanelbase, wxID_ANY, _T("fps: "));
    wxPanel* pSpacePanel = new wxPanel(rightpanelbase, wxID_ANY, wxDefaultPosition, wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    pSpacePanel->SetMinSize(wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    pSpacePanel->SetMaxSize(wxSize(DEFAULTSCROLLBARWIDTH, SCALEBARHIGHT));
    m_pButtonAdd = new wxButton(leftpanelbase, wxID_ANY, "+");
    m_pButtonDelete = new wxButton(leftpanelbase, wxID_ANY, "-");
    m_pSpinCtrl = new wxSpinCtrl(leftpanelbase, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxSP_ARROW_KEYS|wxSP_VERTICAL|wxTE_PROCESS_ENTER, 0, MAXFPS, 0);

    m_pTimeLineDataViewCtrl = new TimeBarDataViewCtrl(leftpanelbase, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_NO_SB);
    m_pScalebar = new TimeBarScale(rightpanelbase);
    m_pContainer = new TimeBarItemContainer(rightpanelbase);
    m_pScalebar->SetSplitterwnd(this);
    m_pContainer->SetSplitterwnd(this);
    wxSize size(BUTTONSIZE, BUTTONSIZE);
    int iSpinCtrlWidth = BUTTONSIZE*2.5;
    m_pButtonAdd->SetMinSize(size);
    m_pButtonDelete->SetMinSize(size);
    m_pButtonDelete->SetMinSize(size);
    m_pSpinCtrl->SetMinSize(wxSize(iSpinCtrlWidth, BUTTONSIZE));
    SplitVertically(leftpanelbase, rightpanelbase, SASHPOSITION);
    rightpanelbase->SetSizer(rightpanelsizer);
    leftpanelbase->SetSizer(leftpanelsizer);
    rightpanelsizer->Add(righttopsizer, 0, wxGROW|wxALL, 0 );
    righttopsizer->Add(m_pScalebar, 1, wxGROW|wxALL, 0 );
    righttopsizer->Add(pSpacePanel, 0, wxGROW|wxALL, 0 );
    rightpanelsizer->Add(m_pContainer, 1, wxGROW|wxALL, 0 );
    leftpanelsizer->Add(m_pTimeLineDataViewCtrl, 1, wxGROW|wxALL, 0 );
    leftpanelsizer->Add(leftdownsizer, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pButtonAdd, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pButtonDelete, 0, wxGROW|wxALL, 0);
    leftdownsizer->AddStretchSpacer(1);
    leftdownsizer->Add(pText, 0, wxGROW|wxALL, 0);
    leftdownsizer->Add(m_pSpinCtrl, 0, wxGROW|wxALL, 0);

    m_pContainer->SyncWith(m_pTimeLineDataViewCtrl,m_pScalebar);
    m_pTimeLineDataViewCtrl->SyncWith(m_pContainer);

    m_pButtonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TimeBarFrame::OnBtnAddClick), NULL, this);
    m_pButtonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TimeBarFrame::OnBtnDeleteClick), NULL, this);
    SetFocus();
}

TimeBarFrame::~TimeBarFrame()
{

}

void TimeBarFrame::OnBtnAddClick(wxCommandEvent& /*event*/)
{
    AddItems();
}

void TimeBarFrame::OnBtnDeleteClick(wxCommandEvent& /*event*/)
{
    DeleteItems(m_iCurrentSelect);
    m_iCurrentSelect = -1;
}

void TimeBarFrame::OnPositionChanged(wxSplitterEvent& event)
{
    ResetLeft();
    event.Skip();
}

void TimeBarFrame::OnPositionChanging(wxSplitterEvent& event)
{
    ResetLeft();
    event.Skip();
}

void TimeBarFrame::OnSpinCtrl(wxSpinEvent& event)
{
    int iData = event.GetValue();
    SetFpsToAnimation(iData);
}

void TimeBarFrame::OnSpinTextEnter(wxCommandEvent& event)
{
    unsigned long iValue  = 0;
    if (event.GetString().ToCULong(&iValue))
    {
        SetFpsToAnimation(iValue);
    }
}

void TimeBarFrame::OnSize(wxSizeEvent & event)
{
    super::OnSize(event);
    ResetLeft();
    GetWindow1()->Refresh(false);
}

void TimeBarFrame::ResetLeft()
{
    m_pTimeLineDataViewCtrl->RefreshSize();
}

void TimeBarFrame::AddItems()
{
    int iCount = m_pTimeLineDataViewCtrl->GetUserModel()->GetCount();
    wxVariant Variant;
    wxString strCurrentName;
    wxString strNewName;
    for (int j = 0; j < iCount; j++)
    {
        strNewName = wxString::Format(_T("new item %d"),m_iItemId);
        for (int i = 0; i < iCount; i++)
        {
            m_pTimeLineDataViewCtrl->GetUserModel()->GetValueByRow(Variant, i, 0);
            strCurrentName = Variant.GetString();
            if (strCurrentName == strNewName)
            {
                m_iItemId++;
                strNewName = wxString::Format(_T("new item %d"),m_iItemId);
                break;
            }
        }
    }
    m_pTimeLineDataViewCtrl->AddItem(strNewName, true, false);
    m_pContainer->AddItem(new TimeBarItem(m_pContainer));
}

void TimeBarFrame::AddItems( SharePtr<CAnimation> pAnimationItem )
{
    TimeBarItem* pItem = new TimeBarItem(m_pContainer);
    int iCount = pAnimationItem->GetFrameCount();
    m_pContainer->SetItemCellsCount(iCount);
    m_pContainer->AddItem(pItem);    
    pItem->SetDataCells(iCount);
    wxString name = pAnimationItem->GetFilePath();
    m_pTimeLineDataViewCtrl->AddItem(name.AfterLast('\\'), true, false);
}

void TimeBarFrame::DeleteItems(int index)
{
    if (index != -1)
    {
        ((TimeBarListModel*)m_pTimeLineDataViewCtrl->GetModel())->DeleteItem(index);
        m_pContainer->DeleteItem(index);
    }
}

void TimeBarFrame::ClickOnScaleBar()
{
    if (m_bclickonscalebar)
    {
        m_pContainer->GetCurrentSelect().m_iSelectColumnBegin = -1;
        m_pContainer->GetCurrentSelect().m_iSelectColumnEnd = -1;
        m_pContainer->GetCurrentSelect().m_iSelectRowBegin = -1;
        m_pContainer->GetCurrentSelect().m_iSelectRowEnd = -1;
    }

    m_pContainer->SelectItems();
    m_pScalebar->Refresh(false);
}

void TimeBarFrame::SetCursorPositionX(int pos)
{
    EditAnimationDialog* pDialog = (EditAnimationDialog*)GetFrameWindow();
    if (pos < 0)
    {
        pos = 0;
    }
    int iCellWidth = m_pScalebar->GetScaleWidth();
    int iIndex = pos - iCellWidth * 0.5F;
        iIndex = iIndex / iCellWidth;
    m_iCursorPositionX = pos;
    m_pContainer->SelectItems();
    if (pDialog->IsLoadAllAnimationResource())
    {
        if (iIndex < pDialog->GetAnimation()->GetFrameCount())
        {
            //((GLAnimationCanvas*)pDialog->GetCanvas())->GetModel()->GetAnimationController()->GoToFrame(iIndex);
        }
    }
    m_pScalebar->Refresh(false);
}

void TimeBarFrame::SetCurrentSelect(int iCurrentSelect)
{
    m_iCurrentSelect = iCurrentSelect;
}

int TimeBarFrame::GetCurrentSelect() const
{
    return m_iCurrentSelect;
}

TimeBarScale* TimeBarFrame:: GetScalebar()
{
    return m_pScalebar;
}

void TimeBarFrame::SetClickOnScalebar(bool bIsClicked)
{
    m_bclickonscalebar = bIsClicked;
}

bool TimeBarFrame::IsClickOnScalebar() const
{
    return m_bclickonscalebar;
}

TimeBarItemContainer* TimeBarFrame::GetItemContainer()
{
    return m_pContainer;
}

void TimeBarFrame::SetFps( size_t iFps )
{
    m_pSpinCtrl->SetValue(iFps);
}

void TimeBarFrame::SetFpsToAnimation( size_t /*iFps*/ )
{
    //TODO: Set FPS
}

void TimeBarFrame::SetFrameWindow( wxWindow* pWindow )
{
    m_pFrame = pWindow;
}

wxWindow* TimeBarFrame::GetFrameWindow()
{
    return m_pFrame;
}

void TimeBarFrame::SetCurrentCursor( int pos )
{
    wxPoint point = m_pContainer->GetViewStart();
    int iPositionX = pos * m_pScalebar->GetScaleWidth();
    m_iCursorPositionX = iPositionX + m_pScalebar->GetScaleWidth() * 0.5F;
    int iRange = m_pContainer->GetSize().x / m_pContainer->GetCellWidth();
    
    if (pos >= iRange + point.x)
    {
        point.x += pos - point.x - 10;
        m_pContainer->Scroll(point);
    }
    else if (pos < point.x)
    {
        point.x = pos;
        m_pContainer->Scroll(point);
    }
    m_pContainer->Refresh(false);
    m_pScalebar->Refresh(false);
}

int TimeBarFrame::GetCursorPositionX()
{
    return m_iCursorPositionX;
}

int TimeBarFrame::GetItemCount()
{
    return ((TimeBarListModel*)m_pTimeLineDataViewCtrl->GetModel())->GetCount();
}

void TimeBarFrame::OnSelect(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    int row = (int)item.GetID() - 1;
    SetCurrentSelect(row);
}