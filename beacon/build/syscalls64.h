#include <winternl.h>

EXTERN_C NTSTATUS NtWriteVirtualMemory10(HANDLE hProcess, 
    PVOID lpBaseAddress, 
    PVOID lpBuffer, 
    SIZE_T NumberOfBytesToRead, 
    PSIZE_T NumberOfBytesRead
);

EXTERN_C NTSTATUS NtOpenProcess10(
    PHANDLE ProcessHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    CLIENT_ID* ClientId
);

EXTERN_C NTSTATUS NtProtectVirtualMemory10(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    SIZE_T* NumberOfBytesToProtect,
    ULONG NewAccessProtection,
    PULONG OldAccessProtection
);

EXTERN_C NTSTATUS NtAllocateVirtualMemory10(
    HANDLE ProcessHandle, 
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

EXTERN_C NTSTATUS NtQueueApcThread10(
    HANDLE ThreadHandle,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2 
);