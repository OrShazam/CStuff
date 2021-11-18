#include <stdio.h>
#include <stdlib.h>
// objdump alternative for windows
static unsigned char* readFile(const char* path, size_t* fileSize){
	// Referenced from 'Crafting Interpreters'
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(-1);
	}
	fseek(file, 0L, SEEK_END);
	*fileSize = ftell(file);
	rewind(file);
    unsigned char* buffer = (unsigned char*)malloc(*fileSize + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(-1);
	}
	size_t bytesRead = fread(buffer, sizeof(unsigned char), *fileSize, file);
	 if (bytesRead < *fileSize) {
		fprintf(stderr, "Could not read file \"%s\".\n", path);
		exit(-1);
	}
	buffer[bytesRead] = '\0';
	fclose(file);
	return buffer;
}
static void dump(const unsigned char* buffer, const unsigned int length){
	// Referenced from 'Hacking: The Art Of Exploitation'
	unsigned char byte;
	unsigned int i,j;
	for (i = 0; i < length; i++){
		byte = buffer[i];
		printf("%02x ",byte);
		if((i % 16 == 15) || (i == length -1)){
			for (j = 0; j < 15 - (i%16);j++){
				printf("  ");
			}
			printf("| ");
			for (j = i - (i%16);j <= i; j++){
				byte = buffer[j];
				if ((byte > 31) && (byte < 127))
					printf("%c",byte);
				else 
					printf(".");
			}
			printf("\n");
		}		
	}
}


int main(int argc, char** argv){
	if (argc < 2){
		printf("Usage: objdump.exe <filepath>");
		return 0;
	}
	size_t toPrint;
	unsigned char* buffer = readFile(argv[1],&toPrint);
	dump(buffer,toPrint);
	return 0;
}
