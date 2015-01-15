#include "stdafx.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include "PerformDetector/PerformDetector.h"
#include <dbghelp.h>

#define PERFORM_BITMAP_WIDTH 450
#define PERFORM_BITMAP_HIGHT 100

struct SPerformGraphicRecord
{
    SPerformGraphicRecord(wxSize bitmapSize)
        : m_lastUpdateSamplingCount(0) 
        , m_pPerformBitmap(new wxBitmap(bitmapSize))
        , m_lastResultPoint(wxPoint(0,PERFORM_BITMAP_HIGHT))
        , m_lastAveragePoint(wxPoint(0,0))
    {
    }

    ~SPerformGraphicRecord()
    {
        BEATS_SAFE_DELETE(m_pPerformBitmap);
    }

    void Reset()
    {
        m_lastUpdateSamplingCount = 0;
        m_lastResultPoint = wxPoint(0,PERFORM_BITMAP_HIGHT);
        m_lastAveragePoint = wxPoint(0,0);
        wxMemoryDC memDC;
        memDC.SelectObject(*m_pPerformBitmap);
        memDC.SetPen(wxPen("black",1));
        memDC.SetBrush( *wxBLACK_BRUSH );
        static const uint32_t EraseRectFactor = 3;
        memDC.DrawRectangle(wxRect(0, 0, PERFORM_BITMAP_WIDTH, PERFORM_BITMAP_HIGHT));
    }
    uint32_t m_lastUpdateSamplingCount;
    wxBitmap* m_pPerformBitmap;
    wxPoint m_lastResultPoint;
    wxPoint m_lastAveragePoint;
};

class CPerformResultTreeItemData : public wxTreeItemData
{
public:
    CPerformResultTreeItemData(const uint32_t id, wxSize bitmapSize)
        : m_recordId(id)
    {
        m_pGraphicRecord = new SPerformGraphicRecord(bitmapSize);
    }
    virtual ~CPerformResultTreeItemData()
    {
        BEATS_SAFE_DELETE(m_pGraphicRecord);
    }
    uint32_t GetRecordId(){return m_recordId;}
    void SetRecordId(const uint32_t& id){m_recordId = id;}
    SPerformGraphicRecord* GetGraphicRecord() {return m_pGraphicRecord;}

private:
    uint32_t m_recordId;
    SPerformGraphicRecord* m_pGraphicRecord;
};



