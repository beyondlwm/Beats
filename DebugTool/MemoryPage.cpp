#include "stdafx.h"
#include "MemoryDetector/MemoryDetector.h"
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include <wx/statline.h>
#include <Psapi.h>
#include <shellapi.h>

static const size_t MemoryGridStartPosOffset = 100;

void CBDTWxFrame::CreateMemoryPage()
{
    m_pMemoryPanel = new wxPanel( m_pNoteBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* pMemorySizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* pFirstRowSizer = new wxBoxSizer(wxHORIZONTAL);
    wxString strMemoryViewChoiceString[] = { wxT("按照分配位置查看"), wxT("按照分配尺寸查看"), wxT("按照分配时间查看"), wxT("按照分配地址查看") };
    int uChoiceCount = sizeof( strMemoryViewChoiceString ) / sizeof( wxString );
    m_pMemoryViewChoice = new wxChoice( m_pMemoryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, uChoiceCount, strMemoryViewChoiceString, 0 );
    m_pMemoryViewChoice->SetSelection( 0 );
    pFirstRowSizer->Add(m_pMemoryViewChoice, 0, wxALL, 5);

    m_pFilterLabel = new wxStaticText( m_pMemoryPanel, wxID_ANY, wxT("过滤"), wxDefaultPosition, wxDefaultSize, 0 );
    pFirstRowSizer->Add(m_pFilterLabel, 0, wxALL, 5);
    m_pFilterTextComboBox = new wxComboBox(m_pMemoryPanel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 20), 0, NULL, wxTE_PROCESS_ENTER|wxCB_DROPDOWN );
    //m_pFilterTextComboBox->SetIgnoreEnterFlag(true);
    pFirstRowSizer->Add(m_pFilterTextComboBox, 0, wxALL, 5);
    
    m_pAutoUpdateMemoryCheckBox = new wxCheckBox(m_pMemoryPanel, wxID_ANY, wxT("自动更新"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pAutoUpdateMemoryCheckBox->SetValue(1);
    pFirstRowSizer->Add(m_pAutoUpdateMemoryCheckBox, 0, wxALL, 5);

    pMemorySizer->Add( pFirstRowSizer, 0, wxALL, 5 );

    m_pMemoryDataGrid = new wxGrid( m_pMemoryPanel, wxID_ANY, wxDefaultPosition, wxSize(400, 300), 0 );
    // Grid
    m_pMemoryDataGrid->CreateGrid( 5, 5 );
    m_pMemoryDataGrid->EnableEditing( false );
    m_pMemoryDataGrid->EnableGridLines( true );
    m_pMemoryDataGrid->EnableDragGridSize( false );
    m_pMemoryDataGrid->SetMargins( 0, 0 );

    // Columns
    m_pMemoryDataGrid->EnableDragColMove( false );
    m_pMemoryDataGrid->EnableDragColSize( true );
    m_pMemoryDataGrid->SetColLabelSize( 30 );
    m_pMemoryDataGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    // Rows
    m_pMemoryDataGrid->EnableDragRowSize( true );
    m_pMemoryDataGrid->SetRowLabelSize( 80 );
    m_pMemoryDataGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

    ClearAllColumnLabel();

    // Label Appearance
    // Cell Defaults
    m_pMemoryDataGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
    pMemorySizer->Add( m_pMemoryDataGrid, 0, wxEXPAND | wxALL, 5 );
    //NOTICE: to set scroll rate exactly same with the row height.
    //If didn't do so, we can't get the exactly scroll pos, the value will be wrong for wxwidgets reason.
    int scrollRateX, scrollRateY;
    m_pMemoryDataGrid->GetScrollPixelsPerUnit(&scrollRateX, &scrollRateY);
    m_pMemoryDataGrid->SetScrollRate(scrollRateX, m_pMemoryDataGrid->GetDefaultRowSize());

    wxStaticBoxSizer* pMemoryInfoSizer = new wxStaticBoxSizer( new wxStaticBox( m_pMemoryPanel, wxID_ANY, wxT("内存信息") ), wxVERTICAL);

    wxBoxSizer* p1stRowSizer = new wxBoxSizer(wxHORIZONTAL);
    m_pTotalAllocSizeLabel = new wxStaticText( m_pMemoryPanel, wxID_ANY, wxT("总共申请内存:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_pTotalAllocSizeLabel->Wrap( -1 );
    p1stRowSizer->Add(m_pTotalAllocSizeLabel, 1, wxBottom, 5);

    m_pAllocCountLabel = new wxStaticText(m_pMemoryPanel, wxID_ANY, wxT("内存块数量:"), wxDefaultPosition, wxDefaultSize, 0);
    m_pAllocCountLabel->Wrap( -1 );
    p1stRowSizer->Add(m_pAllocCountLabel, 1, wxBottom, 5);

    m_pMaxAllocSizeLabel = new wxStaticText(m_pMemoryPanel, wxID_ANY, wxT("最大内存块:"), wxDefaultPosition, wxDefaultSize, 0);
    m_pMaxAllocSizeLabel->Wrap( -1 );
    p1stRowSizer->Add(m_pMaxAllocSizeLabel, 1, wxBottom, 5);

    wxStaticText* pStackOffsetLabel = new wxStaticText(m_pMemoryPanel, wxID_ANY, wxT("栈信息偏移:"), wxDefaultPosition, wxDefaultSize, 0);
    pStackOffsetLabel->Wrap( -1 );
    p1stRowSizer->Add(pStackOffsetLabel, 0, wxBottom, 5);
    m_pStackOffsetTextCtrl = new wxTextCtrl(m_pMemoryPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(18, 18));
    m_pStackOffsetTextCtrl->SetEditable(false);
    p1stRowSizer->Add(m_pStackOffsetTextCtrl, 0, wxTop, 5);
    m_pStackOffsetSpinBtn = new wxSpinButton(m_pMemoryPanel, wxID_ANY, wxDefaultPosition, wxSize(10, 18));
    p1stRowSizer->Add(m_pStackOffsetSpinBtn, 0, wxTop, 5);

    p1stRowSizer->Add(0, 0, 1, wxEXPAND, 5);
    pMemoryInfoSizer->Add(p1stRowSizer, 1, wxEXPAND, 5);

    wxStaticBoxSizer* pMemoryCallStackSizer = new wxStaticBoxSizer( new wxStaticBox( m_pMemoryPanel, wxID_ANY, wxT("栈信息") ), wxVERTICAL);
    m_pCallStackListBox = new wxListBox( m_pMemoryPanel, wxID_ANY, wxDefaultPosition, wxSize(350, 400), 0, NULL, 0 );
    pMemoryCallStackSizer->Add(m_pCallStackListBox, 1, wxEXPAND, 5);

    pMemoryInfoSizer->Add(pMemoryCallStackSizer, 1, wxEXPAND, 5);

    pMemorySizer->Add( pMemoryInfoSizer, 0, wxEXPAND, 5 );

    m_pMemoryPanel->SetSizerAndFit( pMemorySizer );
    m_pMemoryPanel->Layout();
    OnViewMemoryByLocation();
    m_pNoteBook->AddPage( m_pMemoryPanel, wxT("内存统计"), false );

    m_pMemoryViewChoice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnMemoryChoiceChanged ), NULL, this );
    m_pFilterTextComboBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CBDTWxFrame::OnFilterTextEnter ), NULL, this );
    m_pFilterTextComboBox->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnFilterTextSelected ), NULL, this );
    m_pMemoryDataGrid->Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( CBDTWxFrame::OnGridCellSelected ), NULL, this );
    m_pStackOffsetSpinBtn->Connect(wxEVT_SPIN, wxSpinEventHandler( CBDTWxFrame::OnSetMemoryStackOffset ), NULL, this);
    m_pCallStackListBox->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(CBDTWxFrame::OnCallStackListDoubleClicked), NULL, this);
}

