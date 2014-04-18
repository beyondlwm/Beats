#ifndef __CUSTOMEVENT
#define __CUSTOMEVENT

#include "wx/event.h"

class TimeBarEvent : public wxCommandEvent
{
public:
    TimeBarEvent(wxEventType cmdType = wxEVT_NULL,int id = 0);
    TimeBarEvent(const TimeBarEvent& event);
    ~TimeBarEvent();

    virtual wxEvent* Clone() const;

public:
    void    SetRow(const int msg);
    int     GetRow() const;

    void    SetType(const int msg);
    int     GetType() const;

    void    SetStrMsg(const wxString& msg);
    const wxString& GetStrMsg() const ;

protected:
    int         m_nIndex;
    int         m_nType;
    wxString    m_strStringMsg;
private:
    DECLARE_DYNAMIC_CLASS(TimeBarEvent)
};


#define ID_CUSTOM_EVENT_TYPE (wxID_HIGHEST + 1)

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(TIMEBAR_EVENT_TYPE, ID_CUSTOM_EVENT_TYPE)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*TimeLineEventFunction)(TimeBarEvent&);

#define EVT_DATAVIEW_CUSTRENDER_ACTIVE(id, fn) DECLARE_EVENT_TABLE_ENTRY( \
    TIMEBAR_EVENT_TYPE, id, -1, \
    (wxObjectEventFunction)(wxEventFunction)(TimeLineEventFunction)&fn, \
    (wxObject*)NULL),

#endif