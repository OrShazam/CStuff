#include <ntddk.h>
// tiny POC 
// a driver to mitigate CreateRemoteThread
void RemoteGuardUnload(PDRIVER_OBJECT);
void CreateThreadNotify(HANDLE, HANDLE, BOOLEAN);
extern "C" NTSTATUS
DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DriverObject->DriverUnload = RemoteGuardUnload;
	PsSetCreateThreadNotifyRoutine(CreateThreadNotify);
}

void CreateThreadNotify(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create) {
	if (PsGetCurrentProcessId() != ProcessId) {
		// if target process is not calling process
		// it's a remote thread
		Create = false;
	}
	
}

void RemoteGuardUnload(PDRIVER_OBJECT DriverObject) {
	PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify);
}