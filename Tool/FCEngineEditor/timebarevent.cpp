#include "stdafx.h"
#include "timebarevent.h"

DEFINE_EVENT_TYPE(TIMEBAR_EVENT_TYPE)
IMPLEMENT_DYNAMIC_CLASS(TimeBarEvent, wxCommandEvent)

TimeBarEvent::TimeBarEvent(wxEventType cmdType ,int id)
:wxCommandEvent(cmdType, id)
{
    m_nIndex = 0;
    m_nType = 0;
}

TimeBarEvent::TimeBarEvent(const TimeBarEvent& event)
:wxCommandEvent(event)
{

}

TimeBarEvent::~TimeBarEvent()
{

}

wxEvent* TimeBarEvent::Clone() const 
{
    return new TimeBarEvent(*this);
}

void TimeBarEvent::SetRow(const int msg) 
{ 
    m_nIndex = msg;
}

int TimeBarEvent::GetRow() const 
{
    return m_nIndex;
}

void TimeBarEvent::SetType(const int msg) 
{
    m_nType = msg;
}

int TimeBarEvent::GetType() const 
{ 
    return m_nType;
}

void TimeBarEvent::SetStrMsg(const wxString& msg) 
{ 
    m_strStringMsg = msg;
}

const wxString& TimeBarEvent::GetStrMsg() const 
{
    return m_strStringMsg;
}
