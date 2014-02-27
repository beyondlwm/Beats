// Components.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "malloc.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif


BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  /*ul_reason_for_call*/,
                       LPVOID /*lpReserved*/
                     )
{
    return TRUE;
}

__declspec(dllexport) void ForceExport(){ };

#ifdef _MANAGED
#pragma managed(pop)
#endif

