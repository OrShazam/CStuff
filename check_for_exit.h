#include <windows.h>
// different ways to check if a process exited 
// might be useful for obfuscation?

int check_for_exit1(DWORD dwProcessId){
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE,dwProcessId);
	DWORD returnVal = WaitForSingleObject(hProcess,0);
	if (returnVal == WAIT_OBJECT_0){
		return 1;
	}
	return 0;
}
int check_for_exit2(DWORD dwProcessId){
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE,dwProcessId);
	DWORD returnVal = WaitForMultipleObjects(1,(CONST HANDLE *)&hProcess, TRUE,0);
	if (returnVal == WAIT_OBJECT_0){
		return 1;
	}
	return 0;
}
int check_for_exit3(DWORD dwProcessId){
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,dwProcessId);
	DWORD exitCode;
	GetExitCodeProcess(hProcess,(LPDWORD)&exitCode);
	if (exitCode == STILL_ACTIVE){
		return 0;
	}
	return 1;
}
int check_for_exit4(DWORD dwProcessId){
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, dwProcessId);
	if (hProcess == NULL){
		return 0;
	}
	return 1;
}