void CBDTWxFrame::CreatePerformancePage()
{
    m_pPerformancePanel = new wxPanel( m_pNoteBook, wxID_ANY, wxPoint( -1,-1 ), wxSize( -1,-1 ), wxTAB_TRAVERSAL );
    wxBoxSizer* pPerformanceSizer;
    pPerformanceSizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* pPerFormanceTreeSizer;
    pPerFormanceTreeSizer = new wxBoxSizer( wxVERTICAL );

    m_pPerformanceTreeCtrl = new wxTreeCtrl( m_pPerformancePanel, wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), wxTR_DEFAULT_STYLE );
    pPerFormanceTreeSizer->Add( m_pPerformanceTreeCtrl, 1, wxEXPAND, 5 );

    pPerformanceSizer->Add( pPerFormanceTreeSizer, 0, wxEXPAND, 5 );

    wxBoxSizer* pPerformanceRightSideSizer;
    pPerformanceRightSideSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* pFunctionAeraSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticBoxSizer* pPerformanceInfoSizer;
    pPerformanceInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pPerformancePanel, wxID_ANY, wxT("统计信息") ), wxVERTICAL );

    pPerformanceInfoSizer->SetMinSize( wxSize( 450,-1 ) ); 
    wxBoxSizer* p1stRowInfoSizer;
    p1stRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* pCurResultSizer;
    pCurResultSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pCurResultLabel = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("当前耗时:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pCurResultLabel->Wrap( -1 );
    pCurResultSizer->Add( m_pCurResultLabel, 0, wxALL, 5 );

    m_pCurResultTxtLabel = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pCurResultTxtLabel->Wrap( -1 );
    pCurResultSizer->Add( m_pCurResultTxtLabel, 0, wxALL, 5 );

    p1stRowInfoSizer->Add( pCurResultSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* pAverageSizer;
    pAverageSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pAverageValueLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("平均耗时:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAverageValueLab->Wrap( -1 );
    pAverageSizer->Add( m_pAverageValueLab, 0, wxALL, 5 );

    m_pAverageValueTxtLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAverageValueTxtLab->Wrap( -1 );
    pAverageSizer->Add( m_pAverageValueTxtLab, 0, wxALL, 5 );

    p1stRowInfoSizer->Add( pAverageSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* pPeakValueSizer;
    pPeakValueSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pPeakValueLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("耗时峰值:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPeakValueLab->Wrap( -1 );
    pPeakValueSizer->Add( m_pPeakValueLab, 0, wxALL, 5 );

    m_pPeakValueTxtLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pPeakValueTxtLab->Wrap( -1 );
    pPeakValueSizer->Add( m_pPeakValueTxtLab, 0, wxALL, 5 );

    pPeakValueSizer->Add( 0, 0, 1, wxEXPAND, 5 );

    p1stRowInfoSizer->Add( pPeakValueSizer, 1, wxFIXED_MINSIZE, 5 );

    pPerformanceInfoSizer->Add( p1stRowInfoSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* p2ndRowInfoSizer;
    p2ndRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* pTotalRunTimeSizer;
    pTotalRunTimeSizer = new wxBoxSizer( wxHORIZONTAL );

    m_totalRunTimeLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("运行耗时累计:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
    m_totalRunTimeLab->Wrap( -1 );
    pTotalRunTimeSizer->Add( m_totalRunTimeLab, 0, wxALL, 5 );

    m_pTotalRunTimeTxtLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pTotalRunTimeTxtLab->Wrap( -1 );
    pTotalRunTimeSizer->Add( m_pTotalRunTimeTxtLab, 0, wxALL, 5 );

    p2ndRowInfoSizer->Add( pTotalRunTimeSizer, 1, 0, 5 );

    wxBoxSizer* pFrameCountSizer;
    pFrameCountSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pFrameCountLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("运行帧数累计:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
    m_pFrameCountLab->Wrap( -1 );
    pFrameCountSizer->Add( m_pFrameCountLab, 0, wxALL, 5 );

    m_pFrameCountTxtLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pFrameCountTxtLab->Wrap( -1 );
    pFrameCountSizer->Add( m_pFrameCountTxtLab, 0, wxALL, 5 );

    p2ndRowInfoSizer->Add( pFrameCountSizer, 1, 0, 5 );

    p2ndRowInfoSizer->Add( 0, 0, 1, wxEXPAND, 5 );

    pPerformanceInfoSizer->Add( p2ndRowInfoSizer, 1, wxEXPAND, 5 );

    wxBoxSizer* p3rdRowInfoSizer;
    p3rdRowInfoSizer = new wxBoxSizer( wxHORIZONTAL );

    m_performLocationLab = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxT("统计位置:"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
    m_performLocationLab->Wrap( -1 );
    p3rdRowInfoSizer->Add( m_performLocationLab, 0, wxALL, 5 );
    pPerformanceInfoSizer->Add( p3rdRowInfoSizer, 1, wxEXPAND, 5 );

    pFunctionAeraSizer->Add( pPerformanceInfoSizer, 1, wxALL | wxEXPAND, 5 );

    wxStaticBoxSizer* pFunctionButtonAeraSizer = new wxStaticBoxSizer( new wxStaticBox( m_pPerformancePanel, wxID_ANY, wxT("功能区") ), wxVERTICAL );
    m_pClearPeakValueBtn = new wxButton( m_pPerformancePanel, wxID_ANY, wxT("重置峰值"), wxPoint( -1,-1 ), wxSize( 60,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pClearPeakValueBtn, 0, wxRIGHT, 5 );
    m_pPausePerformanceBtn = new wxButton( m_pPerformancePanel, wxID_ANY, wxT("暂停监控"), wxPoint( -1,-1 ), wxSize( 60,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pPausePerformanceBtn, 0, wxRIGHT, 5 );
    m_pClearPerformanceBtn = new wxButton( m_pPerformancePanel, wxID_ANY, wxT("重新采集"), wxPoint( -1,-1 ), wxSize( 60,-1 ), 0 );
    pFunctionButtonAeraSizer->Add( m_pClearPerformanceBtn, 0, wxRIGHT, 5 );

    pFunctionAeraSizer->Add(pFunctionButtonAeraSizer, 1, wxALL | wxEXPAND, 5 );
    pPerformanceRightSideSizer->Add( pFunctionAeraSizer, 1, wxALL | wxEXPAND, 5 );

    wxStaticBoxSizer* pSubItemInfoSizer;
    pSubItemInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pPerformancePanel, wxID_ANY, wxT("子节点信息") ), wxVERTICAL );

    m_pSubItemInfoGrid = new wxGrid( m_pPerformancePanel, wxID_ANY, wxPoint( -1,-1 ), wxSize( -1,-1 ), 0 );

    // Grid
    m_pSubItemInfoGrid->CreateGrid( 8, 5 );
    m_pSubItemInfoGrid->EnableEditing( false );
    m_pSubItemInfoGrid->EnableGridLines( true );
    m_pSubItemInfoGrid->EnableDragGridSize( false );

    // Columns
    m_pSubItemInfoGrid->EnableDragColMove( false );
    m_pSubItemInfoGrid->EnableDragColSize( true );
    m_pSubItemInfoGrid->SetColLabelSize( 30 );
    m_pSubItemInfoGrid->SetColLabelValue( 0, wxT("当前耗时") );
    m_pSubItemInfoGrid->SetColLabelValue( 1, wxT("平均耗时") );
    m_pSubItemInfoGrid->SetColLabelValue( 2, wxT("耗时峰值") );
    m_pSubItemInfoGrid->SetColLabelValue( 3, wxT("运行耗时累计") );
    m_pSubItemInfoGrid->SetColLabelValue( 4, wxT("运行帧数累计") );
    m_pSubItemInfoGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    // Rows
    m_pSubItemInfoGrid->EnableDragRowSize( false );
    m_pSubItemInfoGrid->SetRowLabelSize( 100 );
    m_pSubItemInfoGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    m_pSubItemInfoGrid->AutoSize();
    // Label Appearance

    // Cell Defaults
    m_pSubItemInfoGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
    pSubItemInfoSizer->Add( m_pSubItemInfoGrid, 1, wxEXPAND | wxALL, 5 );

    pPerformanceRightSideSizer->Add( pSubItemInfoSizer, 1, wxALL| wxEXPAND, 5 );

    wxStaticBoxSizer* pPerformanceGrahpicSizer;
    pPerformanceGrahpicSizer = new wxStaticBoxSizer( new wxStaticBox( m_pPerformancePanel, wxID_ANY, wxT("曲线图") ), wxVERTICAL );
    pPerformanceGrahpicSizer->SetMinSize( wxSize( PERFORM_BITMAP_WIDTH, PERFORM_BITMAP_HIGHT ) ); 

    m_pPerformanceGraphics = new wxStaticBitmap( m_pPerformancePanel, wxID_ANY, wxBitmap(PERFORM_BITMAP_WIDTH, PERFORM_BITMAP_HIGHT), wxDefaultPosition, wxDefaultSize, 0 );
    pPerformanceGrahpicSizer->Add( m_pPerformanceGraphics, 0, wxALL, 5 );

    wxBoxSizer* pFirstRowForGraphicScroll;
    pFirstRowForGraphicScroll = new wxBoxSizer( wxHORIZONTAL );

    m_pSampleFrequencyLabel = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxString::Format("采样频率:%d", m_samplingFrequency), wxDefaultPosition, wxDefaultSize, 0 );
    m_pSampleFrequencyLabel->Wrap( -1 );
    pFirstRowForGraphicScroll->Add( m_pSampleFrequencyLabel, 0, wxALL, 5 );

    m_pSamplingFreqSlider = new wxSlider( m_pPerformancePanel, wxID_ANY, 1, 1, 300, wxDefaultPosition, wxSize(70,-1), wxSL_HORIZONTAL );
    m_pSamplingFreqSlider->SetValue(m_samplingFrequency);
    pFirstRowForGraphicScroll->Add( m_pSamplingFreqSlider, 0, 0, 5 );

    m_pLineMaxValueLabel = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxString::Format("曲线最大值:%d", m_lineMaxValue), wxDefaultPosition, wxDefaultSize, 0 );
    m_pLineMaxValueLabel->Wrap( -1 );
    pFirstRowForGraphicScroll->Add( m_pLineMaxValueLabel, 0, wxALL, 5 );


    m_pLineMaxValueSlider = new wxSlider( m_pPerformancePanel, wxID_ANY, 1, 1, 50, wxDefaultPosition, wxSize(70,-1), wxSL_HORIZONTAL );
    m_pLineMaxValueSlider->SetValue(m_lineMaxValue);
    pFirstRowForGraphicScroll->Add( m_pLineMaxValueSlider, 0, 0, 5 );

    m_pLineStepLabel = new wxStaticText( m_pPerformancePanel, wxID_ANY, wxString::Format("曲线步长:%d", m_lineStep), wxDefaultPosition, wxDefaultSize, 0 );
    m_pLineStepLabel->Wrap( -1 );
    pFirstRowForGraphicScroll->Add( m_pLineStepLabel, 0, wxALL, 5 );

    m_pLineStepSlider = new wxSlider( m_pPerformancePanel, wxID_ANY, 1, 1, 50, wxDefaultPosition, wxSize(70,-1), wxSL_HORIZONTAL );
    m_pLineStepSlider->SetValue(m_lineStep);
    pFirstRowForGraphicScroll->Add( m_pLineStepSlider, 0, 0, 5 );

    pPerformanceGrahpicSizer->Add( pFirstRowForGraphicScroll, 1, wxEXPAND, 5 );

    pPerformanceRightSideSizer->Add( pPerformanceGrahpicSizer, 1, wxALL | wxEXPAND, 5 );

    pPerformanceSizer->Add( pPerformanceRightSideSizer, 1, wxALL | wxEXPAND, 5 );

    m_pPerformancePanel->SetSizer( pPerformanceSizer );
    m_pPerformancePanel->Layout();
    pPerformanceSizer->Fit( m_pPerformancePanel );
    m_pNoteBook->AddPage( m_pPerformancePanel, wxT("性能分析"), true );

    m_performTreeItemMap[0] = m_pPerformanceTreeCtrl->AddRoot("Performance");
    m_pPerformanceTreeCtrl->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CBDTWxFrame::OnTreeItemChanged ), NULL, this );
    m_pClearPeakValueBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickClearPeakValueBtn ), NULL, this );
    m_pPausePerformanceBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickPerformancePauseBtn ), NULL, this );
    m_pClearPerformanceBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickPerformanceClearBtn ), NULL, this );
    m_pSamplingFreqSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CBDTWxFrame::OnSamplingFrenquecnyChanged ), NULL, this );
    m_pLineMaxValueSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler(CBDTWxFrame::OnLineMaxValueChanged), NULL, this);
    m_pLineStepSlider->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler(CBDTWxFrame::OnLineStepChanged), NULL, this);

}
void CBDTWxFrame::DestroyPerformancePage()
{
    m_pPerformanceTreeCtrl->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( CBDTWxFrame::OnTreeItemChanged ), NULL, this );
    m_pClearPeakValueBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickClearPeakValueBtn ), NULL, this );
    m_pPausePerformanceBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickPerformancePauseBtn ), NULL, this );
    m_pClearPerformanceBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CBDTWxFrame::OnClickPerformanceClearBtn ), NULL, this );
    m_pSamplingFreqSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( CBDTWxFrame::OnSamplingFrenquecnyChanged ), NULL, this );
    m_pLineMaxValueSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler(CBDTWxFrame::OnLineMaxValueChanged), NULL, this);
    m_pLineStepSlider->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler(CBDTWxFrame::OnLineStepChanged), NULL, this);
}

