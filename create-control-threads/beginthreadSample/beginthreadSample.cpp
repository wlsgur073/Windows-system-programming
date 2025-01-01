// beginthreadSample.cpp : 이 파일에는 'main' 함수가 포함. 거기서 프로그램 실행이 시작되고 종료.

#include <iostream>
#include <process.h>
#include <windows.h>

UINT WINAPI ThreadFunction(LPVOID pParam)
{
    std::cout << "ThreadFunction() - Begin" << std::endl;
    std::cout << (const char*)pParam << std::endl;
    std::cout << "ThreadFunction() - End" << std::endl;
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
    }

    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);

    std::cout << "main() - End" << std::endl;
    return 0;
}
