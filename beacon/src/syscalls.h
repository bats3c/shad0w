#include <windows.h>

#define SYSCALL_STUB_SIZE 23
#define NTDLL_PATH "C:\\Windows\\System32\\ntdll.dll"

typedef NTSTATUS (NTAPI * _NtAllocateVirtualMemory) (
    HANDLE    ProcessHandle,
    PVOID     *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T   RegionSize,
    ULONG     AllocationType,
    ULONG     Protect
);

typedef NTSTATUS (NTAPI * _NtProtectVirtualMemory) (
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    PSIZE_T NumberOfBytesToProtect,
    ULONG NewAccessProtection,
    PULONG OldAccessProtection
);

typedef NTSTATUS (NTAPI * _NtWriteVirtualMemory) (
    HANDLE ProcessHandle,
    PVOID  BaseAddress,
    PVOID  Buffer,
    ULONG  BufferSize,
    PULONG NumberOfBytesWritten
);

typedef NTSTATUS (NTAPI * _NtQueueApcThread) (
    HANDLE ThreadHandle,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
);


struct NtInfo
{
	PIMAGE_EXPORT_DIRECTORY pExprtDir;
	LPVOID					lpRawData;
	PIMAGE_SECTION_HEADER	pTextSection;
	PIMAGE_SECTION_HEADER	pRdataSection;
	CHAR	  				cSyscallStub;
};

struct Syscalls
{
	_NtAllocateVirtualMemory NtAllocateVirtualMemory;
    _NtProtectVirtualMemory  NtProtectVirtualMemory;
    _NtWriteVirtualMemory    NtWriteVirtualMemory;
    _NtQueueApcThread        NtQueueApcThread;

};

extern CHAR SyscallStub[SYSCALL_STUB_SIZE];