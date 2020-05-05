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
/******************************************/