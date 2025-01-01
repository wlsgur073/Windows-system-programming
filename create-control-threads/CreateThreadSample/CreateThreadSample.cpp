// CreateThreadSample.cpp : 이 파일에는 'main' 함수가 포함. 거기서 프로그램 실행이 시작되고 종료.

#include <iostream>
#include <windows.h>

// WINAPI는 __stdcall이다.
// __stdcall의 특징: 스택 정리 책임, 파라미터 전달 순서(right to left), 함수 이름 맹글링
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
    std::cout << "ThreadFunction() - Begin" << std::endl;
    std::cout << (const char*)pParam << std::endl;
    std::cout << "ThreadFunction() - End" << std::endl;
    return 0;
}

int main()
{
    std::cout << "main() - Begin" << std::endl;
    DWORD dwThreadId = 0;
    HANDLE hThread = ::CreateThread(
        NULL,
        0,
        ThreadFunction, // 생성이 되면서 자식 스레드가 됨.
        (LPVOID)"PARAM",
        0,
        &dwThreadId);
    if (hThread == NULL) {
    }

    std::cout << "main() - WaitForSingleObject() - before" << std::endl;

    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);

    std::cout << "main() - End" << std::endl;
    return 0;
}
