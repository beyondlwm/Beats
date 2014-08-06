// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include "BeatsPlatFormConfig.h"

#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
#ifndef WINVER                // Allow use of features specific to Windows XP or later.
#define WINVER 0x0501        // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501    // Change this to the appropriate value to target other versions of Windows.
#endif                        

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE            // Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600    // Change this to the appropriate value to target other versions of IE.
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif

#include "BeatsTchar.h"
#include "BeatsTypeDef.h"

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <mmsystem.h>

#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <math.h>

#include <d3dx9.h>
#include <DXErr.h>

#include "PublicDef.h"
//#define BEATS_DEBUG_TOOL_DISABLE
#include "../DebugTool/BeatsDebugTool.h"

#define DIRECTINPUT_VERSION 0x0800

#define BEATS_WINDOW_WIDTH 800
#define BEATS_WINDOW_HEIGHT 600

#ifdef _UNICODE
    #define TString std::wstring
    #define TOfstream std::wofstream
    #define TIfstream std::wifstream
#else
    #define TString std::string
    #define TOfstream std::ofstream
    #define TIfstream std::ifstream
#endif

namespace NBDT
{
    enum EPerfromanceType
    {
        ePT_RuntimePerformance,
        ePT_Input,
        ePT_CaptureMouse,
        ePT_CaptureKeyboard,
        ePT_InputCallback,
        ePT_Render,
        ePT_RenderCache,
        ePT_RenderParticle,
        ePT_UpdateUI,
        ePT_UpdateUISystem,
        ePT_UpdateUIPages,
        ePT_Physics,
        ePT_Object,
        ePT_Task,

        ePT_Count,
        ePT_ForceTo32Bit = 0xffffffff
    };

    static const TCHAR* perfromanceStr[] =
    {
        _T("RunTimePerformance"),
        _T("Input"),
        _T("CaptureMouse"),
        _T("CaptureKeyboard"),
        _T("InputCallBack"),
        _T("Render"),
        _T("RenderCache"),
        _T("RenderParticle"),
        _T("UpdateUI"),
        _T("UpdateUISystem"),
        _T("UpdateUIPages"),

        _T("Physics"),
        _T("Object"),
        _T("Task"),
    };
}

// TODO: reference additional headers your program requires here