void CBDTWxFrame::DestroyMemoryPage()
{
    m_pMemoryViewChoice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnMemoryChoiceChanged ), NULL, this );
    m_pFilterTextComboBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CBDTWxFrame::OnFilterTextEnter ), NULL, this );
    m_pFilterTextComboBox->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( CBDTWxFrame::OnFilterTextSelected ), NULL, this );
    m_pMemoryDataGrid->Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( CBDTWxFrame::OnGridCellSelected ), NULL, this );
    m_pStackOffsetSpinBtn->Disconnect(wxEVT_SPIN, wxSpinEventHandler( CBDTWxFrame::OnSetMemoryStackOffset ), NULL, this);
    m_pCallStackListBox->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(CBDTWxFrame::OnCallStackListDoubleClicked), NULL, this);
}

void CBDTWxFrame::OnMemoryChoiceChanged( wxCommandEvent& event )
{
    if ((int)m_memoryViewType != event.GetSelection())
    {    
        //TODO: this method is very very very slow when we got lots of data. try to replace it.
        m_pMemoryDataGrid->AutoSizeColumns();

        m_memoryViewType = event.GetSelection();
        m_bMemoryUINeedUpdate = true;
        m_pMemoryDataGrid->ClearGrid();
        ClearAllColumnLabel();
        m_pMemoryDataGrid->Scroll(0,0);
        m_pCallStackListBox->Clear();
        switch (m_memoryViewType)
        {
        case eMVT_Addr:
            OnViewMemoryByAddr();
            break;
        case eMVT_Location:
            OnViewMemoryByLocation();
            break;
        case eMVT_Size:
            OnViewMemoryBySize();
            break;
        case eMVT_AllocTime:
            OnViewMemoryByAllocTime();
            break;
        default:
            BEATS_ASSERT(false, _T("Never reach here!"));
            break;
        }
    }
}

