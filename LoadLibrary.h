#include <string.h>
#include <windows.h>
#include <stdbool.h>
#define MAX_PATH 260
#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
//LoadLibrary implementation

PBYTE LoadFile(LPSTR filename,LPVOID desiredBase);
PBYTE AllocBuffer(size_t bufferSize, DWORD protect, LPVOID desiredBase);

HMODULE LoadLibrary(const char* libName){
	char libPath[MAX_PATH];
	strcpy(libPath,"C:/Windows/System32/");
	strcat(libPath, libName);
	PBYTE fileBuffer = LoadFile(libPath,NULL);
	if(fileBuffer == NULL)
		return NULL;
	
	bool applyReloc = false;
	PIMAGE_NT_HEADERS libNtHeader = (PIMAGE_NT_HEADERS)(fileBuffer +
		((PIMAGE_DOS_HEADER)fileBuffer)->e_lfanew);	
	size_t imageSize = libNtHeader->OptionalHeader.SizeOfImage;
	ULONGLONG imageDesiredBase = libNtHeader->OptionalHeader.ImageBase;
	
	PBYTE buffer = AllocBuffer(imageSize, PAGE_EXECUTE_READWRITE, imageDesiredBase);
	if (buffer != imageDesiredBase)
		applyReloc = true;
	if (buffer == NULL)
		return NULL;
	
	int numSections = libNtHeader->FileHeader.NumberOfSections;
	
	size_t headerSize = ((PIMAGE_DOS_HEADER)fileBuffer)->e_lfanew + sizeof(DWORD)
		sizeof(IMAGE_FILE_HEADER) + libNtHeader->FileHeader.SizeOfOptionalHeader + numSections * sizeof(IMAGE_SECTION_HEADER);
	
	memcpy(buffer,fileBuffer, headerSize);
	
	PIMAGE_SECTION_HEADER firstSection = (PIMAGE_SECTION_HEADER)
		(fileBuffer + sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS));
	
	for (int i = 0; i < numSections; i++){
		PIMAGE_SECTION_HEADER currSection = firstSection + i * sizeof(IMAGE_SECTION_HEADER);
		memcpy(buffer + currSection->VirtualAddress, fileBuffer 
			+ currSection->PointerToRawData, currSection->SizeOfRawData);
		
		if (applyReloc){
			PIMAGE_BASE_RELOCATION relocData = (PIMAGE_BASE_RELOCATION)(fileBuffer +
				currSection->PointerToRelocations);
			int relocDataCount = currSection->NumberOfRelocations;
			for (int j = 0; j < relocDataCount;j++){
				
				int entries = (relocData->SizeOfBlock -
					sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) + 1;
				
				for (int k = 0; k < entries; k++){
					WORD* currEntry = (WORD*)((relocData + sizeof(DWORD) * 2 
						+ sizeof(WORD) * k) & 0XFFF) // exclude the type data
						
					LPVOID* address = (LPVOID*)( buffer + relocData->VirtualAddress + *currEntry);
					*address = (LPVOID)(*address + (DWORD)buffer - (DWORD)imageDesiredBase);
				}
				
				relocData += relocData->SizeOfBlock;	
			}
		}		
	}
	BOOL( APIENTRY* DllMain)(
		IN HMODULE hModule,
		IN DWORD Reason,
		IN LPVOID lpReserved
	) = NULL;
	DllMain = (BOOL(APIENTRY*)(HMODULE,DWORD,LPVOID))
		(buffer + libNtHeader->OptionalHeader.AddressOfEntryPoint);
	
	DllMain((HMODULE)buffer,DLL_PROCESS_ATTACH,NULL);
	DWORD oldProtect;
	VirtualProtect(buffer,imageSize,PAGE_EXECUTE_READ, &oldProtect);
	// read protection for GetProcAddress
	return (HMODULE)buffer;
}
BOOL FreeLibrary(HMODULE hModule){
	PIMAGE_NT_HEADERS libNtHeader = (PIMAGE_NT_HEADERS)(hModule +
		((PIMAGE_DOS_HEADER)hModule)->e_lfanew);
	
	BOOL( APIENTRY* DllMain)(
		IN HMODULE hModule,
		IN DWORD Reason,
		IN LPVOID lpReserved
	) = NULL;
	DllMain = (BOOL(APIENTRY*)(HMODULE,DWORD,LPVOID))
		(hModule + libNtHeader->OptionalHeader.AddressOfEntryPoint);
		
	DllMain((HMODULE)buffer,DLL_PROCESS_DETACH,NULL);
		
	VirtualFree(hModule,0,MEM_RELEASE);
}

// stephen fewer (https://github.com/stephenfewer) got an amazing implementation 
// for GetProcAddress if you might be interested 
PBYTE LoadFile(LPSTR filename,LPVOID desiredBase)
{
	PBYTE address;
	HANDLE hFile = NULL, mapping = NULL;
	bool error;
	PBYTE rawData = NULL;
	size_t fileSize;
	hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	mapping = CreateFileMapping(hFile, 0,
		PAGE_READONLY, 0, 0, 0);
	if (!mapping)
		goto END;
	rawData = (PBYTE)MapViewOfFile(mapping, FILE_MAP_READ,
		0, 0, 0);
	if (!rawData)
		goto END;

	fileSize = GetFileSize(hFile, 0);
	address = AllocBuffer(fileSize,PAGE_READWRITE, desiredBase);
	if (address != NULL) {
		memcpy(address, rawData, fileSize);
	}
END:
	if (hFile)
		CloseHandle(hFile);
	if (mapping)
		CloseHandle(mapping);
	if (rawData)
		UnmapViewOfFile(rawData);

	return address;

}
PBYTE AllocBuffer(size_t bufferSize, DWORD protect,
	LPVOID desiredBase)
{
	if (!bufferSize)
		return NULL;

	PBYTE buffer = (PBYTE)VirtualAlloc(desiredBase,
		bufferSize, MEM_COMMIT | MEM_RESERVE, protect);
	return buffer;
}
