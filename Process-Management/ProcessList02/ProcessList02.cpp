#include <iostream>

#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>

int main()
{
	_wsetlocale(LC_ALL, L"korean");
	HANDLE		hSnapshot = INVALID_HANDLE_VALUE;

	hSnapshot = ::CreateToolhelp32Snapshot( // Create a snapshot of the system
		TH32CS_SNAPPROCESS // Take a snapshot of the system
		, NULL // Take a snapshot of the entire system
	);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32 = { sizeof(pe32) };

		BOOL bFlag = ::Process32First(hSnapshot, &pe32);
		for (; bFlag; bFlag = ::Process32Next(hSnapshot, &pe32))
			_tprintf(TEXT("[PID: %d] %s\n"),
				pe32.th32ProcessID, pe32.szExeFile);
		::CloseHandle(hSnapshot);
	}
	
	return 0;
}
