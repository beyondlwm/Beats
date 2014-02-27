#ifndef BEATS_GAME_TASKS_TASKDEF_H__INCLUDE
#define BEATS_GAME_TASKS_TASKDEF_H__INCLUDE

enum ETaskType
{
    eTT_Title,
    eTT_RecordTrack,
    eTT_PlayTrack,

    eTT_Count,
    eTT_Force32Bit = 0xffffffff
};

static const TCHAR* TaskName[] = 
{
    _T("eTT_Title"),

    _T("eTT_RecordTrack"),
    _T("eTT_PlayTrack")
};

struct SBeatRecord
{
    unsigned long m_sampleIndex;
    DWORD m_keyCode;
};

#endif