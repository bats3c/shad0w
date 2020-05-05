#include <windows.h>
#include <winternl.h>
#include <versionhelpers.h>

// syscall definitions
// #include "syscalls.h"

NTSTATUS GetWriteVirtualMemory() 
{
    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    typedef void (WINAPI * RtlGetVersion_) (OSVERSIONINFOEXW *);
    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");

    RtlGetVersion(&osInfo);
    printf("NT Version: %d.%d\n", osInfo.dwMajorVersion, osInfo.dwMinorVersion);

    LPVOID rBuffer = NULL;
    SIZE_T Size;
    char* Bytes;
    HANDLE hProcess2;

    hProcess2 = hProcess;
    rBuffer = lpBaseAddress;
    Bytes = lpBuffer;
    Size = NumberOfBytesToRead;

    printf("NtWriteVirtualMemory10: 0x%x, 0x%p, 0x%p, %d\n", hProcess2, rBuffer, Bytes, Size);
    NtWriteVirtualMemory10(hProcess2, rBuffer, Bytes, Size, NULL);

    return 0;
}