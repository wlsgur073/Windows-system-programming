#include <iostream>
#include <tchar.h>
#include <windows.h>

int main()
{
	SYSTEM_INFO si = { 0 };

	::GetSystemInfo(&si);
	_tprintf(_T("Page size: %d\n"), si.dwPageSize);
	_tprintf(_T("Allocation size: %d\n"), si.dwAllocationGranularity);
}
