; read head 0, cylinder 0, sector 1 from floppy into string and print it
org 0x7c00
[bits 16]

jmp MAIN

MAIN:
	mov ax, 0x7c00
	; mov cs, ax
	mov ds, ax

	mov ss, ax
	mov bp, ax
	mov sp, 0x100

	mov ax, 0
	mov es, ax

	call RESETDISK

	mov ax, 0x1000
	mov es, ax
	xor bx, bx
	call READSECTOR
	jmp 0x1000:0x0

; read sectors into es:bx
READSECTOR:
	push ax
	push cx
	push dx

	clc

	mov ah, 0x02
	mov al, 2
	mov ch, 0
	mov cl, 2
	mov dh, 0
	mov dl, 0
	int 0x13
	jc .FAILURE
.DONE:
	pop dx
	pop cx
	pop ax
	ret
.FAILURE:
	mov di, READFAIL
	call PRINT
	jmp $


RESETDISK:
	push ax
	push dx
	xor ax, ax
	xor dx, dx

	mov ah, 0x0
	mov dl, 0x0
	int 0x13
	jc RESETDISK

	pop dx
	pop ax
	ret

PRINT:
	push ax
	push bx
PRINTLOOP:
	xor ax, ax
	xor bx, bx
	lodsb
	test al, al
	jz PRINTDONE
	mov ah, 0x0e
	int 0x10
	jmp PRINTLOOP
PRINTDONE:
	pop bx
	pop ax
	ret

READFAIL: db 'READ SECTOR FAILED!', 13, 10, 0

times 510 - ($ - $$) db 0
dw 0xaa55