section .text

; Windows 10 definitions
global NtAllocateVirtualMemory10
global NtProtectVirtualMemory10

; Windows 8.1 definitions
global NtAllocateVirtualMemory81
global NtProtectVirtualMemory81

; Windows 10
; ----------------------------------------
NtAllocateVirtualMemory10:
	mov r10, rcx
	mov eax, 18h
	syscall
	ret

NtProtectVirtualMemory10:
	mov r10, rcx
	mov eax, 50h
	syscall
	ret
; ----------------------------------------

; Windows 8.1
; ----------------------------------------
NtAllocateVirtualMemory81:
	mov r10, rcx
	mov eax, 17h
	syscall
	ret
	
NtProtectVirtualMemory81:
	mov r10, rcx
	mov eax, 4fh
	syscall
	ret
; ----------------------------------------

; Windows 8.0
; ----------------------------------------
NtAllocateVirtualMemory80:
	mov r10, rcx
	mov eax, 16h
	syscall
	ret
	
NtProtectVirtualMemory80:
	mov r10, rcx
	mov eax, 4eh
	syscall
	ret
; ----------------------------------------