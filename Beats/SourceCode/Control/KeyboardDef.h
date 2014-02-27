#ifndef BEATS_CONTROL_KEYBOARDDEF_H__INCLUDE
#define BEATS_CONTROL_KEYBOARDDEF_H__INCLUDE

//! Keyboard scan codes
enum EKeyCode
{
    eKC_UNASSIGNED  = 0x00,
    eKC_ESCAPE      = 0x01,
    eKC_1           = 0x02,
    eKC_2           = 0x03,
    eKC_3           = 0x04,
    eKC_4           = 0x05,
    eKC_5           = 0x06,
    eKC_6           = 0x07,
    eKC_7           = 0x08,
    eKC_8           = 0x09,
    eKC_9           = 0x0A,
    eKC_0           = 0x0B,
    eKC_MINUS       = 0x0C,    // - on main keyboard
    eKC_EQUALS      = 0x0D,
    eKC_BACK        = 0x0E,    // backspace
    eKC_TAB         = 0x0F,
    eKC_Q           = 0x10,
    eKC_W           = 0x11,
    eKC_E           = 0x12,
    eKC_R           = 0x13,
    eKC_T           = 0x14,
    eKC_Y           = 0x15,
    eKC_U           = 0x16,
    eKC_I           = 0x17,
    eKC_O           = 0x18,
    eKC_P           = 0x19,
    eKC_LBRACKET    = 0x1A,
    eKC_RBRACKET    = 0x1B,
    eKC_RETURN      = 0x1C,    // Enter on main keyboard
    eKC_LCONTROL    = 0x1D,
    eKC_A           = 0x1E,
    eKC_S           = 0x1F,
    eKC_D           = 0x20,
    eKC_F           = 0x21,
    eKC_G           = 0x22,
    eKC_H           = 0x23,
    eKC_J           = 0x24,
    eKC_K           = 0x25,
    eKC_L           = 0x26,
    eKC_SEMICOLON   = 0x27,
    eKC_APOSTROPHE  = 0x28,
    eKC_GRAVE       = 0x29,    // accent
    eKC_LSHIFT      = 0x2A,
    eKC_BACKSLASH   = 0x2B,
    eKC_Z           = 0x2C,
    eKC_X           = 0x2D,
    eKC_C           = 0x2E,
    eKC_V           = 0x2F,
    eKC_B           = 0x30,
    eKC_N           = 0x31,
    eKC_M           = 0x32,
    eKC_COMMA       = 0x33,
    eKC_PERIOD      = 0x34,    // . on main keyboard
    eKC_SLASH       = 0x35,    // / on main keyboard
    eKC_RSHIFT      = 0x36,
    eKC_MULTIPLY    = 0x37,    // * on numeric keypad
    eKC_LMENU       = 0x38,    // left Alt
    eKC_SPACE       = 0x39,
    eKC_CAPITAL     = 0x3A,
    eKC_F1          = 0x3B,
    eKC_F2          = 0x3C,
    eKC_F3          = 0x3D,
    eKC_F4          = 0x3E,
    eKC_F5          = 0x3F,
    eKC_F6          = 0x40,
    eKC_F7          = 0x41,
    eKC_F8          = 0x42,
    eKC_F9          = 0x43,
    eKC_F10         = 0x44,
    eKC_NUMLOCK     = 0x45,
    eKC_SCROLL      = 0x46,    // Scroll Lock
    eKC_NUMPAD7     = 0x47,
    eKC_NUMPAD8     = 0x48,
    eKC_NUMPAD9     = 0x49,
    eKC_SUBTRACT    = 0x4A,    // - on numeric keypad
    eKC_NUMPAD4     = 0x4B,
    eKC_NUMPAD5     = 0x4C,
    eKC_NUMPAD6     = 0x4D,
    eKC_ADD         = 0x4E,    // + on numeric keypad
    eKC_NUMPAD1     = 0x4F,
    eKC_NUMPAD2     = 0x50,
    eKC_NUMPAD3     = 0x51,
    eKC_NUMPAD0     = 0x52,
    eKC_DECIMAL     = 0x53,    // . on numeric keypad
    eKC_OEM_102     = 0x56,    // < > | on UK/Germany keyboards
    eKC_F11         = 0x57,
    eKC_F12         = 0x58,
    eKC_F13         = 0x64,    //                     (NEC PC98)
    eKC_F14         = 0x65,    //                     (NEC PC98)
    eKC_F15         = 0x66,    //                     (NEC PC98)
    eKC_KANA        = 0x70,    // (Japanese keyboard)
    eKC_ABNT_C1     = 0x73,    // / ? on Portugese (Brazilian) keyboards
    eKC_CONVERT     = 0x79,    // (Japanese keyboard)
    eKC_NOCONVERT   = 0x7B,    // (Japanese keyboard)
    eKC_YEN         = 0x7D,    // (Japanese keyboard)
    eKC_ABNT_C2     = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards
    eKC_NUMPADEQUALS= 0x8D,    // = on numeric keypad (NEC PC98)
    eKC_PREVTRACK   = 0x90,    // Previous Track (eKC_CIRCUMFLEX on Japanese keyboard)
    eKC_AT          = 0x91,    //                     (NEC PC98)
    eKC_COLON       = 0x92,    //                     (NEC PC98)
    eKC_UNDERLINE   = 0x93,    //                     (NEC PC98)
    eKC_KANJI       = 0x94,    // (Japanese keyboard)
    eKC_STOP        = 0x95,    //                     (NEC PC98)
    eKC_AX          = 0x96,    //                     (Japan AX)
    eKC_UNLABELED   = 0x97,    //                        (J3100)
    eKC_NEXTTRACK   = 0x99,    // Next Track
    eKC_NUMPADENTER = 0x9C,    // Enter on numeric keypad
    eKC_RCONTROL    = 0x9D,
    eKC_MUTE        = 0xA0,    // Mute
    eKC_CALCULATOR  = 0xA1,    // Calculator
    eKC_PLAYPAUSE   = 0xA2,    // Play / Pause
    eKC_MEDIASTOP   = 0xA4,    // Media Stop
    eKC_VOLUMEDOWN  = 0xAE,    // Volume -
    eKC_VOLUMEUP    = 0xB0,    // Volume +
    eKC_WEBHOME     = 0xB2,    // Web home
    eKC_NUMPADCOMMA = 0xB3,    // , on numeric keypad (NEC PC98)
    eKC_DIVIDE      = 0xB5,    // / on numeric keypad
    eKC_SYSRQ       = 0xB7,
    eKC_RMENU       = 0xB8,    // right Alt
    eKC_PAUSE       = 0xC5,    // Pause
    eKC_HOME        = 0xC7,    // Home on arrow keypad
    eKC_UP          = 0xC8,    // UpArrow on arrow keypad
    eKC_PGUP        = 0xC9,    // PgUp on arrow keypad
    eKC_LEFT        = 0xCB,    // LeftArrow on arrow keypad
    eKC_RIGHT       = 0xCD,    // RightArrow on arrow keypad
    eKC_END         = 0xCF,    // End on arrow keypad
    eKC_DOWN        = 0xD0,    // DownArrow on arrow keypad
    eKC_PGDOWN      = 0xD1,    // PgDn on arrow keypad
    eKC_INSERT      = 0xD2,    // Insert on arrow keypad
    eKC_DELETE      = 0xD3,    // Delete on arrow keypad
    eKC_LWIN        = 0xDB,    // Left Windows key
    eKC_RWIN        = 0xDC,    // Right Windows key
    eKC_APPS        = 0xDD,    // AppMenu key
    eKC_POWER       = 0xDE,    // System Power
    eKC_SLEEP       = 0xDF,    // System Sleep
    eKC_WAKE        = 0xE3,    // System Wake
    eKC_WEBSEARCH   = 0xE5,    // Web Search
    eKC_WEBFAVORITES= 0xE6,    // Web Favorites
    eKC_WEBREFRESH  = 0xE7,    // Web Refresh
    eKC_WEBSTOP     = 0xE8,    // Web Stop
    eKC_WEBFORWARD  = 0xE9,    // Web Forward
    eKC_WEBBACK     = 0xEA,    // Web Back
    eKC_MYCOMPUTER  = 0xEB,    // My Computer
    eKC_MAIL        = 0xEC,    // Mail
    eKC_MEDIASELECT = 0xED     // Media Select
};

