// CreateProcessSample.cpp : 이 파일에는 'main' 함수가 포함. 거기서 프로그램 실행이 시작되고 종료.

#include <iostream>

#include <Windows.h>
#include <lm.h>
#include <Sddl.h>
#include <Psapi.h>


void CreateSample01()
{
	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	TCHAR szBuffer[_MAX_FNAME] = TEXT("notepad.exe");
	if (::CreateProcess(
		NULL, // No module name (use command line). 
		szBuffer,
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		0,                // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi)              // Pointer to PROCESS_INFORMATION structure.
		)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}


void CreateSample02()
{
	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(pi)); // memset 0으로 초기화하는 것과 유사함.

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = (LPTSTR)TEXT("winsta0\\default");

	HANDLE hToken = NULL; //handle나오면 그냥 포인터라고 생각하셈.
	HANDLE hProcess = NULL;

	//현재 프로세스에 대한 토큰을 얻는다.
	hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION,
		TRUE, ::GetCurrentProcessId());
	::OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);

	//얻은 토큰으로 프로세스를 생성한다.
	TCHAR szBuffer[_MAX_FNAME] = TEXT("notepad.exe");
	if (CreateProcessAsUser(
		hToken,
		NULL, // No module name (use command line). 
		szBuffer,
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		TRUE,             // Set handle inheritance to FALSE. 
		CREATE_NEW_CONSOLE,
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi)              // Pointer to PROCESS_INFORMATION structure.
		)
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	::CloseHandle(hToken);
	::CloseHandle(hProcess);
}

void PrintSid()
{
	HANDLE hToken = NULL;
	HANDLE hProcess = NULL;
	//현재 프로세스의 토큰을 얻는다.
	hProcess = ::OpenProcess(
		PROCESS_QUERY_INFORMATION, TRUE,
		::GetCurrentProcessId());
	::OpenProcessToken(hProcess,
		TOKEN_ALL_ACCESS, &hToken);

	//얻은 토큰에서 객체 소유자의 SID를 구한다.
	PTOKEN_USER pTokenUser = NULL;
	DWORD dwLength = 0;
	::GetTokenInformation(hToken,
		TokenUser, (LPVOID)pTokenUser, 0, &dwLength);
	pTokenUser = (PTOKEN_USER)HeapAlloc(
		::GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);

	if (::GetTokenInformation(hToken,
		TokenUser, pTokenUser, dwLength, &dwLength))
	{
		TCHAR* pszBuffer = NULL;
		//SID를 문자열로 바꾸어 출력한다.
		::ConvertSidToStringSid(pTokenUser->User.Sid, &pszBuffer);

		wprintf(L"SID: %s\n", pszBuffer);
		::LocalFree((HLOCAL)pszBuffer);
	}
	else
	{
		wprintf(L"ERROR NUMBER: %d", ::GetLastError());
	}

	::CloseHandle(hToken);
	::CloseHandle(hProcess);
	::HeapFree(::GetProcessHeap(), 0, (LPVOID)pTokenUser);
}

int main()
{
	//CreateSample01();
	//CreateSample02();
	PrintSid();
}

/*
* CreateProcess() 함수를 보면 다음과 같은 매개변수들이 있다.
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	이 외에도 위 코드들과 같이 매개변수에 null을 넣겠다는 건,
	CreateProcess()를 실행하는 프로세스를 부모 프로세스로 하고 그것을 상속받겠다는 의미.

	DALC 안에 ACL 리스트가 있고 ACL 안에 또 ACE라는 배열이 있다.
	그리고 ACE에는 SID(Security IDentifier)라는 사용자나 그룹의 식별자가 있다.
	결론: 관리자 SID가 털리면 큰일나는거임.
*/