void CBDTWxFrame::AdjustMemoryGridColumnCount( size_t count )
{
    int delta = m_pMemoryDataGrid->GetCols() - count;
    if (delta > 0)
    {
        m_pMemoryDataGrid->DeleteCols(0, delta);
    }
    else if (delta < 0)
    {
        m_pMemoryDataGrid->AppendCols(-delta);
    }
}

void CBDTWxFrame::OnViewMemoryByAddr()
{ 
    AdjustMemoryGridColumnCount(5);
    m_pMemoryDataGrid->SetColLabelValue(0, _T("内存地址"));
    m_pMemoryDataGrid->SetColumnWidth(0, 58);
    m_pMemoryDataGrid->SetColLabelValue(1, _T("尺寸"));
    m_pMemoryDataGrid->SetColumnWidth(1, 62);
    m_pMemoryDataGrid->SetColLabelValue(2, _T("用途"));
    m_pMemoryDataGrid->SetColumnWidth(2, 50);
    m_pMemoryDataGrid->SetColLabelValue(3, _T("分配时间"));
    m_pMemoryDataGrid->SetColumnWidth(3, 62);
    m_pMemoryDataGrid->SetColLabelValue(4, _T("分配位置"));
    m_pMemoryDataGrid->SetColumnWidth(4, 478);
}

void CBDTWxFrame::OnViewMemoryByLocation()
{
    AdjustMemoryGridColumnCount(4);
    m_pMemoryDataGrid->SetColLabelValue(0, _T("位置ID"));
    m_pMemoryDataGrid->SetColumnWidth(0, 61);
    m_pMemoryDataGrid->SetColLabelValue(1, _T("尺寸"));
    m_pMemoryDataGrid->SetColumnWidth(1, 41);
    m_pMemoryDataGrid->SetColLabelValue(2, _T("分配次数"));
    m_pMemoryDataGrid->SetColumnWidth(2, 58);
    m_pMemoryDataGrid->SetColLabelValue(3, _T("分配位置"));    
    m_pMemoryDataGrid->SetColumnWidth(3, 478);
    m_pCallStackListBox->AppendAndEnsureVisible(_T("当前查看方式不支持堆栈打印，请切换查看方式"));
}

void CBDTWxFrame::OnViewMemoryBySize()
{
    AdjustMemoryGridColumnCount(4);
    m_pMemoryDataGrid->SetColLabelValue(0, _T("尺寸"));
    m_pMemoryDataGrid->SetColumnWidth(0, 58);
    m_pMemoryDataGrid->SetColLabelValue(1, _T("内存地址"));
    m_pMemoryDataGrid->SetColumnWidth(1, 58);
    m_pMemoryDataGrid->SetColLabelValue(2, _T("位置ID"));
    m_pMemoryDataGrid->SetColumnWidth(2, 62);
    m_pMemoryDataGrid->SetColLabelValue(3, _T("分配位置"));
    m_pMemoryDataGrid->SetColumnWidth(3, 478);
}

