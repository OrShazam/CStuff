#include <stdlib.h>
#include <windows.h>
// naive and inefficient mutex implementation

typedef struct _MUTEX_LIST_ENTRY MUTEX_LIST_ENTRY;
typedef MUTEX_LIST_ENTRY* PMUTEX_LIST_ENTRY;

typedef struct _WAIT_LIST_ENTRY WAIT_LIST_ENTRY;
typedef WAIT_LIST_ENTRY* PWAIT_LIST_ENTRY;

struct _MUTEX_LIST_ENTRY {
	int mutexId;
	PMUTEX_LIST_ENTRY next;
	PWAIT_LIST_ENTRY waitListHead;
	PWAIT_LIST_ENTRY waitListTail;
}; 

struct _WAIT_LIST_ENTRY {
	int threadId;
	PWAIT_LIST_ENTRY next;
};

int highestId = 0;
PMUTEX_LIST_ENTRY head = NULL;

int CreateMutex(int acquire){
	PMUTEX_LIST_ENTRY mutex = (PMUTEX_LIST_ENTRY)
		malloc(sizeof(MUTEX_LIST_ENTRY));
	if (mutex == NULL)
		return -1;
	mutex->mutexId = highestId++ << 1;
	mutex->waitListHead = NULL;
	mutex->waitListTail = NULL;
	if (!head){
		head = mutex;
	}
	else {
		mutex->next = head;
		head = mutex;
	}
	mutex->mutexId |= acquire; 
	return mutex->mutexId;
}
PMUTEX_LIST_ENTRY FindMutex(int mutexId){
	// a vector is much more efficient 
	// since this is just a POC I don't bother
	PMUTEX_LIST_ENTRY entry = head;
	while (entry != NULL){
		if (entry->id & -2 == mutexId)
			return entry;
		entry = entry->next;
	}
	return NULL;
}

int AcquireMutex(int mutexId){
	PMUTEX_LIST_ENTRY mutex = FindMutex(mutexId);
	if (mutex == NULL){
		return false;
	}
	if (mutex->mutexId & 1){
		PWAIT_LIST_ENTRY entry = (PWAIT_LIST_ENTRY)
			malloc(sizeof(WAIT_LIST_ENTRY));
		
		entry->threadId = GetCurrentThreadId();		
		entry->next = NULL;
		if (mutex->waitListTail == NULL){
			mutex->waitListTail = entry;
			mutex->waitListHead = entry;
		}
		else {
			mutex->waitListTail->next = entry;
			mutex->waitListTail = entry;
		}
		SuspendThread(-2);	
		return true;
	}
	else {
		mutex->mutexId |= 1;
		return true;
	}
}
int ReleaseMutex(int mutexId){
	PMUTEX_LIST_ENTRY mutex = FindMutex(mutexId);
	if (mutex == NULL){
		return false;
	}
	if (mutex->mutexId & 1){
		PWAIT_LIST_ENTRY entry = mutex->waitListHead;
		if (entry == NULL){
			mutexId &= -2;
			return true;
		}
		if (mutex->waitListTail == entry){
			mutex->waitListHead = NULL;
			mutex->waitListTail = NULL; 
		}
		else {
			mutex->waitListHead = entry->next;
		}
		ResumeThread(OpenThread(SYNCHRONIZE, FALSE, entry->threadId));
		free(entry);
	}
	return true;
}
int DeleteMutex(int mutexId){
	//resume all waiting threads 
	PMUTEX_LIST_ENTRY mutex = FindMutex(mutexId);
	if (mutex == NULL){
		return false;
	}
	PWAIT_LIST_ENTRY entry = mutex->waitListHead;
	PWAIT_LIST_ENTRY next;
	while (entry != NULL){
		ResumeThread(OpenThread(SYNCHRONIZE, FALSE, entry->threadId));
		next = entry->next;
		free(entry);
		entry = next;
	}
	PMUTEX_LIST_ENTRY currEntry = head;
	PMUTEX_LIST_ENTRY prevEntry = NULL;
	while (currEntry != mutex){
		prevEntry = currEntry;
		currEntry = currEntry->next;
	}
	if (prevEntry == NULL){
		head = head->next;
	}
	else {
		prevEntry->next = currEntry->next;
	}
	free(mutex);
	return true;
}

