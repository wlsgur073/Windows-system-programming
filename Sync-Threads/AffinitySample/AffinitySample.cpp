#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <process.h>


// affinity란, 프로세스나 스레드가 실행될 수 있는 CPU의 코어를 제한하는 것을 말한다.
UINT WINAPI ThreadFunction(LPVOID pParam)
{
    int nTmp = 0;
    while (1)
    {
		++nTmp; // busy loop: sleep을 사용하지 않아 CPU 사용률이 100%가 된다.
    }

    return 0;
}

int main()
{
    UINT nThreadId = 0;
	HANDLE hThread = (HANDLE)::_beginthreadex( // 일반적으로는 OS가 코어를 자동으로 할당함.
        NULL,
        0,
        ThreadFunction,
        NULL,
        0,
        &nThreadId);
    if (hThread == NULL) {
        std::cout << "ERROR: Failed to begin thread." << std::endl;
        return 0;
    }

	for (int i = 0; i < 6; ++i) // 논리 코어의 개수만큼 반복
    {
		::SetThreadAffinityMask(hThread, 0x00000001 << i); // 비트 연산자를 이용하여 코어를 선택함.
		// 0001, 0010, 0100, 1000 ... 1000000 이런식으로 코어를 선택할 수 있음.
        ::Sleep(5000);
    }

    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
}
