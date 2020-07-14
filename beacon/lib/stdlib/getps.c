#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <wow64apiset.h>

#define MAX_NAME 256

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL GetLogonFromToken (HANDLE hToken, char* lpName, char* lpDomain) 
{
    DWORD dwLength  = 0;
    BOOL bSuccess   = FALSE;
    PTOKEN_USER ptu = NULL;
    DWORD dwSize    = MAX_NAME;

    if (NULL == hToken)
    {
        return FALSE;
    }

    if (!GetTokenInformation(hToken, TokenUser, (LPVOID) ptu, 0, &dwLength))
    {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            return FALSE;
        }

        ptu = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);

        if (ptu == NULL)
        {
            return FALSE;
        }
    }

    if (!GetTokenInformation(hToken, TokenUser, (LPVOID) ptu, dwLength, &dwLength))
    {
      return FALSE;
    }

    SID_NAME_USE SidType;

    if( !LookupAccountSid( NULL , ptu->User.Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType ) )
    {
        DWORD dwResult = GetLastError();

        if(dwResult == ERROR_NONE_MAPPED)
        {
            strcpy (lpName, "NONE_MAPPED" );
        } else
        {
            return FALSE;
        }
    } else
    {
        bSuccess = TRUE;
    }

   return bSuccess;
}

char* getps()
{
    PROCESSENTRY32 ProEntry;
    char* text = (char*)malloc(20000);
    HANDLE hProcSnap, hProcess, hToken;
    LPFN_ISWOW64PROCESS _IsWow64Process;

    _IsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if (!_IsWow64Process)
    {
        sprintf(text, "Resolving IsWow64Process(): FAILED\n", NULL);
        return text;
    }

    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (!hProcSnap)
    {
        sprintf(text, "CreateToolhelp32Snapshot(): FAILED\n", NULL);
        return text;
    }

    ProEntry.dwSize = sizeof(PROCESSENTRY32);

    sprintf(text, "PID\tPPID\tArch\tUser\t\t\t \t\tName\n", NULL);
    sprintf(text, "%s===\t====\t====\t====\t\t\t \t\t====\n\n", text);

    if(!Process32First(hProcSnap, &ProEntry))
    {
        sprintf(text, "%sProcess32First(): FAILED\n", text);
        return text;
    }

    do
    {
        char lpName[MAX_NAME];
        char lpDomain[MAX_NAME];

        BOOL  bWow64 = FALSE;
        CHAR* Arch   = "x64";
        hProcess     = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProEntry.th32ProcessID);

        // if we cant get a handle then we cant get as much info
        if(!hProcess)
        {
            sprintf(text, "%s%d\t%d\t   \t\t   \t\t  \t\t%s\n", text, ProEntry.th32ProcessID, ProEntry.th32ParentProcessID, ProEntry.szExeFile);
            continue;
        }

        if (!_IsWow64Process(hProcess, &bWow64))
        {
            continue;
        }

        if (bWow64)
        {
            Arch = "x86";
        }

        if( !OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
        {
            continue;
        }

        GetLogonFromToken(hToken, &lpName,  &lpDomain);

        if ((strlen(lpName) + strlen(lpDomain) + 1) <= 20)
        {
            sprintf(text, "%s%d\t%d\t%s\t%s\\%s\t\t\t%s\n", text, ProEntry.th32ProcessID, ProEntry.th32ParentProcessID, Arch, lpDomain, lpName, ProEntry.szExeFile);
            continue;
        } else {
            sprintf(text, "%s%d\t%d\t%s\t%s\\%s\t\t%s\n", text, ProEntry.th32ProcessID, ProEntry.th32ParentProcessID, Arch, lpDomain, lpName, ProEntry.szExeFile);
        }

    } while(Process32Next(hProcSnap, &ProEntry));


    return text;
}