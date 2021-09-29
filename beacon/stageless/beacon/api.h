#pragma once

typedef LPVOID (WINAPI * API_RtlGetVersion)
(
    POSVERSIONINFOEXW OsInfo
);

typedef LPVOID (WINAPI * API_HeapFree)
(
    HANDLE hHeap,
    DWORD  dwFlags,
    LPVOID lpMem
);

typedef LPVOID (WINAPI * API_HeapAlloc)
(
    HANDLE hHeap,
    DWORD  dwFlags,
    SIZE_T dwBytes
);

typedef LPVOID (WINAPI * API_WinHttpOpen)
(
    LPCWSTR pszAgentW,
    DWORD   dwAccessType,
    LPCWSTR pszProxyW,
    LPCWSTR pszProxyBypassW,
    DWORD   dwFlags
);

typedef LPVOID (WINAPI * API_WinHttpSetOption)
(
    HINTERNET hInternet,
    DWORD     dwOption,
    LPVOID    lpBuffer,
    DWORD     dwBufferLength
);

typedef LPVOID (WINAPI * API_WinHttpConnect)
(
    HINTERNET     hSession,
    LPCWSTR       pswzServerName,
    INTERNET_PORT nServerPort,
    DWORD         dwReserved
);

typedef LPVOID (WINAPI * API_WinHttpSendRequest)
(
    HINTERNET hRequest,
    LPCWSTR   lpszHeaders,
    DWORD     dwHeadersLength,
    LPVOID    lpOptional,
    DWORD     dwOptionalLength,
    DWORD     dwTotalLength,
    DWORD_PTR dwContext
);

typedef LPVOID (WINAPI * API_WinHttpCloseHandle)
(
    HINTERNET hInternet
);

typedef LPVOID (WINAPI * API_WinHttpOpenRequest)
(
    HINTERNET hConnect,
    LPCWSTR   pwszVerb,
    LPCWSTR   pwszObjectName,
    LPCWSTR   pwszVersion,
    LPCWSTR   pwszReferrer,
    LPCWSTR   *ppwszAcceptTypes,
    DWORD     dwFlags
);

typedef LPVOID (WINAPI * API_WinHttpAddRequestHeaders)
(
    HINTERNET hRequest,
    LPCWSTR   lpszHeaders,
    DWORD     dwHeadersLength,
    DWORD     dwModifiers
);

typedef LPVOID (WINAPI * API_WinHttpQueryDataAvailable)
(
    HINTERNET hRequest,
    LPDWORD   lpdwNumberOfBytesAvailable
);

typedef LPVOID (WINAPI * API_WinHttpReadData)
(
    HINTERNET hRequest,
    LPVOID    lpBuffer,
    DWORD     dwNumberOfBytesToRead,
    LPDWORD   lpdwNumberOfBytesRead
);

typedef LPVOID (WINAPI * API_WinHttpReceiveResponse)
(
    HINTERNET hRequest,
    LPVOID    lpReserved
);

typedef BOOL (WINAPI * API_WinHttpSetTimeouts)
(
    HINTERNET hInternet,
    int       nResolveTimeout,
    int       nConnectTimeout,
    int       nSendTimeout,
    int       nReceiveTimeout
);

typedef struct _API
{
    API_HeapFree HeapFree;
    API_HeapAlloc HeapAlloc;
    API_WinHttpOpen WinHttpOpen;
    API_RtlGetVersion RtlGetVersion;
    API_WinHttpConnect WinHttpConnect;
    API_WinHttpReadData WinHttpReadData;
    API_WinHttpSetOption WinHttpSetOption;
    API_WinHttpSendRequest WinHttpSendRequest;
    API_WinHttpCloseHandle WinHttpCloseHandle;
    API_WinHttpOpenRequest WinHttpOpenRequest;
    API_WinHttpSetTimeouts WinHttpSetTimeouts;
    API_WinHttpReceiveResponse WinHttpReceiveResponse;
    API_WinHttpAddRequestHeaders WinHttpAddRequestHeaders;
    API_WinHttpQueryDataAvailable WinHttpQueryDataAvailable;
} API, *PAPI;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// typedef struct _LIST_ENTRY2 {
//   struct _LIST_ENTRY2 *Flink;
//   struct _LIST_ENTRY2 *Blink;
// } LIST_ENTRY2, *PLIST_ENTRY2, PRLIST_ENTRY2;

typedef struct _PEB_LDR_DATA {
  BYTE       Reserved1[8];
  PVOID      Reserved2[3];
  LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    UCHAR           Reserved1[16];
    PVOID           Reserved2[10];
    UNICODE_STRING  ImagePathName;
    UNICODE_STRING  CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct
        {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB {
    BYTE                          Reserved1[2];
    BYTE                          BeingDebugged;
    BYTE                          Reserved2[1];
    PVOID                         Reserved3[2];
    PPEB_LDR_DATA                 Ldr;
    PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
    BYTE                          Reserved4[104];
    PVOID                         Reserved5[52];
    PVOID                         PostProcessInitRoutine;
    BYTE                          Reserved6[128];
    PVOID                         Reserved7[1];
    ULONG                         SessionId;
} PEB, *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID ExitStatus;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation
} PROCESSINFOCLASS;

typedef struct _MODULE_ENTRY {
    UNICODE_STRING BaseName;
    UNICODE_STRING FullName;
    ULONG SizeOfImage;
    PVOID BaseAddress;
    PVOID EntryPoint;
} MODULE_ENTRY, *PMODULE_ENTRY;

typedef struct _MODULE_INFORMATION_TABLE {
    ULONG Pid;
    ULONG ModuleCount;
    PMODULE_ENTRY Modules;
} MODULE_INFORMATION_TABLE, *PMODULE_INFORMATION_TABLE;

BOOL ResolveImports( PAPI g_Api );