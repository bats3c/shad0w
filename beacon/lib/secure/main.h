#pragma once
#include <windows.h>

// these are the only DLLs allowed into our process when running in secure mode,
// to allow a new DLL, simply add it to the 'AllowDlls' array and increment 'AllowDllCount' by 1
#define AllowDllCount 11
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
                                            "ntdll.dll"
                                          };

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef void (WINAPI * LdrLoadDll_) (PWSTR SearchPath OPTIONAL, 
                                     PULONG DllCharacteristics OPTIONAL, 
                                     PUNICODE_STRING DllName, 
                                     PVOID *BaseAddress);

NTSTATUS __stdcall _LdrLoadDll(PWSTR SearchPath OPTIONAL, PULONG DllCharacteristics OPTIONAL, PUNICODE_STRING DllName, PVOID *BaseAddress);

LPVOID lAddr;
char OriginalBytes[50] = {};