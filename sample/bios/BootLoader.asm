; read head 0, cylinder 0, sector 1 from floppy into string and print it
org 0x7c00
bits 16

main:
	mov ax, 0x7e00
	mov ss, ax
	mov bp, ax
	mov sp, ax

	mov ax, 0x7c00
	mov ds, ax

	mov ax, 0
	mov es, ax

	call GetMemMap
	mov si, Done
	call Print
	cli
	hlt

	; call ResetDisk

	; mov ax, 0x7e00
	; mov ss, ax
	; mov bp, ax
	; mov sp, ax

	; mov ax, 0x1000
	; mov es, ax
	; xor bx, bx
	; call ReadSector
	; jmp 0x1000:0x0

GetMemMap:
	mov di, MemMap
	mov ebx, 0 ; 0 when start
.GetMemMapLoop:
	mov edx, 0x534d4150 ; magic number
	mov ecx, 0x00000014 ; read 20 bytes
	mov eax, 0x0000e820 ; interrupt function number
	int 0x15
	jc .Error ; error if carry flag is set
	or ebx, 0 ; done when ebx = 0 
	jz .Done
	add di, 0x0014
	jmp .GetMemMapLoop
.Done:
	ret
.Error:
	mov si, ReadMemMapError
	call Print
	ret

; read sectors into es:bx
ReadSector:
	push ax
	push cx
	push dx

	mov ah, 0x02
	mov al, 1
	mov ch, 0
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

MemMap: resb 200
ReadMemMapError: db "Read Mem Map Error", 0x0d, 0x0a, 0
Done: db "Read Mem Map Done...", 0x0d, 0x0a, 0

times 510 - ($ - $$) db 0
dw 0xaa55