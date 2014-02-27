// DebugTool.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#ifdef _MANAGED
#pragma managed(push, off)
#endif
#include <dbghelp.h>

static bool bInitializeSuccess = false;

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                      DWORD  ul_reason_for_call,
                      LPVOID /*lpReserved*/
                      )
{    
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
            SymSetOptions(SYMOPT_LOAD_LINES);
            bInitializeSuccess = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
            BEATS_ASSERT(bInitializeSuccess, _T("Initialize windows debug tool failed!"));
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (bInitializeSuccess)
        {
            SymCleanup(GetCurrentProcess());
        }
        break;
    }

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

