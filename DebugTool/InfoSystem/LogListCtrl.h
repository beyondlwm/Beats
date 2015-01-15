#ifndef BEATS_DEBUGTOOL_INFOSYSTEM_LOGLISTCTRL_H__INCLUDE
#define BEATS_DEBUGTOOL_INFOSYSTEM_LOGLISTCTRL_H__INCLUDE

#include <wx/listctrl.h>

struct SInfoData
{
    uint32_t m_color;
    wxString m_log;
    uint32_t m_time;
    uint32_t m_pos;
};

enum ELogFilterType
{
    eLFT_None = 0,
    eLFT_Str = 1 << 0,
    eLFT_Color = 1 << 1,
    eLFT_Time = 1 << 2,
    eLFT_Pos = 1 << 3,

    eLFT_Count = 4,
    eLFT_Force32Bit = 0xFFFFFFFF
};
struct SFilterCondition
{
    bool m_bReverse;
    union
    {
        wxString* m_pStrFilter;
        uint32_t    m_colorFilter;
        uint32_t m_timeFilter;
        uint32_t m_posFilter;
    };
    SFilterCondition()
        : m_type(eLFT_None)
        , m_bReverse(false)
    {

    }
    ~SFilterCondition()
    {
        BEATS_SAFE_DELETE(m_pStrFilter);
    }

    void SetType(ELogFilterType type)
    {
        if (m_type != type)
        {
            if (m_type == eLFT_Str)
            {
                BEATS_SAFE_DELETE(m_pStrFilter);
            }
            if (type == eLFT_Str)
            {
                m_pStrFilter = new wxString(_T(""));
            }
            m_type = type;
        }
    }

    ELogFilterType GetType()
    {
        return m_type;
    }

private:
    ELogFilterType m_type;

};

class CLogListCtrl : public wxListCtrl
{
public:
    CLogListCtrl(wxWindow *parent, const wxWindowID id,    const wxPoint& pos,    const wxSize& size,    long style);
    virtual ~CLogListCtrl();

public:
    void AddLog(const TCHAR* pLog, uint32_t logPos, const wxTextAttr* pTextAttr);
    void ClearAllLog();

    void AddFilterCondition(std::vector<SFilterCondition*>& condition);
    void DeleteAllFilterConditions();

    bool ShouldShowLog(const SInfoData& data);
    void UpdateList();

private:
    virtual wxString OnGetItemText(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

private:
    std::vector<uint32_t> m_visibleLogId;

    std::vector<SInfoData*>    m_pLogs;
    std::vector<std::vector<SFilterCondition*>> m_conditionFilters; // AND relationship among m_conditionFilters' element , OR relationship among its element's elements.
    std::map<uint32_t, wxString> m_addressMap;
    CRITICAL_SECTION m_logUpdateSection;
};

#endif