org 0x10000
bits 16

entry:
	mov si, DisplayStr
	call Print
	cli
	hlt

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

DisplayStr: db "Hello, world!", 0x0d, 0x0a, 0