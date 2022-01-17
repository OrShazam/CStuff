#include <windows.h>
// tiny POC demonstrating a way for a process to protect itself
// from basic Process Injection 
// (Won't work against methods that spawn their victim process (process hollowing))
// (or methods that suspended and use NtQueueApcThread)
// TODO: also hook LdrLoadDll to prevent Dll Injection

#define JMP_RCX 0xffe1

BYTE keep[5];
BYTE hook[5];
FARPROC procPtr;
DWORD (WINAPI *_BaseThreadInitChunk)(
		IN BOOL aIsInitialThread,
		void* aStartAddress,
		void* aThreadParam
	) = NULL;
	
DWORD GetMyImageBase(){
	PPEB peb = (PPEB)__readfsdword(0x30); 
	PLDR_DATA_TABLE_ENTRY pLdrDataEntry = (PLDR_DATA_TABLE_ENTRY)((PBYTE)pCurrentPeb->LoaderData->InMemoryOrderModuleList.Flink);
	// first entry in Ldr belongs to image of running process
	return pLdrDataEntry->DllBase;
}
static DWORD WINAPI NopFunc(LPVOID lpParameter){
	return 0;
}

void WINAPI ThreadCheck(BOOL aIsInitialThread, void* aStartAddress, void* aThreadParam){
	// need to actually load the dlls for the functions im using 
	if (aIsInitialThread)
		goto CALL;
	
	DWORD imageBase = GetMyImageBase();
	MEMORY_BASIC_INFORMATION info;
	SIZE_T infoRead = VirtualQuery(imageBase,&info, sizeof(info));
	if (infoRead != sizeof(info)){
		goto BAD;
	}
	DWORD imageAllocBase = info.AllocationBase;
	infoRead = VirtualQuery(aStartAddress,&info, sizeof(info));
	if (infoRead != sizeof(info)){
		goto BAD;
	}
	if (info.Type != MEM_IMAGE || info.Protect & PAGE_EXECUTE_READWRITE == PAGE_EXECUTE_READWRITE){
		goto BAD;
	}
	if (*(WORD*)aStartAddress == JMP_RCX){ // imagine my code is actually compatible with x64
		goto BAD;
	}
	DWORD lpFuncAllocBase= info.AllocationBase
	if (lpFuncAllocBase != imageAllocBase){
		// if function space belongs to another allocation
		// it's probably a malicious functions, but perhaps it's not always true
		goto BAD;	
	}
	
	goto CALL; 
	BAD:
	aStartAddress = NopFunc; 
	goto CALL;
	
	CALL:
	WriteProcessMemory(
		GetCurrentProcess(),
		procPtr,
		keep,
		5,
		&written);
	_BaseThreadInitChunk(aIsInitialThread,aStartAddress, aThreadParam);
	// and set again for next call;
	WriteProcessMemory(
		GetCurrentProcess(),
		procPtr,
		hook,
		5,
		&written);

}


int main(){
	SIZE_T read;
	SIZE_T written;
	HMODULE hNtdll = LoadLibrary("kernel32.dll");
	if (hNtdll == NULL){
		return;
	}
	procPtr = GetProcAddress(hNtdll,"BaseThreadInitThunk");
	_BaseThreadInitChunk = (DWORD(WINAPI*)(
		BOOL, 
		void*,
		void*))procPtr;
	DWORD address = &ThreadCheck;
	hook[0] = 0xE8;
	*(DWORD*)&hook[1] = address
	ReadProcessMemory(
		GetCurrentProcess(),
		procPtr,
		keep,
		5,
		&read);
	WriteProcessMemory(
		GetCurrentProcess(),
		procPtr,
		hook,
		5,
		&written);
	//...
	return 0;
	
	
}