void CBDTWxFrame::OnViewMemoryByAllocTime()
{
    AdjustMemoryGridColumnCount(5);
    m_pMemoryDataGrid->SetColLabelValue(0, _T("分配时间"));
    m_pMemoryDataGrid->SetColumnWidth(0, 58);
    m_pMemoryDataGrid->SetColLabelValue(1, _T("内存地址"));
    m_pMemoryDataGrid->SetColumnWidth(1, 62);
    m_pMemoryDataGrid->SetColLabelValue(2, _T("尺寸"));
    m_pMemoryDataGrid->SetColumnWidth(2, 58);
    m_pMemoryDataGrid->SetColLabelValue(3, _T("位置ID"));
    m_pMemoryDataGrid->SetColumnWidth(3, 62);
    m_pMemoryDataGrid->SetColLabelValue(4, _T("分配位置"));
    m_pMemoryDataGrid->SetColumnWidth(4, 478);
}

void CBDTWxFrame::UpdateMemoryData()
{
    if (m_pMemoryPanel->IsShown())
    {
        RefreshMemoryGrid(m_memoryViewType);
        m_pTotalAllocSizeLabel->SetLabel(wxString::Format("总共申请内存: %dbyte", CMemoryDetector::GetInstance()->GetAllocMemorySize()));
        size_t count = CMemoryDetector::GetInstance()->GetAllocMemoryCount();
        m_pAllocCountLabel->SetLabel(wxString::Format("内存块数量: %d", count));
        if (count > 0)
        {
            size_t maxSize = CMemoryDetector::GetInstance()->GetMemoryBlockMaxSize();
            m_pMaxAllocSizeLabel->SetLabel(wxString::Format("最大内存块: %dbyte", maxSize));
        }
        else
        {
            m_pMaxAllocSizeLabel->SetLabel(_T(""));
        }
    }
}

void CBDTWxFrame::ClearAllColumnLabel()
{
    size_t colSize = m_pMemoryDataGrid->GetCols();
    for (size_t i = 0; i < colSize; ++i)
    {
        m_pMemoryDataGrid->SetColLabelValue(i, "");
    }
}

void CBDTWxFrame::RefreshMemoryGrid(size_t type)
{
    m_bMemoryUINeedUpdate = false;
    m_pMemoryDataGrid->ClearGrid();
    switch (type)
    {
    case eMVT_Addr:
        RefreshViewMemoryByAddr();
        break;
    case eMVT_Location:
        RefreshViewMemoryByLocation();
        break;
    case eMVT_Size:
        RefreshViewMemoryBySize();
        break;
    case eMVT_AllocTime:
        RefreshViewMemoryByTime();
        break;
    default:
        break;
    }
}

void CBDTWxFrame::RefreshViewMemoryByAddr()
{
    const SMemoryFrameRecord& recordCache = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetMemoryUICache();
    if(recordCache.type == eMFRT_Address || recordCache.type == eMFRT_All)
    {
        std::map<void*, SMemoryRecord*>::const_iterator iter = recordCache.recordMap.begin();
        std::map<void*, SMemoryRecord*>::const_iterator endIter = recordCache.recordMap.end();
        SetGridRowsCount(m_pMemoryDataGrid, recordCache.totalItemCount);
        int rowIndex = recordCache.startPos;
        m_memoryRecordCacheStartPos = recordCache.startPos;
        for (; iter != endIter; ++iter)
        {
            size_t allocId = iter->second->m_allocId;
            size_t stackOffset = m_pStackOffsetSpinBtn->GetValue();
            if (stackOffset > 0)
            {
                std::vector<size_t> callStack;
                CMemoryDetector::GetInstance()->GetCallStack((size_t)iter->first, callStack);
                if (callStack.size() > stackOffset)
                {
                    allocId = callStack[stackOffset];
                }
                else
                {
                    allocId = callStack.back();
                }
            }
            IMAGEHLP_LINE info;
            bool getAddrSuccess = false;
            bool shouldFilter = FilterTest(allocId, info, getAddrSuccess);
            if (!shouldFilter)
            {
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", iter->first),rowIndex, 0);
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->second->m_size),rowIndex, 1);
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->second->m_usage),rowIndex, 2);
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->second->m_allocTime),rowIndex, 3);
                wxString infoStr = getAddrSuccess ? wxString::Format(_T("%s %d行"), wxString(info.FileName), info.LineNumber) : wxString::Format(_T("未知位置"));
                bool bValueUpdated = m_pMemoryDataGrid->GetCellValue(rowIndex, 4).CompareTo(infoStr) != 0;
                if (bValueUpdated)
                {
                    m_pMemoryDataGrid->SetCellValue(infoStr, rowIndex, 4);
                }
                ++rowIndex;
            }
        }
    }
    else
    {
        m_pMemoryDataGrid->ClearGrid();
    }
}

