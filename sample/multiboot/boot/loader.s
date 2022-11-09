bits 32
%include "gdt.inc"
%include "paging.inc"

%define debug xchg bx, bx

MAGIC_NUMBER 		equ 0x1BADB002     ; define the magic number constant
ALIGN_MEM_MAP   equ 0x3            ; multiboot flags
CHECKSUM     		equ -(MAGIC_NUMBER+ALIGN_MEM_MAP)  ; calculate the checksum

[section .multiboot]
align 4                         ; the code must be 4 byte aligned
		dd MAGIC_NUMBER             ; write the magic number to the machine code,
		dd ALIGN_MEM_MAP                    ; the flags,
		dd CHECKSUM                 ; and the checksum

[section .text]                  ; start of the text (code) section
global loader
loader:                         ; the loader label (defined as entry point in linker script)
	mov esp, 0x9000
	call InstallGDT	
	debug
	jmp 0x08:next
next:
	mov ax, DATA_DESC
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov esp, 0x9000
	call EnablePaging
	extern kmain
	push ebx
	call kmain

	cli
	hlt