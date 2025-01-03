#include "pch.h"
#include <tchar.h>

// extern is explicitly used to declare the function as an external function.
extern "C" __declspec(dllexport) DWORD WINAPI MyDllFunction(void);
extern "C" __declspec(dllexport) void WINAPI SetStaticTest(int);
extern "C" __declspec(dllexport) int WINAPI GetStaticTest(void);
extern "C" __declspec(dllexport) int g_nTest; // possible to access global variable from DLL.
// not recommended to code global variable in DLL, cuz it can be collided.


extern "C" __declspec(dllexport) void WINAPI SetTlsData(int);
extern "C" __declspec(dllexport) int WINAPI GetTlsData(void);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	switch (ul_reason_for_call) // four cases
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString(_T("DLL Sample: DLL_PROCESS_ATTACH"));
        break;

    case DLL_THREAD_ATTACH:
        OutputDebugString(_T("DLL Sample: DLL_THREAD_ATTACH"));
        break;

    case DLL_THREAD_DETACH:
        OutputDebugString(_T("DLL Sample: DLL_THREAD_DETACH"));
        break;

    case DLL_PROCESS_DETACH:
        OutputDebugString(_T("DLL Sample: DLL_PROCESS_DETACH"));
        break;
    }
    return TRUE;
}

DWORD WINAPI MyDllFunction(void)
{
    OutputDebugString(_T("DLL Sample: MyDllFunction()"));
    return 0;
}


int g_nTest = 0;

void WINAPI SetStaticTest(int nParam)
{
    g_nTest = nParam;
}

int WINAPI GetStaticTest(void)
{
    return g_nTest;
}

__declspec(thread) int g_tlsData = 0;

void WINAPI SetTlsData(int nParam)
{
    g_tlsData = nParam;
}

int WINAPI GetTlsData(void)
{
    return g_tlsData;
}
