org 0x0
bits 16

jmp main

LoadRoot:
	mov ax, 0x0020 ; 32 bytes
	mul	word [bpbRootEntries] ; 32 * 224
	div word [bpbBytesPerSector] ; how many sectors used by root directory(14)

	mov al, [bpbNumberOfFATs]
	mul [bpbSectorsPerFAT]
	add ax, [bpbReservedSectors] ; how many sectors before root directory(19)

	mov ax, 0x0200 ; load root directory to 7c00:0x0200
	call ReadSectors

	mov cx, [bpbRootEntries]
	mov di, 0x0200 ; root directory was loaded here
.LOOP:
	push cx
	mov cx, 11 ; file name and extension
	mov si, ImageName
	push di
rep cmpsb
	pop di
	je LOAD_FAT
	pop cx
	add di, 32
	loop .LOOP
	jmp FAILURE

	mov dx, [di+ax001a]
	
	xor ax, ax
	mov al, [bpbNumberOfFATs]
	mul word [bpbSectorsPerCluster]



Print:
; For legacy mode, Load byte at address DS:(E)SI into AL. For 64-bit mode load byte at address (R)SI into AL.
; stosb es:edi > al
; movsb ds:si > es:di cx是次数，si,di自动增加, 使用df控制，cld（df=0）递增，std（df=1）递减，可以使用如下
; rep movsb
	lodsb 
	or al, al
	jz PrintDone
	mov ah, 0x0e
; int 0x10
;AH = 0x0E
;AL = Character to write
;BH - Page Number (Should be 0)
;BL = Foreground color (Graphics Modes Only)
	int 0x10
	jmp Print
PrintDone:
	ret

main:
	cli
	push cs
	pop ds

	mov si, msg
	call Print

	cli
	hlt

msg	db	"Preparing to load operation system...", 13, 10, 0