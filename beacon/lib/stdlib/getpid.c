#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

char* getpid()
{
    char* text = (char*)malloc(100 + 1);
    DWORD dwPid = GetCurrentProcessId();


    sprintf(text, "PID: %d\n", dwPid);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if(hSnapshot)
    {
        PROCESSENTRY32 pe32;
        if(Process32First(hSnapshot,&pe32))
        {
            do
            {
                if (pe32.th32ProcessID == dwPid)
                {
                    sprintf(text, "PID: %d\n", pe32.th32ProcessID);
                    sprintf(text, "%sImage: %s", text, pe32.szExeFile);
                    break;
                }
            } while(Process32Next(hSnapshot,&pe32));
            CloseHandle(hSnapshot);
        }
    }

    return text;
}