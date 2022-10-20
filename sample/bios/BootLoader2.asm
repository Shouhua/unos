org 0x10000
bits 16

entry:
	mov ax, 0x1000
	mov ds, ax
	mov si, DisplayStr
	call Print

	mov ah, 0
	int 0x16                    ; await keypress
	int 0x19                    ; warm boot computer

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

DisplayStr: db "Stage2 Now...", 0x0d, 0x0a, 0