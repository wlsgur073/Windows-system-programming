// ThreadCtrl.cpp : 이 파일에는 'main' 함수가 포함. 거기서 프로그램 실행이 시작되고 종료.

#include <iostream>
#include <process.h>
#include <windows.h>

int g_bFlag = FALSE;

UINT WINAPI ThreadFunction(LPVOID pParam)
{
    std::cout << "ThreadFunction - begin" << std::endl;
    g_bFlag = TRUE;
    while (g_bFlag)
    {
        std::cout << "Worker thread - while loop" << std::endl;
        ::Sleep(1000);
    }

    std::cout << "ThreadFunction - End" << std::endl;
    return 0;
}

int main()
{
    std::cout << "main() - Begin" << std::endl;
    UINT nThreadId = 0;
    HANDLE hThread = (HANDLE)::_beginthreadex(
        NULL,
        0,
        ThreadFunction,
        (LPVOID)"PARAM",
        0,
        &nThreadId);
    if (hThread == NULL) {
        std::cout << "ERROR: Failed to begin thread." << std::endl;
        return 0;
    }

    ::Sleep(2000);

    std::cout << "main() - SuspendThread()" << std::endl;
    ::SuspendThread(hThread); // no execute

    for (int i = 0; i < 3; ++i)
    {
        ::WaitForSingleObject(hThread, 1000);
        std::cout << "main() - WaitForSingleObject(1000)" << std::endl;
    }

    ::ResumeThread(hThread); // execute
    std::cout << "main() - ResumeThread()" << std::endl;

    ::Sleep(2000);
    g_bFlag = FALSE;
    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);

    std::cout << "main() - End" << std::endl;
    return 0;
}