void CBDTWxFrame::OnTreeItemChanged( wxTreeEvent& event )
{
    m_pPerformanceTreeCtrl->Expand(event.GetItem());
    SPerformanceRecord* pRecord = GetRecord(event.GetItem());
    m_pSubItemInfoGrid->Scroll(0,0);
    if (pRecord)
    {
        RefreshItemGridRowLab(pRecord);
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(event.GetItem()));
        m_pPerformanceGraphics->SetBitmap(*pItemData->GetGraphicRecord()->m_pPerformBitmap);

        IMAGEHLP_LINEW64 info;
        info.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);
        DWORD displacement;
        BOOL Result = SymGetLineFromAddrW64(GetCurrentProcess(), pRecord->id, &displacement, &info);
        m_performLocationLab->SetLabel(Result == TRUE ? wxString::Format("统计位置:%s %d行", info.FileName, info.LineNumber) : "统计位置: 无法获取代码信息");
    }
    else
    {
        ClearGrid();
    }
}

void CBDTWxFrame::UpdatePerformData()
{
    static std::vector<SPerformanceResult*> output;
    m_updateValueMap.clear();
    bool dataExists = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetFirstResult(output);
    if (dataExists)
    {
        const uint32_t AppUpdateIndex = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetPerformUpdateCount();
        bool shouldUpdate = AppUpdateIndex % m_samplingFrequency == 0;
        if (shouldUpdate)
        {
            ++m_curSamplingCount;
        }

        for (uint32_t i = 0; i < output.size(); ++i)
        {
            SPerformanceResult* pCurResult = output[i];
            std::map<uint32_t, wxTreeItemId>::iterator iter =m_performTreeItemMap.find(pCurResult->id);
            if (iter == m_performTreeItemMap.end())//that's a new node!
            {
                AddPerformNode(CPerformDetector::GetInstance()->GetRecord(pCurResult->id));
            }
            m_updateValueMap[pCurResult->id] = pCurResult->result;
            if (shouldUpdate || pCurResult->result >= CPerformDetector::GetInstance()->GetRecord(pCurResult->id)->maxValue)
            {
                UpdatePerformGraphicData(pCurResult);
            }
        }
        SPerformanceRecord* pRecord = GetRecord(m_pPerformanceTreeCtrl->GetSelection());
        if (pRecord != NULL && m_pPerformancePanel->IsShown())
        {
            RefreshPerformBoard(pRecord);
            RefreshItemGridCell(pRecord);        
        }
    }
}

