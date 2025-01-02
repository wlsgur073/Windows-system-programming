#include <iostream>
#include <stdio.h>
#include <windows.h>

int main()
{
	::_wsetlocale(LC_ALL, L"korean");

	HANDLE hFileSource = NULL, hFileTarget = NULL;

	// 1. Open the source file in read mode.
	hFileSource = ::CreateFile(TEXT("C:\\TEST\\test.zip"),
		GENERIC_READ,				// Read mode
		FILE_SHARE_READ,			// Allow read sharing
		NULL,						// No security attributes
		OPEN_EXISTING,				// Open existing file
		FILE_ATTRIBUTE_NORMAL,		// Normal file attribute
		NULL);						// Synchronous 
	if (hFileSource == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Failed to open source file [ERROR CODE: %d]\n",
			::GetLastError());
		return 0;
	}

	// 2. Open the target file in write mode.
	hFileTarget = ::CreateFile(TEXT("C:\\TEST\\test - copy.zip"),
		GENERIC_WRITE,				// Write mode
		0,							// Do not share
		NULL,						// No security attributes
		CREATE_ALWAYS,				// Always create a new file // if the file exists, it will be overwritten.
		FILE_ATTRIBUTE_NORMAL,		// Normal file attribute
		NULL);						// Synchronous 
	if (hFileTarget == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Failed to open target file [ERROR CODE: %d]\n",
			::GetLastError());

		::CloseHandle(hFileSource);
		return 0;
	}

	// 3. Loop to copy the file content.
	LARGE_INTEGER	llFileSize = { 0 };
	LONGLONG	llTotalReadSize = 0;
	DWORD		dwReadSize = 0, dwWriteSize = 0;
	BOOL		bResult = FALSE;
	BYTE		byBuffer[65536];

	if (!::GetFileSizeEx(hFileSource, &llFileSize))
	{
		wprintf(L"원본 파일의 크기를 알 수 없습니다.\n");
		::CloseHandle(hFileSource);
		::CloseHandle(hFileTarget);
		return 0;
	}

	for (LONGLONG i = 0; i < llFileSize.QuadPart; i += dwReadSize)
	{
		// Read and copy 64KB at a time // page size is 4KB
		::ZeroMemory(byBuffer, 65536);
		bResult = ::ReadFile(hFileSource, byBuffer, 65536, &dwReadSize, NULL);
		if (!bResult)
		{
			wprintf(L"Failed to read source file [ERROR CODE: %d]\n",
				::GetLastError());
			break;
		}
		// End of file
		else if (bResult && dwReadSize == 0)
			break;

		llTotalReadSize += dwReadSize;
		wprintf(L"%I64d%%\n",
			llTotalReadSize * 100 / llFileSize.QuadPart);

		// Write 64KB at a time
		if (!::WriteFile(hFileTarget, byBuffer, dwReadSize, &dwWriteSize, NULL)
			|| dwReadSize != dwWriteSize)
		{
			wprintf(L"Failed to write target file [ERROR CODE: %d]\n",
				::GetLastError());
			break;
		}
	}

	_putws(L"Complete!\n");
	::CloseHandle(hFileSource);
	::CloseHandle(hFileTarget);
	return 0;
}
