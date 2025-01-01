#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <process.h>

char* g_pszBuffer = NULL;
CRITICAL_SECTION g_cs;

void SetString(const char* pszData)
{
	::EnterCriticalSection(&g_cs); // 임계 영역 구간 시작
	if (g_pszBuffer != NULL) // 기존 메모리 free 시키고 다시 malloc함.
	{
		free(g_pszBuffer); 
		g_pszBuffer = (char*)malloc(64);
		sprintf_s(g_pszBuffer, 64, "%s", pszData);
	}
	else
	{
		g_pszBuffer = (char*)malloc(64); // 결론적으로 무조건 동적 할당함.
		sprintf_s(g_pszBuffer, 64, "%s", pszData);
	}
	::LeaveCriticalSection(&g_cs);
}

BOOL GetString(char* pszData)
{
	::EnterCriticalSection(&g_cs);
	if (pszData != NULL)
	{
		sprintf_s(pszData, 64, "%s", g_pszBuffer); // pszData에 g_pszBuffer를 복사함.
		free(g_pszBuffer);
		g_pszBuffer = NULL; // g_pszBuffer를 NULL로 초기화함.

		::LeaveCriticalSection(&g_cs);
		return TRUE;
	}

	::LeaveCriticalSection(&g_cs);
	return FALSE;
}

/*
* SetString()이든 GetString()이든 싱글스레드 안에서는 문제가 없지만
* 멀티스레드에서는 동시에 같은 g_pszBuffer를 접근하다가 문제가 발생할 수 있음.
*/

unsigned int __stdcall ThreadFunc1(LPVOID pParam)
{
	while (TRUE)
	{
		::Sleep(1); // cpu 사용량을 줄이기 위해 1ms 대기
		SetString("ThreadFunc1");
	}
	return 0;
}
unsigned int __stdcall ThreadFunc2(LPVOID pParam)
{
	while (TRUE)
	{
		::Sleep(1);
		SetString("ThreadFunc2");
	}
	return 0;
}

int main()
{
	::InitializeCriticalSection(&g_cs); // Critical section을 이용한 동기화

	UINT nThreadId = 0;
	HANDLE hThread = (HANDLE)::_beginthreadex(
		NULL,
		0,
		ThreadFunc1,
		NULL,
		0,
		&nThreadId);
	::CloseHandle(hThread);

	hThread = (HANDLE)::_beginthreadex(
		NULL,
		0,
		ThreadFunc2,
		NULL,
		0,
		&nThreadId);

	char szBuffer[64];
	for (int i = 0; i < 5; ++i)
	{
		::Sleep(500);

		GetString(szBuffer);
		puts(szBuffer);
	}

	::WaitForSingleObject(hThread, INFINITE);
	::CloseHandle(hThread);

	::DeleteCriticalSection(&g_cs);
    return 0;
}

/*
* 결론:
* 전역 변수를 하나 두고, 하나의 포인터에 malloc도하고 free도 하는데, 이것을 여러 스레드가 동시에 접근하면 문제가 발생할 수 있음.
* 그래서 어떤 코드의 접근하는 구간을 잘 산정하고 임계 구간(CRITICAL_SECTION)을 설정해주어야 함.
* 
* 그렇다고 CRITICAL_SECTION 구간을 무작정 늘려서 사용하는게 좋은 것은 아님.
* - Deadlock: 두 개 이상의 스레드가 서로 다른 임계 구간을 점유하고 있을 때, 서로 상대방이 점유한 임계 구간을 기다리는 현상
* - 효율성 저하: 임계 구간을 사용하면 다른 스레드가 접근하지 못하도록 되어 있어서, 성능이 저하될 수 있음.
* 
* Thread 동기화에서	가장 흔하게	사용되는 방법은 Critical Section임.
*/