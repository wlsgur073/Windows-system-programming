﻿#include <iostream>
#include <tchar.h>
#include <windows.h>

int main()
{
	// debugview를 이용하여 출력된 문자열을 확인할 수 있음
	OutputDebugString(_T("Client - begin\n"));

	//1. 파일 매핑 객체 생성(혹은 개방)
	HANDLE hMap = ::CreateFileMapping(
		INVALID_HANDLE_VALUE, 
		NULL,
		PAGE_READWRITE, // read/write
		0,
		128, // shared memory size in kernel
		TEXT("IPC_TEST_SHARED_MEMORY"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//파일 매핑 객체가 이미 생성되어 있으므로 기존 것을 오픈
		hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,
			TEXT("IPC_TEST_SHARED_MEMORY"));
		if (hMap == NULL)
		{
			_tprintf(TEXT("Failed to open file mapping obj. (%d)\n"),
				::GetLastError());
			return FALSE;
		}
	}
	 
	//2. 매핑 객체에 대한 접근 포인터를 얻음. (메모리 추상화)
	TCHAR *pSharedMemory = (TCHAR*)::MapViewOfFile(
		hMap, FILE_MAP_ALL_ACCESS, 0, 0, 128);
	if (pSharedMemory == NULL)
	{
		_tprintf(TEXT("Failed to get shared memory. (%d)\n"), // unicode string
			::GetLastError());
		::CloseHandle(hMap);
		return 0;
	}

	//3. 공유 메모리 접근 및 읽기(수신 완료) 이벤트 생성(혹은 개방)
	HANDLE hEvent = ::CreateEvent(
		NULL, TRUE, FALSE, TEXT("IPC_SHAREDMEM_ACCESS"));
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hEvent = ::OpenEvent(
			EVENT_ALL_ACCESS, FALSE, TEXT("IPC_SHAREDMEM_ACCESS"));
		if (hEvent == NULL)
		{
			_tprintf(TEXT("Failed to open event obj. (%d)\n"),
				::GetLastError());

			::UnmapViewOfFile(pSharedMemory);
			::CloseHandle(hMap);
			return 0;
		}
	}

	HANDLE hEvtComplete = ::CreateEvent(
		NULL, TRUE, FALSE, TEXT("IPC_SHAREDMEM_RECV_COMPLETE")); // completion event
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hEvtComplete = ::OpenEvent(
			EVENT_ALL_ACCESS, FALSE, TEXT("IPC_SHAREDMEM_RECV_COMPLETE"));
		if (hEvent == NULL)
		{
			_tprintf(TEXT("Failed to open event obj. (%d)\n"),
				::GetLastError());

			::UnmapViewOfFile(pSharedMemory);
			::CloseHandle(hMap);
			::CloseHandle(hEvent);
			return 0;
		}
	}

	//4. 프로세스 동기화를 위한 뮤텍스 생성
	HANDLE hMutex = ::CreateMutex(NULL, FALSE,
		TEXT("IPC_SHAREDMEM_MUTEX"));
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE,
			TEXT("IPC_SHAREDMEM_MUTEX"));
		if(hMutex == NULL)
		{
			_tprintf(TEXT("Failed to open mutex obj. (%d)\n"),
				::GetLastError());

			::UnmapViewOfFile(pSharedMemory);
			::CloseHandle(hMap);
			::CloseHandle(hEvent);
			::CloseHandle(hEvtComplete);
			return 0;
		}
	}

	//5. 공유 메모리 쓰기 이벤트 대기
	OutputDebugString(TEXT("Waiting message from server...\n"));
	if (::WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0) // wait for the event from the server
	{
		//6. 뮤텍스를 이용한 프로세스 동기화 (메모리 접근)
		if(::WaitForSingleObject(hMutex, INFINITE) != WAIT_OBJECT_0)
			OutputDebugString(TEXT("MUTEX error.\n"));
		else
			OutputDebugString(TEXT("MUTEX Lock.\n")); // like a critical section start

		OutputDebugString(pSharedMemory);
		::ReleaseMutex(hMutex);
		OutputDebugString(TEXT("MUTEX Unlock.\n")); // // like a critical section end

		//7. 메모리에 읽기 완료 이벤트 셋
		::SetEvent(hEvtComplete);
		OutputDebugString(_T("Completion event for server!\n"));
	}

	::UnmapViewOfFile(pSharedMemory);
	::CloseHandle(hMap);
	::CloseHandle(hEvent);
	::CloseHandle(hEvtComplete);
	::CloseHandle(hMutex);

	_tprintf(_T("Client - end\n"));
	return 0;
}
