#include <iostream>
#include <stdio.h>
#include <process.h>
#include <Windows.h>


HANDLE g_hSema;
TCHAR g_StringList[10][64] = { 0 }; // 컴파일러 설정에 따라서 1byte 문자열이거나 와이드	문자열로 설정될 수 있음.

UINT WINAPI ThreadSemaphore(LPVOID pParam)
{
	int nIndex = (int)pParam;

	while (TRUE)
	{
		///////////////////////////////////////////////////////
		::wsprintf(g_StringList[nIndex],
			TEXT("%d thread is waiting!"), nIndex);
		::Sleep(500);

		///////////////////////////////////////////////////////
		DWORD dwResult = ::WaitForSingleObject(g_hSema, INFINITE); // 총 10개의 스레드 중, 3개의 스레드만 세마포어를 획득할 수 있음.
		::wsprintf(g_StringList[nIndex],
			TEXT("##%d thread is selected!##"), nIndex);
		::Sleep(500);
		::ReleaseSemaphore(g_hSema
			, 1 // lReleaseCount: 해당 숫자만큼 세마포어를 해제하고 다른 스레드가 세마포어를 획득할 수 있게 함.
			, NULL);
	}
	return 0;
}

int main()
{
	g_hSema = ::CreateSemaphore(
		NULL
		, 3 // lInitialCount
		, 3 // lMaximumCount
		, NULL
	);

	UINT nThreadId = 0;
	HANDLE hThread = NULL;

	for (int i = 0; i < 10; ++i)
	{
		hThread = (HANDLE)::_beginthreadex(
			NULL,
			0,
			ThreadSemaphore,
			(LPVOID)i, // index
			0,
			&nThreadId);
		::CloseHandle(hThread);
	}

	while (1)
	{
		system("cls");
		for (int i = 0; i < 10; ++i)
			_putws(g_StringList[i]);

		::Sleep(1000);
	}

	::CloseHandle(g_hSema);
}
/*
* 결론: 세마포어는 특정 개수만큼의 스레드만 동시에 실행되도록 제한하는 기능을 제공한다.
*  Critical Section과의 차이점: Critical Section은 단일 스레드만 진입할 수 있지만, 세마포어는 특정 개수만큼의 스레드만 진입할 수 있다.
*/