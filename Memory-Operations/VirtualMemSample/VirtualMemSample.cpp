#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>

int main()
{
	const char* pszHello = "Hello, World!"; // read-only memory
    printf("%s\n", pszHello);

    DWORD dwOldProtect = 0;
    ::VirtualProtect((LPVOID)pszHello,
		16, PAGE_READWRITE, &dwOldProtect); // change the memory protection

	strcpy_s((char*)pszHello, 16, "nullnull!=null"); // write to read-only memory
    printf("%s\n", pszHello);
}

