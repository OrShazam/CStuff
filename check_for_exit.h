#include <windows.h>
// different ways to check if a process exited 
// might be useful for obfuscation?

int check_for_exit1(DWORD dwProcessId){
	int returnVal;
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE,dwProcessId);
	DWORD waitVal = WaitForSingleObject(hProcess,0);
	if (waitVal == WAIT_OBJECT_0){
		returnVal = 1;
	}
	else { returnVal = 0;}
	CloseHandle(hProcess);
	return returnVal;
}
int check_for_exit2(DWORD dwProcessId){
	int returnVal;
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE,dwProcessId);
	DWORD waitVal = WaitForMultipleObjects(1,(CONST HANDLE *)&hProcess, TRUE,0);
	if (waitVal == WAIT_OBJECT_0){
		returnVal = 1;
	}
	else { returnVal = 0;}
	CloseHandle(hProcess);
	return returnVal;
	
}
int check_for_exit3(DWORD dwProcessId){
	int returnVal;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,dwProcessId);
	DWORD exitCode;
	GetExitCodeProcess(hProcess,(LPDWORD)&exitCode);
	if (exitCode == STILL_ACTIVE){
		returnVal = 0;
	}
	else { returnVal = 1;}
	CloseHandle(hProcess);
	return returnVal;
}
int check_for_exit4(DWORD dwProcessId){
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	if (hProcess == NULL){
		return 0;
	}
	CloseHandle(hProcess);
	return 1;
}