void CBDTWxFrame::AddPerformNode( const SPerformanceRecord* pRecordToAdd )
{
    BEATS_ASSERT(m_performTreeItemMap.find(pRecordToAdd->id) == m_performTreeItemMap.end(), _T("Can't add perform record twice for id : %d"), pRecordToAdd->id);
    SPerformanceRecord* validParentInTree = pRecordToAdd->pParent;
    uint32_t validParentId = validParentInTree == NULL ? 0 : validParentInTree->id;
    std::map<uint32_t, wxTreeItemId>::iterator iter = m_performTreeItemMap.find(validParentId);
    std::vector<SPerformanceRecord*> parentsNeedToAddAlso;
    while (iter == m_performTreeItemMap.end() && validParentInTree != NULL)
    {
        parentsNeedToAddAlso.push_back(validParentInTree);
        validParentInTree = validParentInTree->pParent;
        uint32_t validParentId = validParentInTree == NULL ? 0 : validParentInTree->id;
        iter = m_performTreeItemMap.find(validParentId);
    }
    wxTreeItemId parentId = validParentInTree == NULL ? m_performTreeItemMap[0] : iter->second;
    while (parentsNeedToAddAlso.size() != 0)
    {
        parentId = m_pPerformanceTreeCtrl->AppendItem(parentId
            ,parentsNeedToAddAlso.back()->typeStr, 
            -1, -1, new CPerformResultTreeItemData(parentsNeedToAddAlso.back()->id, m_pPerformanceGraphics->GetSize()));
        m_performTreeItemMap[parentsNeedToAddAlso.back()->id] = parentId;
        parentsNeedToAddAlso.pop_back();
    }
    m_performTreeItemMap[pRecordToAdd->id] = 
        m_pPerformanceTreeCtrl->AppendItem(parentId,
                                            pRecordToAdd->typeStr, 
                                            -1,
                                            -1,
                                            new CPerformResultTreeItemData(pRecordToAdd->id, 
                                                                            m_pPerformanceGraphics->GetSize()));
    //if this is the first node we add, we will select it.
    if (m_pPerformanceTreeCtrl->GetChildrenCount(m_pPerformanceTreeCtrl->GetRootItem()) == 1)
    {
        m_pPerformanceTreeCtrl->SelectItem(m_performTreeItemMap[pRecordToAdd->id]);
    }
}

