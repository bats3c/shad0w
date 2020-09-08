#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#include "main.h"
#include "imports.h"
#include "../../build/strings.h"

NTSTATUS __stdcall _LdrLoadDll(PWSTR SearchPath OPTIONAL, PULONG DllCharacteristics OPTIONAL, PUNICODE_STRING DllName, PVOID *BaseAddress)
{
    BOOL Allowed = FALSE;
    DWORD bytesWritten;
    char name[MAX_PATH + 100];
    DWORD oldProtect, oldOldProtect;

    sprintf(name, "%S", DllName->Buffer);

    for (size_t i = 0; i < AllowDllCount; i++)
    {
        if (_stricmp(name, AllowDlls[i]) == 0)
        {
            Allowed = TRUE;

            VirtualProtect(lAddr, sizeof(OriginalBytes), PAGE_EXECUTE_READWRITE, &oldProtect);
            memcpy(lAddr, OriginalBytes, sizeof(OriginalBytes));
            VirtualProtect(lAddr, sizeof(OriginalBytes), oldProtect, &oldProtect);

            LdrLoadDll_ LdrLoadDll = (LdrLoadDll_)GetProcAddress(LoadLibrary("ntdll.dll"), decrypt_string(STRING_NTDLL_LOADDLL, STRING_NTDLL_LOADDLL_KEY));

            LdrLoadDll(SearchPath, DllCharacteristics, DllName, BaseAddress);

            HookLoadDll(lAddr);
        }

    }

    if (Allowed == FALSE)
    {
        DEBUG("Blocked: %s", name);
    }



    return TRUE;
}

VOID HookLoadDll(LPVOID lAddr)
{
    DWORD oldProtect, oldOldProtect;
    void *hLdrLoadDll = &_LdrLoadDll;

    // our trampoline
    unsigned char boing[] = { 0x49, 0xbb, 0xde, 0xad, 0xc0, 0xde, 0xde, 0xad, 0xc0, 0xde, 0x41, 0xff, 0xe3 };

    // add in the address of our hook
    *(void **)(boing + 2) = &_LdrLoadDll;

    // write the hook
    VirtualProtect(lAddr, 13, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(lAddr, boing, sizeof(boing));
    VirtualProtect(lAddr, 13, oldProtect, &oldProtect);

    return;
}

BOOL InitSecurity()
{
    // start by patching out LdrLoadDll, this will prevent prevent DLLs being mapped by the OS to our process.
    // this does not stop DLLs being manually mapped / bootstrapped into our process though. then move on to
    // setting DLL signing (just a precaution, they shouldnt load) and enabling ACG + some other fun stuff.
    PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY SigPolicy;

    DWORD bytesRead = 50;
    lAddr = (LPVOID)GetProcAddress(GetModuleHandle("ntdll.dll"), decrypt_string(STRING_NTDLL_LOADDLL, STRING_NTDLL_LOADDLL_KEY));

    // save the original bytes
    memcpy(OriginalBytes, lAddr, 50);

    HookLoadDll(lAddr);

    // zero out our structs
    ZeroMemory(&SigPolicy, sizeof(SigPolicy));

    // set our policys
    SigPolicy.MicrosoftSignedOnly = 1;

    SetProcessMitigationPolicy_ rSetProcessMitigationPolicy = (SetProcessMitigationPolicy_)GetProcAddress(LoadLibrary("kernel32.dll"), decrypt_string(STRING_KERNEL32_SPMP, STRING_KERNEL32_SPMP_KEY));

    if (!rSetProcessMitigationPolicy(ProcessSignaturePolicy, &SigPolicy, sizeof(SigPolicy)))
    {
		// we failed to set it, it means we are probly on a system that does not support it
        // we still have other security mitigations active tho so lets stay alive and rely on them
        DEBUG("SetProcessMitigationPolicy (ProcessSignaturePolicy) failed");

        // we know it wont work, so lets make sure we dont enable this in the future
        ProcessMitigationActive = FALSE;
	}

    return TRUE;
}