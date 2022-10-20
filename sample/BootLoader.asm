; read head 0, cylinder 0, sector 1 from floppy into string and print it
org 0x7c00
bits 16

main:
	call ResetDisk

	mov ax, 0x1000
	mov es, ax
	xor bx, bx
	call ReadSector
	jmp 0x1000:0x0
	

; read sectors into es:bx
ReadSector:
	push ax
	push cx
	push dx

	mov ah, 0x02
	mov al, 1
	mov ch, 1
	mov cl, 2
	mov dh, 0
	mov dl, 0
	int 0x13
	jc ReadSector

	pop dx
	pop cx
	pop ax
	ret

ResetDisk:
	push ax
	push dx
	xor ax, ax
	xor dx, dx

	mov ah, 0x0
	mov dl, 0x0
	int 0x13
	jc ResetDisk

	pop dx
	pop ax
	ret

times 510 - ($ - $$) db 0
dw 0xaa55