void CBDTWxFrame::RefreshPerformBoard( const SPerformanceRecord* pRecord )
{
    if (pRecord != NULL)
    {
        RefreshInfo(pRecord);
    }
}

void CBDTWxFrame::RefreshInfo(const SPerformanceRecord* pRecord )
{
    std::map<uint32_t, float>::iterator iter = m_updateValueMap.find(pRecord->id);
    float curValue = iter == m_updateValueMap.end() ? 0.f : iter->second;
    m_pCurResultTxtLabel->SetLabelText(wxString::Format("%.2f", curValue));
    m_pTotalRunTimeTxtLab->SetLabelText(wxString::Format("%.2f", pRecord->totalValue));
    m_pFrameCountTxtLab->SetLabelText(wxString::Format("%d", pRecord->updateCount));
    m_pAverageValueTxtLab->SetLabelText(wxString::Format("%.2f", pRecord->totalValue / pRecord->updateCount));
    m_pPeakValueTxtLab->SetLabelText(wxString::Format("%.2f", pRecord->maxValue));
}

void CBDTWxFrame::RefreshItemGridRowLab(const SPerformanceRecord* pRecord)
{
    uint32_t childCount = pRecord->children.size();
    if (childCount > (uint32_t)m_pSubItemInfoGrid->GetRows() )
    {
        m_pSubItemInfoGrid->AppendRows(childCount - m_pSubItemInfoGrid->GetRows());
    }
    uint32_t i = 0;
    for (; i < childCount; ++i)
    {
        SPerformanceRecord* curRecord = pRecord->children[i];
        m_pSubItemInfoGrid->SetRowLabelValue(i, curRecord->typeStr);
    }
    ClearGridRow(i, m_pSubItemInfoGrid->GetRows() - i);
}