void CBDTWxFrame::RefreshViewMemoryByLocation()
{
    const SMemoryFrameRecord& recordCache = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetMemoryUICache();
    if(recordCache.type == eMFRT_Location || recordCache.type == eMFRT_All)
    {
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator iter = recordCache.recordMapForLocation.begin();
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator endIter = recordCache.recordMapForLocation.end();

        SetGridRowsCount(m_pMemoryDataGrid, recordCache.recordMapForLocation.size());
        int rowIndex = recordCache.startPos;
        m_memoryRecordCacheStartPos = recordCache.startPos;
        for (; iter != endIter; ++iter)
        {
            IMAGEHLP_LINE info;
            bool getAddrSuccess = false;
            bool shouldFilter = FilterTest(iter->first, info, getAddrSuccess);
            if (!shouldFilter)
            {
                std::set<SMemoryRecord*>::const_iterator iterSet = iter->second.begin();
                size_t totalSize = 0;
                for (;  iterSet != iter->second.end(); ++iterSet)
                {
                    totalSize += (*iterSet)->m_size;
                }
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", (size_t*)iter->first),rowIndex, 0);
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", totalSize),rowIndex, 1);
                m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->second.size()),rowIndex, 2);
                wxString infoStr = getAddrSuccess ? wxString::Format(_T("%s %d行"), wxString(info.FileName), info.LineNumber) : wxString::Format(_T("未知位置"));
                bool bValueUpdated = m_pMemoryDataGrid->GetCellValue(rowIndex, 3).CompareTo(infoStr) != 0;
                if (bValueUpdated)
                {
                    m_pMemoryDataGrid->SetCellValue(infoStr, rowIndex, 3);
                }
                ++rowIndex;
            }
        }
    }
    else
    {
        m_pMemoryDataGrid->ClearGrid();
    }
}

void CBDTWxFrame::RefreshViewMemoryBySize()
{
    const SMemoryFrameRecord& recordCache = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetMemoryUICache();
    if(recordCache.type == eMFRT_Size || recordCache.type == eMFRT_All)
    {
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator iter = recordCache.recordMapForSize.begin();
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator endIter = recordCache.recordMapForSize.end();
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator iterForCount = recordCache.recordMapForSize.begin();
        SetGridRowsCount(m_pMemoryDataGrid, recordCache.totalItemCount);
        int rowIndex = recordCache.startPos;
        m_memoryRecordCacheStartPos = recordCache.startPos;
        for (; iter != endIter; ++iter)
        {
            bool firstRow = true;
            m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->first),rowIndex, 0);
            std::set<SMemoryRecord*>::const_iterator iterSet = iter->second.begin();
            size_t rowCountOnShow = 0;
            for (;  iterSet != iter->second.end(); ++iterSet)
            {
                size_t allocId = (*iterSet)->m_allocId;
                size_t stackOffset = m_pStackOffsetSpinBtn->GetValue();
                if (stackOffset > 0)
                {
                    std::vector<size_t> callStack;
                    CMemoryDetector::GetInstance()->GetCallStack((size_t)(*iterSet)->m_pAddress, callStack);
                    if (callStack.size() > stackOffset)
                    {
                        allocId = callStack[stackOffset];
                    }
                    else
                    {
                        allocId = callStack.back();
                    }
                }

                IMAGEHLP_LINE info;
                bool getAddrSuccess = false;
                bool shouldFilter = FilterTest(allocId, info, getAddrSuccess);
                if (!shouldFilter)
                {
                    if (!firstRow)
                    {
                        m_pMemoryDataGrid->SetCellValue("", rowIndex, 0);
                    }
                    firstRow = false;
                    m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", (size_t*)((*iterSet)->m_pAddress)),rowIndex, 1);
                    m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", (size_t*)(allocId)),rowIndex, 2);
                    wxString infoStr = getAddrSuccess ? wxString::Format(_T("%s %d行"), wxString(info.FileName), info.LineNumber) : wxString::Format(_T("未知位置"));
                    bool bValueUpdated = m_pMemoryDataGrid->GetCellValue(rowIndex, 3).CompareTo(infoStr) != 0;
                    if (bValueUpdated)
                    {
                        m_pMemoryDataGrid->SetCellValue(infoStr, rowIndex, 3);
                    }
                    ++rowCountOnShow;
                    ++rowIndex;
                }
            }
            // if all the rows under the size title are filtered. clear the head.
            if (rowCountOnShow == 0)
            {
                m_pMemoryDataGrid->SetCellValue(wxT(""), rowIndex, 0);
            }
        }
    }
    else
    {
        m_pMemoryDataGrid->ClearGrid();
    }
}