static TCHAR* KeyCodeLabel[] = 
{
_T("UNASSIGNED"),
_T("ESCAPE"),
_T("1"),
_T("2"),
_T("3"),
_T("4"),
_T("5"),
_T("6"),
_T("7"),
_T("8"),
_T("9"),
_T("0"),
_T("MINUS"),
_T("EQUALS"),
_T("BACK"),
_T("TAB"),
_T("Q"),
_T("W"),
_T("E"),
_T("R"),
_T("T"),
_T("Y"),
_T("U"),
_T("I"),
_T("O"),
_T("P"),
_T("LBRACKET"),
_T("RBRACKET"),
_T("RETURN"),
_T("LCONTROL"),
_T("A"),
_T("S"),
_T("D"),
_T("F"),
_T("G"),
_T("H"),
_T("J"),
_T("K"),
_T("L"),
_T("SEMICOLON"),
_T("APOSTROPHE"),
_T("GRAVE"),
_T("LSHIFT"),
_T("BACKSLASH"),
_T("Z"),
_T("X"),
_T("C"),
_T("V"),
_T("B"),
_T("N"),
_T("M"),
_T("COMMA"),
_T("PERIOD"),
_T("SLASH"),
_T("RSHIFT"),
_T("MULTIPLY"),
_T("LMENU"),
_T("SPACE"),
_T("CAPITAL"),
_T("F1"),
_T("F2"),
_T("F3"),
_T("F4"),
_T("F5"),
_T("F6"),
_T("F7"),
_T("F8"),
_T("F9"),
_T("F10"),
_T("NUMLOCK"),
_T("SCROLL"),
_T("NUMPAD7"),
_T("NUMPAD8"),
_T("NUMPAD9"),
_T("SUBTRACT"),
_T("NUMPAD4"),
_T("NUMPAD5"),
_T("NUMPAD6"),
_T("ADD"),
_T("NUMPAD1"),
_T("NUMPAD2"),
_T("NUMPAD3"),
_T("NUMPAD0"),
_T("DECIMAL"),
_T("OEM_102"),
_T("F11"),
_T("F12")
};

//! Enum of bit position of modifier
enum EModifier
{
    eM_Shift = 0x0000001,
    eM_Ctrl  = 0x0000010,
    eM_Alt   = 0x0000100,

    eM_Count = 3,
    eM_ForceTo32Bite = 0xFFFFFFFF
};

enum EKeyState
{
    eKS_Release,
    eKS_JustRelease,
    eKS_Press,
    eKS_JustPress,

    eKS_Count,
    eKS_ForceTo32Bit = 0xffffffff
};

#endif