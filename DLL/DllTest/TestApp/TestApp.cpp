#include <iostream>
#include <windows.h>

#pragma comment(lib, "..\\x64\\Debug\\DllTest.lib")
void WINAPI DllTestFunction(int nParam); // Forward declaration

int main()
{
	DllTestFunction(5);
}

/*
	Error: unresolved external symbol "void __cdecl DllTestFunction(int)" (?DllTestFunction@@YAXH@Z) referenced in function main
	Reason: The function DllTestFunction is declared but not defined. where is the definition of DllTestFunction?
	Solution: #param comment(lib, path) is used to link the DllTest.lib with the TestApp project.
	Caution: The path of the DllTest.lib should be correct.
*/