void CBDTWxFrame::RefreshViewMemoryByTime()
{    
    const SMemoryFrameRecord& recordCache = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetMemoryUICache();
    if(recordCache.type == eMFRT_Time || recordCache.type == eMFRT_All)
    {

        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator iter = recordCache.recordMapForTime.begin();
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator endIter = recordCache.recordMapForTime.end();
        std::map<size_t, std::set<SMemoryRecord*>>::const_iterator iterForCount = recordCache.recordMapForTime.begin();
        SetGridRowsCount(m_pMemoryDataGrid, recordCache.totalItemCount);
        int rowIndex = recordCache.startPos;
        m_memoryRecordCacheStartPos = recordCache.startPos;
        for (; iter != endIter; ++iter)
        {
            bool firstRow = true;
            m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", iter->first),rowIndex, 0);
            std::set<SMemoryRecord*>::const_iterator iterSet = iter->second.begin();
            size_t rowCountOnShow = 0;
            for (;  iterSet != iter->second.end(); ++iterSet)
            {
                size_t allocId = (*iterSet)->m_allocId;
                size_t stackOffset = m_pStackOffsetSpinBtn->GetValue();
                if (stackOffset > 0)
                {
                    std::vector<size_t> callStack;
                    CMemoryDetector::GetInstance()->GetCallStack((size_t)(*iterSet)->m_pAddress, callStack);
                    if (callStack.size() > stackOffset)
                    {
                        allocId = callStack[stackOffset];
                    }
                    else
                    {
                        allocId = callStack.back();
                    }
                }
                IMAGEHLP_LINE info;
                info.SizeOfStruct = sizeof(IMAGEHLP_LINE);
                bool getAddrSuccess = false;
                bool shouldFilter = FilterTest(allocId, info, getAddrSuccess);
                if (!shouldFilter)
                {
                    if (!firstRow)
                    {
                        m_pMemoryDataGrid->SetCellValue("", rowIndex, 0);
                    }
                    firstRow = false;
                    m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", (size_t*)((*iterSet)->m_pAddress)),rowIndex, 1);
                    m_pMemoryDataGrid->SetCellValue(wxString::Format("%d", ((*iterSet)->m_size)),rowIndex, 2);
                    m_pMemoryDataGrid->SetCellValue(wxString::Format("%p", (size_t*)(allocId)),rowIndex, 3);
                    //TODO: Try to catch a mystery crash.
                    bool bLineNumberMayOverFlow = getAddrSuccess && info.LineNumber > 100000;
                    bool bFileNameMayBroken = getAddrSuccess && (size_t)info.FileName < 10;
                    if (bLineNumberMayOverFlow || bFileNameMayBroken)
                    {
                        BEATS_ASSERT(false);
                    }

                    wxString infoStr = getAddrSuccess ? wxString::Format(_T("%s %d行"), wxString(info.FileName), info.LineNumber) : wxString::Format(_T("未知位置"));
                    //TODO: Try to catch a mystery crash.
                    bLineNumberMayOverFlow = getAddrSuccess && info.LineNumber > 100000;
                    bFileNameMayBroken = getAddrSuccess && (size_t)info.FileName < 10;
                    if (bLineNumberMayOverFlow || bFileNameMayBroken)
                    {
                        BEATS_ASSERT(false);
                    }

                    bool bValueUpdated = m_pMemoryDataGrid->GetCellValue(rowIndex, 4).CompareTo(infoStr) != 0;
                    if (bValueUpdated)
                    {
                        m_pMemoryDataGrid->SetCellValue(infoStr, rowIndex, 4);
                    }
                    ++rowCountOnShow;
                    ++rowIndex;
                }
            }
            // if all the rows under the size title are filtered. clear the head.
            if (rowCountOnShow == 0)
            {
                m_pMemoryDataGrid->SetCellValue(wxT(""), rowIndex, 0);
            }
        }
    }
    else
    {
        m_pMemoryDataGrid->ClearGrid();
    }
}

