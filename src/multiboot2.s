MAGIC_NUMBER 	equ 0xE85250D6
ARCHITECTURE 	equ 0
HEADER_LENGTH	equ header_end - header_start
CHECKSUM 			equ -(MAGIC_NUMBER + ARCHITECTURE + HEADER_LENGTH)

section .multiboot2:
header_start:
	dd	MAGIC_NUMBER
	dd	ARCHITECTURE
	dd	HEADER_LENGTH
	dd 	CHECKSUM

	dw	0
	dw	0
	dd	8
header_end:
