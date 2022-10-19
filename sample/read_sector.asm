; read head 0, cylinder 0, sector 1 from floppy into string and print it
org 0x7c00
bits 16

main:
	mov si, DisplayStr
	call Print
	cli
	hlt
	jmp main

; ds:si string address
Print:
	push ax
	push bx

PrintLoop:
	xor ax, ax
	xor bx, bx
	lodsb
	test al, al
	jz PrintDone
	mov ah, 0x0e
	int 0x10
	jmp PrintLoop
PrintDone:
	pop bx
	pop ax
	ret

DisplayStr: db "Hello, world!", 0

times 510 - ($ - $$) db 0
dw 0xaa55