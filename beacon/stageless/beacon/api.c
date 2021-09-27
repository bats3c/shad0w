#include "pch.h"
#include "api.h"

HMODULE BeaconGetModuleHandle(
  LPWSTR lpwModuleName
)
{
    #ifdef _M_IX86
        #define PEB_OFFSET 0x30
        #define READ_MEMLOC __readfsdword
    #endif

    #ifdef _M_AMD64
        #define PEB_OFFSET 0x60
        #define READ_MEMLOC __readgsqword
    #endif

    PPEB pPeb = (PPEB)READ_MEMLOC( PEB_OFFSET );

    PLIST_ENTRY pModListEnd = &pPeb->Ldr->InMemoryOrderModuleList;
    PLIST_ENTRY pModList = pModListEnd->Flink;

    while ( pModList != pModListEnd )
    {
        UCHAR* ucModPtrOff = (UCHAR*)pModList - (sizeof(LIST_ENTRY));

        PLDR_DATA_TABLE_ENTRY pLdrTbl = (PLDR_DATA_TABLE_ENTRY)ucModPtrOff;

        if (!_wcsicmp( pLdrTbl->BaseDllName.Buffer, (PWSTR)lpwModuleName ))
        {
            return (HMODULE)pLdrTbl->DllBase;
        }

        pModList = pModList->Flink;
    }

    return NULL;
}

BOOL ResolveImports(
    PAPI g_Api
)
{
    BOOL bSuccess = TRUE;

    HMODULE hKernel32 = BeaconGetModuleHandle((LPWSTR)L"kernel32.dll");
    HMODULE hWinHTTP = LoadLibraryW(L"winhttp.dll");

    if (!hKernel32 || !hWinHTTP)
    {
        return FALSE;
    }

    g_Api->HeapFree = (API_HeapFree)GetProcAddress(hKernel32, "HeapFree");
    g_Api->HeapAlloc = (API_HeapAlloc)GetProcAddress(hKernel32, "HeapAlloc");

    g_Api->WinHttpOpen = (API_WinHttpOpen)GetProcAddress(hWinHTTP, "WinHttpOpen");
    g_Api->WinHttpConnect = (API_WinHttpConnect)GetProcAddress(hWinHTTP, "WinHttpConnect");
    g_Api->WinHttpReadData = (API_WinHttpReadData)GetProcAddress(hWinHTTP, "WinHttpReadData");
    g_Api->WinHttpSetOption = (API_WinHttpSetOption)GetProcAddress(hWinHTTP, "WinHttpSetOption");
    g_Api->WinHttpSendRequest = (API_WinHttpSendRequest)GetProcAddress(hWinHTTP, "WinHttpSendRequest");
    g_Api->WinHttpCloseHandle = (API_WinHttpCloseHandle)GetProcAddress(hWinHTTP, "WinHttpCloseHandle");
    g_Api->WinHttpOpenRequest = (API_WinHttpOpenRequest)GetProcAddress(hWinHTTP, "WinHttpOpenRequest");
    g_Api->WinHttpSetTimeouts = (API_WinHttpSetTimeouts)GetProcAddress(hWinHTTP, "WinHttpSetTimeouts");
    g_Api->WinHttpReceiveResponse = (API_WinHttpReceiveResponse)GetProcAddress(hWinHTTP, "WinHttpReceiveResponse");
    g_Api->WinHttpAddRequestHeaders = (API_WinHttpAddRequestHeaders)GetProcAddress(hWinHTTP, "WinHttpAddRequestHeaders");
    g_Api->WinHttpQueryDataAvailable = (API_WinHttpQueryDataAvailable)GetProcAddress(hWinHTTP, "WinHttpQueryDataAvailable");

    return TRUE;
}