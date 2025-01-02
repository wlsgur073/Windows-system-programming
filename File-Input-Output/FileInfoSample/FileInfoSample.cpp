#include <iostream>
#include <windows.h>
#include <stdio.h>

void LoadDiskInfo(const TCHAR* pszPath)
{
	TCHAR szVolName[MAX_PATH] = { 0 }; // Symbolic link  상수를 이용한 MAX_PATH
	TCHAR szFileSys[MAX_PATH] = { 0 };
	TCHAR szRoot[MAX_PATH] = { 0 };
	DWORD dwSerialNum = 0, dwMaxCompLen = 0, dwSysFlag = 0;

	memcpy(szRoot, pszPath, sizeof(TCHAR) * 3); // "C:\" 문자열 복사, 하드코딩

	::GetVolumeInformation(szRoot, szVolName, MAX_PATH, &dwSerialNum,
		&dwMaxCompLen, &dwSysFlag, szFileSys, MAX_PATH);

	wprintf(TEXT("Volume name : %s, File system : %s\n"), szVolName, szFileSys);

	// ULARGE_INTEGER: 64비트 정수형, win32 API에서 사용
	ULARGE_INTEGER llAvailableSpace = { 0 };
	ULARGE_INTEGER llTotalSpace = { 0 };
	ULARGE_INTEGER llFreeSpace = { 0 };

	if (::GetDiskFreeSpaceEx(szRoot,
		&llAvailableSpace, &llTotalSpace, &llFreeSpace))
	{
		wprintf(TEXT(" (Disk free space: %I64u/%I64u GB)\n"),
			llFreeSpace.QuadPart / (1024 * 1024 * 1024),
			llTotalSpace.QuadPart / (1024 * 1024 * 1024) );
	}
}

void LoadFileList(const TCHAR* pszPath)
{
	//*.exe: 실행파일만 보기
	//a*.txt: a로 시작하는 텍스트 파일만 보기
	//*.* : 모든 파일 보기
	TCHAR szPath[MAX_PATH] = {0};
	wsprintf(szPath, TEXT("%s\\*.*"), pszPath);

	WIN32_FIND_DATA FindData;
	BOOL bResult = TRUE;
	::ZeroMemory(&FindData, sizeof(WIN32_FIND_DATA));

	HANDLE hFirstFile = ::FindFirstFile(
		(LPTSTR)(const TCHAR*)szPath, &FindData);
	while (bResult)
	{
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			wprintf(TEXT("[DIR] %s\n"), FindData.cFileName);
		else
			wprintf(TEXT("%s\n"), FindData.cFileName);

		bResult = ::FindNextFile(hFirstFile, &FindData); // 루프문을 통해 파일을 하나씩 읽어옴
	}
}

int main()
{
	::_wsetlocale(LC_ALL, TEXT("korean")); // set locale to korean

	LoadDiskInfo(TEXT("C:\\"));
	LoadFileList(TEXT("C:\\"));

	TCHAR szCurDir[MAX_PATH] = { 0 };
	::GetCurrentDirectory(MAX_PATH, szCurDir);
	wprintf(TEXT("Current Directory: %s\n"), szCurDir);
}
