#include <stdio.h>
#include <stdlib.h>
// strings implementation
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
static int isPrintable(char ch){
	if ((ch > 31) && (ch < 127))
		return 1;
	return 0;
}
static void searchForStrings(const unsigned char* buffer, size_t length){
	int seqLength;
	int i; int j;
	for (i = 0; i < length; i++){
		for (j = i; j < length; j++){
			if (!isPrintable(buffer[j]))
				break;
		}
		seqLength = j-i;
		if (seqLength >= 5 && buffer[j] == 0){
			printf("%s\n", &buffer[i]);
		}
		i += seqLength;
	}
}
int main(int argc, char** argv){
	if (argc < 2){
		printf("Usage: strings.exe <filepath>");
		return 0;
	}
	size_t toPrint;
	unsigned char* buffer = readFile(argv[1],&toPrint);
	searchForStrings(buffer,toPrint);
	free(buffer);
	return 0;
}
