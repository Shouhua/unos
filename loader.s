global loader

MAGIC_NUMBER    equ 0x1BADB002
FLAGS           equ 0x0
CHECKSUM        equ -MAGIC_NUMBER
KERNEL_STACK_SIZE equ 4096

; extern main
extern fb_write_cell

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE

section .text:
align 4
    dd  MAGIC_NUMBER
    dd  FLAGS
    dd  CHECKSUM

%define debug xchg bx, bx

loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    mov ebp, esp
    push byte 0x08
    push byte 0x02
    push byte 0x41
    push dword 0x5100
    call fb_write_cell

    debug
    ; call main
    mov eax, 0xCAFEBABE
.loop:
    jmp .loop
