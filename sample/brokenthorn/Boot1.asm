;*************************************
;	Boot1.asm
; 	- A Simple Bootloader
;*************************************
bits 16
org 0x7c00

start: jmp loader

;*************************************
; OEM Parameter block
;*************************************
;TIMES 0Bh - $ + start DB 0

; 总共11个字节，其中jmp loader有3个字节，bpbOEM使用8个字节
bpbOEM	db	"My OS    "

bpbBytesPerSector:	dw	512
bpbSectorsPerCluster:	db	1
bpbReservedSectors:	dw	1
bpbNumberOfFATs: 	    DB 2
bpbRootEntries: 	    DW 224
bpbTotalSectors: 	    DW 2880
bpbMedia: 	            DB 0xF0
bpbSectorsPerFAT: 	    DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	    DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 	            DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

loader:
/*
INT 0x13/AH=0x0 - DISK : RESET DISK SYSTEM
AH = 0x0
DL = Drive to Reset

Returns:
AH = Status Code
CF (Carry Flag) is clear if success, it is set if failure*
*/
.Reset:
	mov ah, 0
	mov dl, 0
	int 0x13
	jc	.Reset

/*
INT 0x13/AH=0x02 - DISK : READ SECTOR(S) INTO MEMORY
AH = 0x02
AL = Number of sectors to read
CH = Low eight bits of cylinder number
CL = Sector Number (Bits 0-5). Bits 6-7 are for hard disks only
DH = Head Number
DL = Drive Number (Bit 7 set for hard disks)
ES:BX = Buffer to read sectors to

Returns:
AH = Status Code
AL = Number of sectors read
CF = set if failure, cleared is successfull
*/
	mov ax, 0x1000
	mov es, ax
	xor bx, bx

.Read:
	mov ah, 0x02
	mov al, 1
	mov ch, 1
	mov cl, 2
	mov dh, 0
	mov dl, 0
	int 0x13
	jc	.Read

	jmp 0x1000:0x0
	
times 510 - ($ - $$) db 0

dw 0xaa55