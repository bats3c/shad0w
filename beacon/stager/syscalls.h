#include <winternl.h>

/******************************************/
/* Windows 10                             */
/******************************************/
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