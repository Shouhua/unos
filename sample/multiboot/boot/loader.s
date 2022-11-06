%define debug xchg bx, bx

MAGIC_NUMBER 		equ 0x1BADB002     ; define the magic number constant
ALIGN_MEM_MAP   equ 0x3            ; multiboot flags
CHECKSUM     		equ -(MAGIC_NUMBER+ALIGN_MEM_MAP)  ; calculate the checksum

[section .multiboot]
align 4                         ; the code must be 4 byte aligned
		dd MAGIC_NUMBER             ; write the magic number to the machine code,
		dd ALIGN_MEM_MAP                    ; the flags,
		dd CHECKSUM                 ; and the checksum

[section .bss]
align 16
stack_bottom: resb 16318
stack_top:

[section .text]                  ; start of the text (code) section
global loader
extern kmain
loader:                         ; the loader label (defined as entry point in linker script)
	mov esp, stack_top
	extern kmain
	push ebx
	call kmain

	debug

	cli
	hlt