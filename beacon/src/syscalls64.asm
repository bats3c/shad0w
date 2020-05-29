section .text

; Windows 10 definitions
global NtQueueApcThread10
global NtWriteVirtualMemory10
global NtAllocateVirtualMemory10
global NtProtectVirtualMemory10

; Windows 8.1 definitions
global NtQueueApcThread81
global NtWriteVirtualMemory81
global NtAllocateVirtualMemory81
global NtProtectVirtualMemory81

; Windows 8.1 definitions
global NtQueueApcThread8
global NtWriteVirtualMemory8
global NtAllocateVirtualMemory8
global NtProtectVirtualMemory8

; Windows 8.1 definitions
global NtQueueApcThread7
global NtWriteVirtualMemory7

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

NtQueueApcThread10:
	mov r10, rcx
	mov eax, 45h
	syscall
	ret

NtWriteVirtualMemory10:
	mov r10, rcx
	mov eax, 3Ah
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

NtQueueApcThread81:
	mov r10, rcx
	mov eax, 44h
	syscall
	ret

NtWriteVirtualMemory81:
	mov r10, rcx
	mov eax, 39h
	syscall
	ret
; ----------------------------------------

; Windows 8.0
; ----------------------------------------
NtAllocateVirtualMemory8:
	mov r10, rcx
	mov eax, 16h
	syscall
	ret
	
NtProtectVirtualMemory8:
	mov r10, rcx
	mov eax, 4eh
	syscall
	ret
	
NtQueueApcThread8:
	mov r10, rcx
	mov eax, 43h
	syscall
	ret

NtWriteVirtualMemory8:
	mov r10, rcx
	mov eax, 38h
	syscall
	ret
; ----------------------------------------

; Windows 7 SP1 and below
; ----------------------------------------
NtQueueApcThread7:
	mov r10, rcx
	mov eax, 42h
	syscall
	ret

NtWriteVirtualMemory7:
	mov r10, rcx
	mov eax, 37h
	syscall
	ret
; ----------------------------------------