void CBDTWxFrame::OnFilterTextEnter( wxCommandEvent& /*event*/ )
{
    wxString currentText = m_pFilterTextComboBox->GetValue();
    bool isTextEmpty = m_pFilterTextComboBox->IsTextEmpty();
    if (isTextEmpty)
    {
        if (m_lastSelectedFilterPos != -1)
        {
            char msgCache[512];
            sprintf_s(msgCache, "确定要删除 %s 这一项吗?", m_pFilterTextComboBox->GetString(m_lastSelectedFilterPos).c_str().AsChar());
            if (IDYES == MessageBoxA(this->GetHWND(), msgCache, "删除", MB_YESNO))
            {
                CMemoryDetector::GetInstance()->DelFilterStr(m_pFilterTextComboBox->GetString(m_lastSelectedFilterPos).c_str().AsChar());
                m_pFilterTextComboBox->Delete(m_lastSelectedFilterPos);
                m_pMemoryDataGrid->Scroll(0, 0);
            }
            m_lastSelectedFilterPos = -1;
        }
    }
    else
    {
        char msgCache[512];
        sprintf_s(msgCache, "确定要添加 %s 这一项吗?", currentText.c_str().AsChar());
        if (IDYES == MessageBoxA(this->GetHWND(), msgCache, "添加", MB_YESNO))
        {
            if ( m_pFilterTextComboBox->FindString(currentText, false) == -1)
            {
                CMemoryDetector::GetInstance()->AddFilterStr(currentText.c_str().AsChar());
                m_pFilterTextComboBox->Append(currentText);
                m_pMemoryDataGrid->Scroll(0, 0);
            }
        }
    }

    m_pFilterTextComboBox->SetValue("");
    bool isPopupShown = TRUE == SendMessage(m_pFilterTextComboBox->GetHWND(), CB_GETDROPPEDSTATE, 0, 0);
    if (!isPopupShown)
    {
        m_pFilterTextComboBox->Popup();
    }
}

void CBDTWxFrame::OnFilterTextSelected( wxCommandEvent& event )
{
    m_lastSelectedFilterPos = event.GetSelection(); 
}

bool CBDTWxFrame::FilterTest(size_t eip, IMAGEHLP_LINE& info, bool& getAddrSuccess)
{
    DWORD displacement = 0;
    getAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), eip, &displacement, &info) == TRUE;
    //TODO: Try to catch a mystery crash.
    bool bLineNumberMayOverFlow = getAddrSuccess && info.LineNumber > 100000;
    bool bFileNameMayBroken = getAddrSuccess && (size_t)info.FileName < 10;
    if (bLineNumberMayOverFlow || bFileNameMayBroken)
    {
        BEATS_ASSERT(false);
    }
    bool filter = false;
    if (getAddrSuccess)
    {
        for (size_t i = 0; i < m_pFilterTextComboBox->GetCount(); ++i)
        {
            if (strstr(info.FileName, m_pFilterTextComboBox->GetString(i).c_str().AsChar()) != 0)
            {
                filter = true;
                break;
            }
        }
    }
    return filter;
}

void CBDTWxFrame::OnGridCellSelected( wxGridEvent& event )
{
    if ( m_memoryViewType == eMVT_Addr || m_memoryViewType == eMVT_Size || m_memoryViewType == eMVT_AllocTime)
    {
        m_pCallStackListBox->Clear();
        long value = 0;
        size_t addressValueColIndex = m_memoryViewType == eMVT_Addr ? 0 : 1;//it's hard code here.
        wxString cellStr = m_pMemoryDataGrid->GetCellValue(event.GetRow(), addressValueColIndex);
        if (cellStr.IsEmpty() == false)
        {
            cellStr.ToLong(&value, 16);
            std::vector<size_t> callStack;
            CMemoryDetector::GetInstance()->GetCallStack(value, callStack);
            DWORD displacement = 0;
            IMAGEHLP_LINE info;
            char temp[256];
            for (size_t i = 0; i < callStack.size(); ++i)
            {
                bool getAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), callStack[i], &displacement, &info) == TRUE;
                if (getAddrSuccess)
                {
                    sprintf_s(temp, "%s line:%d", info.FileName, info.LineNumber);
                    m_pCallStackListBox->Append(temp);
                }
                else
                {
                    m_pCallStackListBox->Append("未知！");
                }
            }
        }
    }
}

