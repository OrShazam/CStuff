#include <windows.h>
// tiny POC demonstrating a way for a process to protect itself
// from basic Process Injection 
// (Won't work against methods that spawn their victim process (process hollowing))
// (or methods that suspended and use NtQueueApcThread)
// TODO: also hook LdrLoadDll to prevent Dll Injection

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
	DWORD imageMaxAddr = info.BaseAddress + info.RegionSize;
	DWORD imageMinAddr = info.AllocationBase;
	infoRead = VirtualQuery(aStartAddress,&info, sizeof(info));
	if (infoRead != sizeof(info)){
		goto BAD;
	}
	DWORD lpFuncMaxAddr = info.BaseAddress + info.RegionSize;
	if ((lpFuncMaxAddr > imageMaxAddr) || (info.AllocationBase < imageMinAddr)){
		// if function space belongs to another allocation
		// it's probably a malicious functions, but perhaps it's not always true
		goto BAD;	
	}
	//NOTE: 
	//I'm not a memory expert so i've been extra careful with the check
	//but if the image base is always the allocation base 
	// then it's sufficient to do
	// VirtualQuery(aStartAddress,&info...); if (info.AllocationBase != imageBase)
	
	goto CALL; 
	BAD:
	aStartAddress = 0x0; // make a crash
	goto CALL;
	
	CALL:
	WriteProcessMemory(
		GetCurrentProcess(),
		procPtr,
		keep,
		...);
	_BaseThreadInitChunk(aIsInitialThread,aStartAddress, aThreadParam);
	// and set again for next call;
	WriteProcessMemory(
		GetCurrentProcess(),
		procPtr,
		hook,
		...);

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
