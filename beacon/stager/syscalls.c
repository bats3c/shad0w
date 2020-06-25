#include <windows.h>
#include <ntdef.h>
#include <tlhelp32.h>
#include <winternl.h>

#include "syscalls.h"

CHAR SyscallStub[SYSCALL_STUB_SIZE] = {};

PVOID RVAtoRawOffset(DWORD_PTR RVA, PIMAGE_SECTION_HEADER section)
{
	return (PVOID)(RVA - section->VirtualAddress + section->PointerToRawData);
}

BOOL MakeSyscall(LPCSTR functionName, PIMAGE_EXPORT_DIRECTORY exportDirectory, LPVOID fileData, PIMAGE_SECTION_HEADER textSection, PIMAGE_SECTION_HEADER rdataSection, LPVOID syscallStub)
{
    DWORD  dwOldProc             = 0;
	PDWORD pdwAddressOfNames     = (PDWORD)RVAtoRawOffset((DWORD_PTR)fileData + *(&exportDirectory->AddressOfNames), rdataSection);
	PDWORD pdwAddressOfFunctions = (PDWORD)RVAtoRawOffset((DWORD_PTR)fileData + *(&exportDirectory->AddressOfFunctions), rdataSection);
	BOOL   bStubFound            = FALSE;

	for (size_t i = 0; i < exportDirectory->NumberOfNames; i++)
	{
		DWORD_PTR functionNameVA    = (DWORD_PTR)RVAtoRawOffset((DWORD_PTR)fileData + pdwAddressOfNames[i], rdataSection);
		DWORD_PTR functionVA        = (DWORD_PTR)RVAtoRawOffset((DWORD_PTR)fileData + pdwAddressOfFunctions[i + 1], textSection);
		LPCSTR functionNameResolved = (LPCSTR)functionNameVA;

		if (strcmp(functionNameResolved, functionName) == 0)
		{
			memcpy((LPVOID)syscallStub, (LPVOID)functionVA, SYSCALL_STUB_SIZE);
            VirtualProtect((LPVOID)syscallStub, SYSCALL_STUB_SIZE, PAGE_EXECUTE_READWRITE, &dwOldProc);
			bStubFound = TRUE;
		}
	}

	return bStubFound;
}

VOID CleanSyscall(LPVOID syscallStub)
{
    DWORD dwOldProc   = 0;
    CHAR* pcOverWrite = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

    VirtualProtect((LPVOID)syscallStub, SYSCALL_STUB_SIZE, PAGE_READWRITE, &dwOldProc);
    memcpy((LPVOID)syscallStub, (LPVOID)pcOverWrite, SYSCALL_STUB_SIZE);

    return;
}

VOID ParseNtdll(struct NtInfo *NtdllInfo, struct Syscalls *SyscallTable)
{
    HANDLE hFile;
    DWORD  dwFileSize;
    LPVOID lpFileData;
    DWORD  dwBytesRead;

    DWORD dwOldProc                       = 0;

    SyscallTable->NtAllocateVirtualMemory = (_NtAllocateVirtualMemory)(LPVOID)SyscallStub;
    SyscallTable->NtProtectVirtualMemory  = (_NtProtectVirtualMemory)(LPVOID)SyscallStub;

    VirtualProtect(SyscallStub, SYSCALL_STUB_SIZE, PAGE_READWRITE, &dwOldProc);

    hFile                = CreateFileA(NTDLL_PATH, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    dwFileSize           = GetFileSize(hFile, NULL);
    NtdllInfo->lpRawData = HeapAlloc(GetProcessHeap(), 0, dwFileSize);

    ReadFile(hFile, NtdllInfo->lpRawData, dwFileSize, &dwBytesRead, NULL);

    PIMAGE_DOS_HEADER dosHeader      = (PIMAGE_DOS_HEADER)NtdllInfo->lpRawData;
	PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD_PTR)NtdllInfo->lpRawData + dosHeader->e_lfanew);
	DWORD dwExportDirRVA             = imageNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    PIMAGE_SECTION_HEADER section    = IMAGE_FIRST_SECTION(imageNTHeaders);
	NtdllInfo->pTextSection          = section;
	NtdllInfo->pRdataSection         = section;

    for (INT i = 0; i < imageNTHeaders->FileHeader.NumberOfSections; i++)
	{
        if (strncmp(section->Name, ".rdata", 6) == NULL)
        {
            NtdllInfo->pRdataSection = section;
            break;
        }
        section++;
    }

    NtdllInfo->pExprtDir = (PIMAGE_EXPORT_DIRECTORY)RVAtoRawOffset((DWORD_PTR)NtdllInfo->lpRawData + dwExportDirRVA, NtdllInfo->pRdataSection);
}