typedef struct
{
    DWORD ExitStatus;
    DWORD PebBaseAddress;
    DWORD AffinityMask;
    DWORD BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId; 
} PROCESS_BASIC_INFORMATION;

void CBDTWxFrame::OnCallStackListDoubleClicked(wxCommandEvent& event)
{
    wxString selection = event.GetString();
    TCHAR selectionChar[MAX_PATH];
    const wxChar* myStringChars = selection.c_str();  
    for (size_t i = 0; i < selection.Len(); i++) 
    {
        selectionChar[i] = myStringChars [i];
    }
    selectionChar[selection.Len()] = _T('\0');
    // Remove the "line:xxx" suffix from selection.
    TCHAR* pLastSpacePos = _tcsstr(selectionChar, _T(" "));
    TCHAR* pCurPos = pLastSpacePos;
    while (pCurPos != NULL)
    {
        pLastSpacePos = pCurPos;
        pCurPos = _tcsstr(pLastSpacePos + 1, _T(" "));
    }
    *pLastSpacePos = 0;
    FILE* pFile = _tfopen(selectionChar, _T("rb"));
    if (pFile != NULL)
    {
        fclose(pFile);
        bool bOpenFileInExistingIDE = false;
        if (IsDebuggerPresent() != FALSE)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());
            if (hProcess != NULL)
            {
                typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
                PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle(_T("ntdll")), "NtQueryInformationProcess");
                if (NtQueryInformationProcess != NULL)
                {
                    PROCESS_BASIC_INFORMATION pbi;
                    long status = NtQueryInformationProcess(hProcess, 0, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
                    if (status == 0)
                    {
                        HANDLE hDebuggerProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, pbi.InheritedFromUniqueProcessId);
                        if (hDebuggerProcess != NULL)
                        {
                            TCHAR processPath[MAX_PATH] = {0};
                            GetModuleFileNameEx(hDebuggerProcess, NULL, processPath, MAX_PATH);
                            fclose(pFile);
                            TCHAR parameter[MAX_PATH];
                            _stprintf_s(parameter, _T("/Edit \"%s\""), selectionChar);
                            ShellExecute(NULL, _T("open"), processPath, parameter, NULL, SW_SHOW);
                            CloseHandle(hDebuggerProcess);
                            bOpenFileInExistingIDE = true;
                        }
                    }
                }
            }
            CloseHandle(hProcess);
        }
        if (!bOpenFileInExistingIDE)
        {
            ShellExecute(NULL, _T("open"), selectionChar, NULL, NULL, SW_SHOW);
        }
    }
}

void CBDTWxFrame::OnSetMemoryStackOffset( wxSpinEvent& event )
{
    m_pStackOffsetTextCtrl->SetValue(wxString::Format(_T("%d"), event.GetValue()));
    m_bMemoryUINeedUpdate = true; // Force Update it.
}

int CBDTWxFrame::GetMemoryGridViewStartPos()
{
    int x, y;
    m_pMemoryDataGrid->GetViewStart(&x, &y);
    y -= MemoryGridStartPosOffset;
    if (y < MemoryGridStartPosOffset * 0.5)
    {
        y = 0;
    }
    return y;
}

int CBDTWxFrame::GetMemoryGridViewStartOffset()
{
    return MemoryGridStartPosOffset;
}

int CBDTWxFrame::GetMemoryGridItemCountLimit()
{
    return MemoryGridStartPosOffset * 3;
}

bool CBDTWxFrame::IsMemoryGridNeedUpdateByScroll()
{
    return (abs((int)m_memoryRecordCacheStartPos - GetMemoryGridViewStartPos()) > (MemoryGridStartPosOffset * 0.5f));
}
