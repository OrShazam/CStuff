#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef unsigned char byte;

void RC4Encrypt(byte* key, int keylen, byte* data, int datalen);
 
void RC4Decrypt(byte* key, int keylen, byte* data, int datalen);

void RC4InitState(byte* S, byte* key, int keylen);

byte RC4GenerateRandomByte(byte* S, int* i, int* j);

int main(int argc, char* argv[]) {
	if (argc != 2){
		return 0;
	}
	byte key[16];
	int i;
	printf("Random key: ");
	srand(time(NULL));
	for (i = 0; i < 16; i++){
		key[i] = rand() % 256;
		printf("%02x ", key[i]);
	}
	printf("\n");
	byte S[256];
	printf("Encrypting message: %s\n", argv[1]);
	int messageLen = strlen(argv[1]);
	byte* message = (byte*)argv[1];
	RC4Encrypt(key, 16, message, messageLen);
	printf("Encrypted message: ");
	for (i = 0; i < messageLen; i++){
		printf("%02x ", message[i]);
	}
	return 0;
	
}


void RC4Encrypt(byte* key, int keylen, byte* data, int datalen){
	byte S[256];
	RC4InitState(&S, key, keylen);
	int i = 0; int j = 0;
	for (int k = 0; k < datalen; k++){
		data[k] = data[k] ^ RC4GenerateRandomByte(&S, &i, &j);
	}
}

void RC4InitState(byte* S, byte* key, int keylen){
	int i;
	for (i = 0; i < 256; i++){
		S[i] = (byte)i;
	}
	int j = 0;
	byte temp;
	for (i = 0; i < 256; i++){
		j = (j + S[i] + key[i % keylen]) % 256;
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
	}
	
}
byte RC4GenerateRandomByte(byte* S, int* i, int* j){
	byte temp;
	*i = (*i + 1) % 256;
	*j = (*j + S[*i]) % 256;
	temp = S[*i];
	S[*i] = S[*j];
	S[*j] = temp;
	return (byte)(S[( S[*i] + S[*j] ) % 256]);
}


void RC4Decrypt(byte* key, int keylen, byte* data, int datalen){
	return RC4Encrypt(key, keylen, data, datalen);
}
