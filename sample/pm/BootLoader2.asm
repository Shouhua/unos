%include "pm.inc"

org 0x10000
	jmp LABEL_BEGIN

[section .gdt]
; gdt
LABEL_GDT:					Descriptor	0,								 0,	0
LABEL_DESC_CODE32:	Descriptor  0,  SegCode32Len - 1, DA_C + DA_32	
LABEL_DESC_VIDEO:		Descriptor	0xb8000,		  0xffff,	DA_DRW

GdtLen	equ	$ - LABEL_GDT
GdtPtr	dw	GdtLen - 1 ; 16 bits limit
				dd	0 ; 32 bits base

; selector
SelectorCode32	equ	LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo		equ LABEL_DESC_VIDEO  - LABEL_GDT

[section .s16]
[bits 16]
LABEL_BEGIN:
	; mov ax, cs
	; mov ds, ax
	; mov es, ax
	; mov ss, ax
	; mov sp, 0x100
	; TODO: 查看ss/sp/bp变化

	xor eax, eax
	mov ax, cs

	mov ds, ax

	; shl eax, 4
	; add eax, LABEL_SEG_CODE32
	mov eax, LABEL_SEG_CODE32
	mov word [LABEL_DESC_CODE32 + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_CODE32 + 4], al
	mov byte [LABEL_DESC_CODE32 + 7], ah

	xor eax, eax
	mov ax, ds
	; shl eax, 4
	; add eax, LABEL_GDT
	mov eax, LABEL_GDT
	mov dword [GdtPtr + 2], eax

	; load gdtr
	lgdt [GdtPtr]
	cli

	; open A20
	in al, 0x92
	or al, 00000010b
	out 0x92, al

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp dword	SelectorCode32:0

[section .s32]
[bits 32]
LABEL_SEG_CODE32:
	mov ax, SelectorVideo
	mov gs, ax
	mov edi, (80 * 11 + 79) * 2
	mov ah, 0x0c
	mov al, 'P'
	mov [gs:edi], ax

	cli
	hlt

SegCode32Len	equ	$ - LABEL_SEG_CODE32