SPerformanceRecord* CBDTWxFrame::GetRecord( const wxTreeItemId& treeItemId )
{
    SPerformanceRecord* result = NULL;
    if (treeItemId.IsOk())
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(treeItemId));
        if (pItemData != NULL)
        {
            uint32_t recordId = pItemData->GetRecordId();
            result = CPerformDetector::GetInstance()->GetRecord(recordId);
        }
    }
    return result;
}

void CBDTWxFrame::RefreshItemGridCell( const SPerformanceRecord* pRecord)
{
    for (uint32_t i = 0; i < pRecord->children.size(); ++i)
    {
        SPerformanceRecord* curRecord = pRecord->children[i];
        std::map<uint32_t, float>::iterator iter = m_updateValueMap.find(curRecord->id);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%.2f", iter == m_updateValueMap.end() ? 0 : iter->second), i, 0);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%.2f", curRecord->totalValue / curRecord->updateCount), i, 1);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%.2f", curRecord->maxValue), i, 2);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%.2f", curRecord->totalValue), i, 3);
        m_pSubItemInfoGrid->SetCellValue(wxString::Format("%d", curRecord->updateCount), i, 4);
    }
}

void CBDTWxFrame::ClearGrid()
{
    m_pSubItemInfoGrid->ClearGrid();
    uint32_t rowCount = m_pSubItemInfoGrid->GetRows();
    for (uint32_t i = 0; i < rowCount; ++i)
    {
        m_pSubItemInfoGrid->SetRowLabelValue(i, "");
    }
}

