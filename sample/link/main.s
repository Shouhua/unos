[section .bss]
	stack: resb 1024

[section .data]
msg: db 'hello, world!', 0x0a, 0x0d, 0
msg1: times 32 db 0

[section .text]
align 4
global loader
loader:
	nop
	nop
.done:
	jmp .done