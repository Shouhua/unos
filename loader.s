global loader

; %define debug xchg bx, bx

section .text:
loader:
    mov eax, 0xCAFEBABE
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    ; push byte 0x08
    ; push byte 0x02
    ; push byte 0x41
    ; push dword 320
    ; call fb_write_cell

    ; debug
    call main
.loop:
    jmp .loop

extern main

KERNEL_STACK_SIZE equ 4096

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE
