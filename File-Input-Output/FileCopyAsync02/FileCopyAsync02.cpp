﻿#include <iostream>
#include <stdio.h>
#include <Windows.h>

// if using structure, explicitly define it.
typedef struct _COPY_DATA
{
	LPVOID		pMapView;
	HANDLE		hMap;
	HANDLE		hFileSource;
	HANDLE		hFileTarget;
} COPY_DATA;


VOID CALLBACK WriteFileIOCompletionRoutine(
	DWORD dwErrorCode,
	DWORD dwNumberOfBytesTransfered,
	LPOVERLAPPED lpOverlapped)
{
	_putws(L"WriteFileIOCompletionRoutine() - Begin");
	if (lpOverlapped->hEvent != NULL)
	{
		//각종 핸들 닫기 및 메모리 해제
		COPY_DATA* pCopyData = (COPY_DATA*)lpOverlapped->hEvent;

		::UnmapViewOfFile(pCopyData->pMapView);
		::CloseHandle(pCopyData->hMap);
		::CloseHandle(pCopyData->hFileSource);
		::CloseHandle(pCopyData->hFileTarget);

		free(pCopyData);
		_putws(L"WriteFileIOCompletionRoutine() - Release memory & handles");
	}

	//main() 함수에서 할당한 OVERLAPPED 구조체 메모리 해제
	free(lpOverlapped);
	_putws(L"WriteFileIOCompletionRoutine() - End");
}

void CloseAll(char* pszMem, HANDLE hMap, HANDLE hSrc, HANDLE hDst)
{
	if (pszMem != NULL)
		::UnmapViewOfFile(pszMem);
	if (hMap != NULL)
		::CloseHandle(hMap);
	if (hSrc != NULL)
		::CloseHandle(hSrc);
	if (hDst != NULL)
		::CloseHandle(hDst);
}

int main()
{
	::_wsetlocale(LC_ALL, L"korean");
	HANDLE hFileSource = NULL, hFileTarget = NULL;
	LARGE_INTEGER	llFileSize = { 0 };
	HANDLE	hMap = NULL;
	char* pszMemory = NULL;

	//1. 원본 파일을 읽기모드로 연다.
	hFileSource = ::CreateFile(TEXT("C:\\TEST\\test.zip"),
		GENERIC_READ,				//읽기모드
		FILE_SHARE_READ,			//읽기모드 공유허용
		NULL,						//보안속성 없음.
		OPEN_EXISTING,				//존재하는 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL);						//동기 I/O
	if (hFileSource == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Failed to open source file [ERROR CODE: %d]\n",
			::GetLastError());
		return 0;
	}

	if (!::GetFileSizeEx(hFileSource, &llFileSize))
	{
		wprintf(L"Failed to get source file size [ERROR CODE: %d]\n",
			::GetLastError());
		CloseAll(NULL, NULL, hFileSource, NULL);
		return 0;
	}

	//2. 원본 파일에 대한 파일매핑 객체 생성
	hMap = ::CreateFileMapping(hFileSource, NULL, PAGE_READONLY, 0,
		(DWORD)llFileSize.QuadPart, NULL);
	if (hMap == NULL)
	{
		wprintf(L"Failed to create file mapping obj [ERROR CODE: %d]\n",
			::GetLastError());
		CloseAll(NULL, NULL, hFileSource, NULL);
		return 0;
	}

	//매핑 객체에 대한 접근 포인터를 얻음.
	pszMemory = (char*)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0,
		(DWORD)llFileSize.QuadPart);
	if (pszMemory == NULL)
	{
		CloseAll(NULL, hMap, hFileSource, NULL);
		return 0;
	}

	//3. 대상 파일을 비동기 쓰기모드 모드로 개방
	::DeleteFile(TEXT("C:\\TEST\\test - copy.zip"));
	hFileTarget = ::CreateFile(TEXT("C:\\TEST\\test - copy.zip"),
		GENERIC_ALL,				//쓰기모드
		0,							//공유안함.
		NULL,						//보안속성 없음.
		CREATE_ALWAYS,				//새파일 생성
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (hFileTarget == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Failed to open target file [ERROR CODE: %d]\n",
			::GetLastError());
		CloseAll(pszMemory, hMap, hFileSource, NULL);
		return 0;
	}

	//4. WriteFileEx() 함수를 이용해서 비동기 모드로 저장한다.
	LPOVERLAPPED pOverLapped = (LPOVERLAPPED)malloc(sizeof(OVERLAPPED));
	pOverLapped->OffsetHigh = 0;
	pOverLapped->Offset = 0;

	//※ 지금 할당된 메모리는 나중에 완료 루틴에서 해제
	COPY_DATA* pCopyData = (COPY_DATA*)malloc(sizeof(COPY_DATA)); // COPY_DATA is a user-defined structure
	pCopyData->pMapView = pszMemory;
	pCopyData->hMap = hMap;
	pCopyData->hFileSource = hFileSource;
	pCopyData->hFileTarget = hFileTarget;

	//※이벤트 핸들을 포인터로 활용
	pOverLapped->hEvent = pCopyData;
	if (!::WriteFileEx(hFileTarget, pszMemory, (DWORD)llFileSize.QuadPart
		, pOverLapped
		, WriteFileIOCompletionRoutine) // callback function
		)
	{
		wprintf(L"WriteFileEx [ERROR CODE: %d]", ::GetLastError());
		CloseAll(pszMemory, hMap, hFileSource, hFileTarget);
		return 0;
	}

	//5. 비동기 처리가 완료 될때까지 대기
	for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION;);
	_putws(L"main() - End\n");
	return 0;
}

// file -> pointer -> read&write -> IOCP -> completion routine -> callback function -> free memory