void CBDTWxFrame::ClearGridRow( uint32_t startRowIndex, uint32_t count )
{
    uint32_t colNum = m_pSubItemInfoGrid->GetCols();
    for (uint32_t i = 0; i < count; ++i)
    {
        m_pSubItemInfoGrid->SetRowLabelValue(startRowIndex + i, "");
        for (uint32_t j = 0; j < colNum; ++j)
        {
            m_pSubItemInfoGrid->SetCellValue("", startRowIndex + i, j);
        }
    }
}

void CBDTWxFrame::OnClickClearPeakValueBtn( wxCommandEvent& /*event*/ )
{
    wxTreeItemId selectItem = m_pPerformanceTreeCtrl->GetSelection();
    if (selectItem.IsOk())
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(selectItem));
        SPerformanceRecord* pRecord = CPerformDetector::GetInstance()->GetRecord(pItemData->GetRecordId());
        ClearPeakValue(pRecord);
    }
}

void CBDTWxFrame::OnClickPerformancePauseBtn( wxCommandEvent& /*event*/ )
{
    bool bPauseOrResume = CPerformDetector::GetInstance()->PauseSwitcher();
    m_pPausePerformanceBtn->SetLabel(bPauseOrResume ? _T("继续监控") : _T("暂停监控"));
}

void CBDTWxFrame::OnClickPerformanceClearBtn(wxCommandEvent& /*event*/)
{
    CBDTWxApp::GetBDTWxApp()->ClearResultCache();
    CPerformDetector::GetInstance()->ClearAllResult();
    m_curSamplingCount = 0;
    for (std::map<uint32_t, wxTreeItemId>::iterator iter = m_performTreeItemMap.begin(); iter != m_performTreeItemMap.end(); ++iter)
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(iter->second));
        if (pItemData != NULL)
        {
            SPerformGraphicRecord* pGraphicRecord = pItemData->GetGraphicRecord();
            pGraphicRecord->Reset();
        }
    }
}

void CBDTWxFrame::ClearPeakValue( SPerformanceRecord* pRecord )
{
    if (pRecord != NULL)
    {
        pRecord->maxValue = 0;
        for (uint32_t i = 0; i < pRecord->children.size(); ++i)
        {
            ClearPeakValue(pRecord->children[i]);
        }
    }
}

