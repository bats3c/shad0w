#include <winternl.h>

/******************************************/
/* Windows 10                             */
/******************************************/
EXTERN_C NTSTATUS NtWriteVirtualMemory10(
    HANDLE hProcess, 
    PVOID lpBaseAddress, 
    PVOID lpBuffer, 
    SIZE_T NumberOfBytesToRead, 
    PSIZE_T NumberOfBytesRead
);

EXTERN_C NTSTATUS NtQueueApcThread10(
    HANDLE ThreadHandle,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2 
);

EXTERN_C NTSTATUS NtAllocateVirtualMemory10(
    HANDLE ProcessHandle, 
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

EXTERN_C NTSTATUS NtProtectVirtualMemory10(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    PSIZE_T NumberOfBytesToProtect,
    ULONG NewAccessProtection,
    PULONG OldAccessProtection
);
/******************************************/

/******************************************/
/* Windows 8.1                            */
/******************************************/
EXTERN_C NTSTATUS NtWriteVirtualMemory81(
    HANDLE hProcess, 
    PVOID lpBaseAddress, 
    PVOID lpBuffer, 
    SIZE_T NumberOfBytesToRead, 
    PSIZE_T NumberOfBytesRead
);

EXTERN_C NTSTATUS NtQueueApcThread81(
    HANDLE ThreadHandle,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2 
);

EXTERN_C NTSTATUS NtAllocateVirtualMemory81(
    HANDLE ProcessHandle, 
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

EXTERN_C NTSTATUS NtProtectVirtualMemory81(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    PSIZE_T NumberOfBytesToProtect,
    ULONG NewAccessProtection,
    PULONG OldAccessProtection
);
/******************************************/