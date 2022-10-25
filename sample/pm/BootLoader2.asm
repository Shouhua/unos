%include "pm.inc"

org 0x10000
	jmp LABEL_BEGIN

[section .gdt]
LABEL_GDT:					Descriptor	0,								 0,	0
LABEL_DESC_CODE32:	Descriptor  0,  SegCode32Len - 1, DA_C + DA_32	
LABEL_DESC_VIDEO:		Descriptor	0xb8000,		  0xffff,	DA_DRW
LABEL_DESC_LDT:			Descriptor	0,		  LDTLen - 1,	DA_LDT
LABEL_DESC_CODE_DEST: Descriptor 0, SegCodeDestLen - 1, DA_C + DA_32
LABEL_CALL_GATE_TEST: Gate SelectorCodeDest, 0, 0, DA_386CGate + DA_DPL0

GdtLen	equ	$ - LABEL_GDT
GdtPtr	dw	GdtLen - 1 ; 16 bits limit
				dd	0 ; 32 bits base

; selector
SelectorCode32	equ	LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo		equ LABEL_DESC_VIDEO  - LABEL_GDT
SelectorLDT			equ LABEL_DESC_LDT		- LABEL_GDT
SelectorCodeDest	equ LABEL_DESC_CODE_DEST - LABEL_GDT
SelectorCallGateTest equ LABEL_CALL_GATE_TEST - LABEL_GDT

[section .s16]
[bits 16]
LABEL_BEGIN:
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

	; init LDT
		; shl eax, 4
	; add eax, LABEL_SEG_CODE32
	xor eax, eax
	mov eax, LABEL_LDT
	mov word [LABEL_DESC_LDT + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_LDT + 4], al
	mov byte [LABEL_DESC_LDT + 7], ah

	;init ldt base address
	xor eax, eax
	mov eax, LABEL_CODE_A
	mov word [LABEL_LDT_DESC_CODEA + 2], ax
	shr eax, 16
	mov byte [LABEL_LDT_DESC_CODEA + 4], al
	mov byte [LABEL_LDT_DESC_CODEA + 7], ah

	; init call gate descriptor base address
	xor eax, eax
	mov eax, LABEL_SEG_CODE_DEST
	mov word [LABEL_DESC_CODE_DEST + 2], ax
	shr eax, 16
	mov byte [LABEL_DESC_CODE_DEST + 4], al
	mov byte [LABEL_DESC_CODE_DEST + 7], ah

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

	call SelectorCallGateTest:0

	mov	ax, SelectorLDT
	lldt ax

	jmp SelectorLDTCodeA:0

SegCode32Len	equ	$ - LABEL_SEG_CODE32

[section .ldt]
align 32
LABEL_LDT:
LABEL_LDT_DESC_CODEA: Descriptor 0, CodeALen - 1, DA_C + DA_32
LDTLen	equ	 $ - LABEL_LDT

SelectorLDTCodeA equ LABEL_LDT_DESC_CODEA - LABEL_LDT + SA_TIL

[section .la]
align 32
[bits 32]
LABEL_CODE_A:
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子(目的)

	mov	edi, (80 * 12 + 0) * 2	; 屏幕第 10 行, 第 0 列。
	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
	mov	al, 'L'
	mov	[gs:edi], ax

	cli
	hlt
CodeALen	equ	$ - LABEL_CODE_A

[section .sdest] ; 调用门目标段
[bits 32]
LABEL_SEG_CODE_DEST:
	mov ax, SelectorVideo
	mov gs, ax
	mov edi, (80 * 13 + 0) * 2
	mov ah, 0x0c
	mov al, 'C'
	mov [gs:edi], ax

	retf
SegCodeDestLen	equ	$ - LABEL_SEG_CODE_DEST