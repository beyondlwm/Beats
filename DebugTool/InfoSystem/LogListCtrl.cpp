#include "stdafx.h"
#include "LogListCtrl.h"
#include <DbgHelp.h>

CLogListCtrl::CLogListCtrl( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxListCtrl(parent, id, pos, size, style)
{
    InitializeCriticalSection(&m_logUpdateSection);
}

CLogListCtrl::~CLogListCtrl()
{
    BEATS_SAFE_DELETE_VECTOR(m_pLogs);
    DeleteCriticalSection(&m_logUpdateSection);
    for (size_t i = 0; i < m_conditionFilters.size(); ++i)
    {
        for (size_t j = 0; j < m_conditionFilters[i].size(); ++j)
        {
            BEATS_SAFE_DELETE(m_conditionFilters[i][j]);
        }
    }
    m_conditionFilters.clear();
}

void CLogListCtrl::AddLog( const TCHAR* pLog, size_t logPos, const wxTextAttr* pTextAttr )
{
    EnterCriticalSection(&m_logUpdateSection);
    SInfoData* pInfoData = new SInfoData;
    pInfoData->m_log.assign(pLog);
    pInfoData->m_color = pTextAttr->GetTextColour().GetRGBA();
    pInfoData->m_time = GetTickCount();
    pInfoData->m_pos = logPos;
    m_pLogs.push_back(pInfoData);
    if (m_addressMap.find(logPos) == m_addressMap.end())
    {
        DWORD displacement = 0;
        IMAGEHLP_LINE info;
        bool bGetAddrSuccess = SymGetLineFromAddr(GetCurrentProcess(), logPos, &displacement, &info) == TRUE;
        wxString posStr;
        if (bGetAddrSuccess)
        {
            posStr= wxString::Format(_T("%s Line:%d Id:%d"), wxString(info.FileName), info.LineNumber, logPos);
        }
        else
        {
            posStr = _T("Unkown Pos");
        }
        m_addressMap[logPos] = posStr;
    }
    int nVisibleLogCount = -1;
    if(ShouldShowLog(*pInfoData))
    {
        m_visibleLogId.push_back(m_pLogs.size() - 1);
        nVisibleLogCount = m_visibleLogId.size();
    }
    LeaveCriticalSection(&m_logUpdateSection);
    // Notice: make sure call SetItemCount() after LeaveCriticalSection(). because SetItemCount invoke SendMessage which keep waiting for the message procession.
    // So we must call LeaveCriticalSection to let the message procession keep running.
    if (nVisibleLogCount > -1)
    {
        SetItemCount(nVisibleLogCount);
    }
}

void CLogListCtrl::ClearAllLog()
{
    EnterCriticalSection(&m_logUpdateSection);
    BEATS_SAFE_DELETE_VECTOR(m_pLogs);
    m_visibleLogId.clear();
    DeleteAllItems();
    LeaveCriticalSection(&m_logUpdateSection);
}

void CLogListCtrl::AddFilterCondition(std::vector<SFilterCondition*>& condition)
{
    m_conditionFilters.push_back(condition);
}

wxString CLogListCtrl::OnGetItemText( long item, long column ) const
{
    EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_logUpdateSection));
    wxString pStrData = wxEmptyString;
    // To avoid thread sync problem, such as UI needs to get #11 item to display in wxwidgets thread while we have delete it in main thread.
    if (m_visibleLogId.size() > (size_t)item && m_pLogs.size() > m_visibleLogId[item])
    {
        size_t uVisibleId = m_visibleLogId[item];
        SInfoData* pLogData = m_pLogs[uVisibleId];
        if (column == 0)
        {
            pStrData = pLogData->m_log;
        }
        else if (column == 1)
        {
            pStrData = wxString::Format(_T("%d"), pLogData->m_time);
        }
        else if (column == 2)
        {
            bool bSamePosAsAbove = false;
            size_t pos = pLogData->m_pos;
            if (item > 0)
            {
                size_t lastpos = m_pLogs[m_visibleLogId[item - 1]]->m_pos;
                if (lastpos == pos)
                {
                    bSamePosAsAbove = true;
                    pStrData = wxString(_T("¡ü"));
                }
            }
            if (!bSamePosAsAbove)
            {
                std::map<size_t, wxString>::const_iterator iter = m_addressMap.find(pos);
                if (iter != m_addressMap.end())
                {
                    pStrData = iter->second;
                }
            }
        }
    }
    LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_logUpdateSection));
    return pStrData;
}

wxListItemAttr * CLogListCtrl::OnGetItemAttr( long item ) const
{
    EnterCriticalSection(const_cast<CRITICAL_SECTION*>(&m_logUpdateSection));
    static wxListItemAttr tmpCacheTextAttr;
    if ((size_t)item < m_visibleLogId.size() && m_visibleLogId[item] < m_pLogs.size())
    {
        tmpCacheTextAttr.SetTextColour(wxColour(m_pLogs[m_visibleLogId[item]]->m_color));
    }    
    LeaveCriticalSection(const_cast<CRITICAL_SECTION*>(&m_logUpdateSection));
    return &tmpCacheTextAttr;
}

bool CLogListCtrl::ShouldShowLog( const SInfoData& data)
{
    bool bPassCondition = true;
    for (size_t i = 0; i < m_conditionFilters.size(); ++i)
    {
        for (size_t j = 0; j < m_conditionFilters[i].size(); ++j)
        {
            bPassCondition = true;
            SFilterCondition* pCondition = m_conditionFilters[i][j];
            if ((pCondition->GetType() & eLFT_Str) > 0 && pCondition->m_pStrFilter->length() > 0)
            {
                if (data.m_log.Find(*pCondition->m_pStrFilter) == -1)
                {
                    bPassCondition = false;
                }
            }
            if ((pCondition->GetType() & eLFT_Color) > 0)
            {
                if (data.m_color != pCondition->m_colorFilter)
                {
                    bPassCondition = false;
                }
            }
            if ((pCondition->GetType() & eLFT_Time) > 0)
            {                
                if (wxString::Format(_T("%d"), data.m_time).Find( wxString::Format(_T("%d"), pCondition->m_timeFilter)) == -1)
                {
                    bPassCondition = false;
                }
            }
            if ((pCondition->GetType() & eLFT_Pos) > 0)
            {
                if (wxString::Format(_T("%d"), data.m_pos).Find( wxString::Format(_T("%d"), pCondition->m_posFilter)) == -1)
                {
                    bPassCondition = false;
                }
            }
            bPassCondition = pCondition->m_bReverse ? !bPassCondition : bPassCondition;
            if (bPassCondition)
            {
                break;
            }
        }
        if (!bPassCondition)
        {
            break;
        }
    }
    return bPassCondition;
}

void CLogListCtrl::UpdateList()
{
    EnterCriticalSection(&m_logUpdateSection);
    m_visibleLogId.clear();
    for (size_t i = 0; i < m_pLogs.size(); ++i)
    {
        bool bVisible = ShouldShowLog(*m_pLogs[i]);
        if (bVisible)
        {
            m_visibleLogId.push_back(i);
        }
    }
    SetItemCount(m_visibleLogId.size());
    Refresh();
    LeaveCriticalSection(&m_logUpdateSection);
}

void CLogListCtrl::DeleteAllFilterConditions()
{
    for (size_t i = 0; i < m_conditionFilters.size(); ++i)
    {
        for (size_t j = 0; j < m_conditionFilters[i].size(); ++j)
        {
            BEATS_SAFE_DELETE(m_conditionFilters[i][j]);
        }
    }
    m_conditionFilters.clear();
}
