#include <string.h>
#include <windows.h>
#define MAX_PATH 260

//LoadLibrary implementation


HMODULE LoadLibrary(const char* libName){
	// todo: enumerate PEB to see if library is already loaded
	char libPath[MAX_PATH];
	strcpy(libPath,"C:/Windows/System32/");
	strcat(libPath, libName);
	PBYTE fileBuffer = LoadFile(libPath);
	if(fileBuffer == NULL)
		return NULL;
	
	bool applyReloc = false;
	PIMAGE_NT_HEADERS libNtHeader = (PIMAGE_NT_HEADERS)(fileBuffer +
		((IMAGE_DOS_HEADER)fileBuffer)->e_lfanew);	
	size_t imageSize = libNtHeader->OptionalHeader.SizeOfImage;
	ULONGLONG imageDesiredBase = libNtHeader->OptionalHeader.ImageBase;
	
	PBYTE buffer = AllocBuffer(imageSize, PAGE_EXECUTE_READWRITE, imageDesiredBase);
	if (buffer != imageDesiredBase)
		applyReloc = true;
	if (buffer == NULL)
		return NULL;
	
	int numSections = libNtHeader->FileHeader.NumberOfSections;
	
	size_t headerSize = sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)
		+ numSections * sizeof(IMAGE_SECTION_HEADER);
	
	memcpy(buffer,fileBuffer, headerSize);
	
	PIMAGE_SECTION_HEADER firstSection = (PIMAGE_SECTION_HEADER)
		(fileBuffer + sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS));
	
	for (int i = 0; i < numSections; i++){
		PIMAGE_SECTION_HEADER currSection = firstSection + i * sizeof(IMAGE_SECTION_HEADER);
		memcpy(buffer + currSection->VirtualAddress, fileBuffer 
			+ currSection->PointerToRawData, SizeOfRawData);
		
		if (applyReloc && (currSection->PointerToRelocations != NULL)){
			PIMAGE_BASE_RELOCATION relocData = fileBuffer +
				currSection->PointerToRelocations;
			int relocDataCount = currSection->NumberOfRelocations;
			for (int j = 0; j < relocDataCount;j++){
				
				int entries = (relocData->SizeOfBlock -
					sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) + 1;
				
				for (int k = 0; k < entries; k++){
					WORD* currEntry = (WORD*)(relocData + sizeof(DWORD) * 2 
						+ sizeof(WORD) * k);
						
					DWORD* address = relocData->VirtualAddress + *currEntry;
					*address += (buffer - imageDesiredBase);
				}
				
				relocData += relocData->SizeOfBlock;	
			}
		}
		
	}
	DWORD oldProtect;
	VirtualProtect(buffer,SizeOfImage,PAGE_EXECUTE_READ, &oldProtect);
	// read protection for GetProcAddress
	return (HMODULE)buffer;
	
}
// stephen fewer (https://github.com/stephenfewer) got an amazing implementation 
// for GetProcAddress if you might be interested 
PBYTE LoadFile(LPSTR filename,LPVOID desiredBase = NULL)
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
	HANDLE mapping = CreateFileMapping(hFile, 0,
		PAGE_READONLY, 0, 0, 0);
	if (!mapping)
		goto END;
	rawData = (PBYTE)MapViewOfFile(mapping, FILE_MAP_READ,
		0, 0, 0);
	if (!rawData)
		goto END;

	fileSize = GetFileSize(hFile, 0);
	address = AllocBuffer(toLoad, PAGE_READWRITE, desiredBase);
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
