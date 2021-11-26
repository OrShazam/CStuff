#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
typedef uint16_t ushort;

typedef struct _UNICODE_STRING {
	ushort Length;
	ushort MaxLength;
	wchar_t* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

int _RtlInitUnicodeString(wchar_t* buffer, PUNICODE_STRING str){
	str->Length = wcslen(buffer) * 2;
	str->MaxLength = str->Length;
	str->Buffer = buffer;
	return 0;
}
int _RtlCopyUnicodeString(PUNICODE_STRING dst, PUNICODE_STRING src){
	if (dst->MaxLength < src->Length){
		return 1;
	}
	wcsncpy(dst->Buffer, src->Buffer, src->Length / 2);
	dst->Length = src->Length;
	return 0;
}
int _RtlCompareUnicodeString(PUNICODE_STRING str1, PUNICODE_STRING str2, bool CaseInSensitive){
	wchar_t* buf1, buf2;
	if (CaseInSensitive){
		buf1 = malloc(str1->Length);
		buf2 = malloc(str2->Length);
		wcsnlwr(str1->Buffer,buf1, str1->Length);
		wcsnlwr(str2->Buffer,buf2, str2->Length);
	}
	else {
		buf1 = str1->Buffer;
		buf2 = str2->Buffer;
	}
	int minLength = (str1->Length > str2->Length) ? str2->Length : str1->Length;
	return wcsncmp(buf1,buf2,minLength);
}
int _RtlEqualUnicodeString(PUNICODE_STRING str1, PUNICODE_STRING str2, bool CaseInSensitive){
	
	return _RtlCompareUnicodeString(str1,str2,CaseInSensitive) == 0;
}


int _RtlAppendUnicodeStringToString(PUNICODE_STRING dst, PUNICODE_STRING src){
	if (dst->MaxLength < dst->Length + src->Length){
		return 1;
	}
	wcsncpy(dst->Buffer + dst->Length, src->Buffer, src->Length);
	return 0;
}
int _RtlAppendUnicodeToString(wchar_t* dst, PUNICODE_STRING src){
	wcsncpy(dst, src->Buffer, src->Length);
	return 0;
}