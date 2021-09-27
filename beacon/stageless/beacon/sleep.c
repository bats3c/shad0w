#include "pch.h"
#include "api.h"
#include "beacon.h"

API g_Api;
PROFILE g_Profile;

VOID BeaconSleep()
{
    if (g_Profile.bCleanSleep)
    {
        // restore all hooks and patches
    }

    if (g_Profile.bEncryptSleep)
    {
        // suspend all threads (apart from this one yet)
        // encrypt tracked allocations both heap and virtual
        // TODO: module as well (ROP or sumthin)
    }

    if (g_Profile.bSyscalls)
    {
        // NtDelayExecution
    } else
    {
        Sleep(g_Profile.dwSleepTime * 1000);
    }

    if (g_Profile.bEncryptSleep)
    {
        // decrypt all tracked allocations
        // resume all threads
    }

    if (g_Profile.bCleanSleep)
    {
        // remove all hooks and patches placed
    }

    return;
}