void CBDTWxFrame::UpdatePerformGraphicData( const SPerformanceResult* pResult )
{
    CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(m_performTreeItemMap[pResult->id]));
    BEATS_ASSERT(pItemData != NULL, _T("Can't Get the Item Data!"));
    SPerformGraphicRecord* pGraphicRecord = pItemData->GetGraphicRecord();
    uint32_t samplingDelta = m_curSamplingCount - pGraphicRecord->m_lastUpdateSamplingCount;
    pGraphicRecord->m_lastUpdateSamplingCount = m_curSamplingCount;
    wxMemoryDC memDC;
    memDC.SelectObject(*pGraphicRecord->m_pPerformBitmap);
    //draw result line.
    memDC.SetPen(wxPen("green",1));
    const uint32_t bitmapWidth = pGraphicRecord->m_pPerformBitmap->GetWidth();
    const uint32_t bitmapHeight = pGraphicRecord->m_pPerformBitmap->GetHeight();

    if ( samplingDelta > (uint32_t)1 )//it means we haven't sampling this for some count.
    {
        BEATS_ASSERT((uint32_t)pGraphicRecord->m_lastResultPoint.x <= bitmapWidth, _T("Invalid X pos!"));
        if ((uint32_t)pGraphicRecord->m_lastResultPoint.x < bitmapWidth)
        {
            wxPoint nextDownPoint(pGraphicRecord->m_lastResultPoint.x + 1, bitmapHeight);
            memDC.DrawLine(pGraphicRecord->m_lastResultPoint, nextDownPoint);
        }
        pGraphicRecord->m_lastResultPoint.x = (pGraphicRecord->m_lastResultPoint.x + samplingDelta - 1) % bitmapWidth;
        pGraphicRecord->m_lastResultPoint.y = 0;
    }

    uint32_t thisPointX = pGraphicRecord->m_lastResultPoint.x + m_lineStep;
    if (thisPointX > bitmapWidth)//overflow of the bitmap width
    {
        thisPointX = 0;
    }
    const float CONST_RENDER_FACTOR = (float)bitmapHeight / m_lineMaxValue;

    uint32_t thisPointY = CONST_RENDER_FACTOR * pResult->result;
    if (thisPointY > bitmapHeight)//overflow of the bitmap height
    {
        thisPointY = bitmapHeight;
    }

    wxPoint curResultPoint(thisPointX, bitmapHeight - thisPointY);
    if (pGraphicRecord->m_lastResultPoint.x > curResultPoint.x)
    {
        pGraphicRecord->m_lastResultPoint.x = 0;
    }
    memDC.DrawLine(pGraphicRecord->m_lastResultPoint, curResultPoint);
    //cover black to the existing data.
    memDC.SetPen(wxPen("black",1));
    memDC.SetBrush( *wxBLACK_BRUSH );
    static const uint32_t EraseRectFactor = 3;
    memDC.DrawRectangle(wxRect(curResultPoint.x + 1, 0, m_lineStep * EraseRectFactor, bitmapHeight));
    memDC.SelectObject(wxNullBitmap);

    wxTreeItemId selectItem = m_pPerformanceTreeCtrl->GetSelection();

    if (selectItem.IsOk() && m_pPerformancePanel->IsShown())
    {
        CPerformResultTreeItemData* pItemData = static_cast<CPerformResultTreeItemData*>(m_pPerformanceTreeCtrl->GetItemData(selectItem));
        if (pItemData != NULL && pItemData->GetRecordId() == pResult->id)
        {
            m_pPerformanceGraphics->SetBitmap(*pGraphicRecord->m_pPerformBitmap);
            m_pPerformanceGraphics->RefreshRect(wxRect(pGraphicRecord->m_lastResultPoint,curResultPoint), false);
        }
    }

    pGraphicRecord->m_lastResultPoint = curResultPoint;
    return;                
}

void CBDTWxFrame::OnSamplingFrenquecnyChanged( wxScrollEvent& event )
{
    m_samplingFrequency = event.GetPosition();
    m_pSampleFrequencyLabel->SetLabel(wxString::Format("采样频率:%d", m_samplingFrequency));
}

void CBDTWxFrame::OnLineMaxValueChanged( wxScrollEvent& event )
{
    m_lineMaxValue = event.GetPosition();
    m_pLineMaxValueLabel->SetLabel(wxString::Format("曲线最大值:%d", m_lineMaxValue));
}

void CBDTWxFrame::OnLineStepChanged( wxScrollEvent& event )
{
    m_lineStep = event.GetPosition();
    m_pLineStepLabel->SetLabel(wxString::Format("曲线步长:%d", m_lineStep));
}