#ifndef BEATS_CONTROL_MOUSEPUBLICDEF_H__INCLUDE
#define BEATS_CONTROL_MOUSEPUBLICDEF_H__INCLUDE

enum EMouseBtnState
{
    eMBS_Release,
    eMBS_JustRelease,
    eMBS_Press,
    eMBS_JustPress,
    eMBS_DoubleClick,

    eMBS_Count,
    eMBS_ForceTo32Bit = 0xffffffff
};

enum EMouseButton
{
    eMB_Left,
    eMB_Right,
    eMB_Middle,

    eMB_Count,
    eMB_ForceTo32Bit = 0xffffffff
};


#endif