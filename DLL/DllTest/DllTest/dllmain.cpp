#include "pch.h"
#include <stdio.h>


__declspec(dllexport) void WINAPI DllTestFunction(int);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void WINAPI DllTestFunction(int nParam)
{
    printf("DLL TEST: DllTestFunction(param: %d)\n", nParam);
}
