%define debug xchg bx, bx

MAGIC_NUMBER 		equ 0x1BADB002     ; define the magic number constant
ALIGN_MEM_MAP   equ 0x3            ; multiboot flags
CHECKSUM     		equ -(MAGIC_NUMBER+ALIGN_MEM_MAP)  ; calculate the checksum

; Some constants for loading higher half kernel
VM_BASE     equ 0xC0000000
PDE_INDEX   equ (VM_BASE >> 22)
PSE_BIT     equ 0x00000010
PG_BIT      equ 0x80000000

[section .multiboot]
align 4                         ; the code must be 4 byte aligned
		dd MAGIC_NUMBER             ; write the magic number to the machine code,
		dd ALIGN_MEM_MAP                    ; the flags,
		dd CHECKSUM                 ; and the checksum

[section .bss]
align 16
stack_bottom: resb 16318
stack_top:

; http://www.rcollins.org/ddj/May96/
; page directory entry中PE为1,指向4M
; 两个映射，0-4M identity mapping，0xC0000000 -> 0
section .data
align 4096
global TEMP_PAGE_DIRECTORY
TEMP_PAGE_DIRECTORY:
    dd 0x00000083
    times(PDE_INDEX - 1) dd 0
    dd 0x00000083
    times(1024 - PDE_INDEX - 1) dd 0 

[section .text]                  ; start of the text (code) section
global loader
extern kmain
loader:                         ; the loader label (defined as entry point in linker script)
	mov ecx, (TEMP_PAGE_DIRECTORY - VM_BASE)
	mov cr3, ecx

	; Enable 4mb pages
	mov ecx, cr4;
	or ecx, PSE_BIT
	mov cr4, ecx

	; Set PG bit, enable paging
	mov ecx, cr0
	or ecx, PG_BIT
	mov cr0, ecx

	; Why not just jmp higher_half ? If you do that, that will be a relative jmp, so u r jumping to virtual memory around 0x100000, which is fine since we have identity mapped earlier
	; but we also want to change the eip(now point to somewhere around 0x100000) to somewhere around 0xc0100000, so we need to get the absolute address of higher half into ecx, and jmp ecx
	lea ecx, [higher_half]
	jmp ecx
higher_half:
	; Unmap the first 4mb physical mem, because we don't need it anymore. Flush the tlb too
	; mov dword[TEMP_PAGE_DIRECTORY], 0
	; invlpg[0]

	; mov esp, stack_top
	; extern kmain
	; push ebx
	; call kmain

	debug

	cli
	hlt