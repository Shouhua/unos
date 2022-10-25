%include "pm.inc"

org 0x10000
	jmp LABEL_BEGIN

[section .gdt]
LABEL_GDT:							Descriptor	0,								 0,	0
LABEL_DESC_CODE32:			Descriptor  0,  SegCode32Len - 1, DA_C + DA_32	
LABEL_DESC_VIDEO:				Descriptor	0xb8000,		  0xffff,	DA_DRW + DA_DPL3
LABEL_DESC_LDT:					Descriptor	0,		  LDTLen - 1,	DA_LDT
LABEL_DESC_CODE_DEST: 	Descriptor  0, SegCodeDestLen - 1, DA_C + DA_32
LABEL_CALL_GATE_TEST: 	Gate 				SelectorCodeDest, 0, 0, DA_386CGate + DA_DPL3
LABEL_DESC_STACK:     	Descriptor  0,       TopOfStack - 1, DA_DRWA+DA_32;Stack, 32 位`

LABEL_DESC_CODE_RING3:  Descriptor  0, SegCodeRing3Len - 1, DA_C + DA_32 + DA_DPL3
LABEL_DESC_STACK3: 			Descriptor	0, TopOfStack3 - 1, DA_DRWA + DA_32 + DA_DPL3
LABEL_DESC_TSS:         Descriptor  0,          TSSLen-1, DA_386TSS

GdtLen	equ	$ - LABEL_GDT
GdtPtr	dw	GdtLen - 1 ; 16 bits limit
				dd	0 ; 32 bits base

; selector
SelectorCode32	equ	LABEL_DESC_CODE32 - LABEL_GDT
SelectorVideo		equ LABEL_DESC_VIDEO  - LABEL_GDT
SelectorLDT			equ LABEL_DESC_LDT		- LABEL_GDT
SelectorCodeDest	equ LABEL_DESC_CODE_DEST - LABEL_GDT
SelectorCallGateTest equ LABEL_CALL_GATE_TEST - LABEL_GDT + SA_RPL3
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT

SelectorCodeRing3 equ LABEL_DESC_CODE_RING3 - LABEL_GDT + SA_RPL3
SelectorStack3 equ LABEL_DESC_STACK3 - LABEL_GDT + SA_RPL3
SelectorTSS		equ	LABEL_DESC_TSS		- LABEL_GDT

[section .s16]
[bits 16]
LABEL_BEGIN:
	xor eax, eax
	mov ax, cs

	mov ds, ax

	; shl eax, 4
	; add eax, LABEL_SEG_CODE32
	xor eax, eax
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

	; 初始化 TSS 描述符
	xor	eax, eax
	mov	eax, LABEL_TSS
	mov	word [LABEL_DESC_TSS + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_TSS + 4], al
	mov	byte [LABEL_DESC_TSS + 7], ah

	; 初始化堆栈段描述符
	xor	eax, eax
	mov	eax, LABEL_STACK
	mov	word [LABEL_DESC_STACK + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK + 4], al
	mov	byte [LABEL_DESC_STACK + 7], ah

	; 初始化Ring3描述符
	mov	eax, LABEL_CODE_RING3
	mov	word [LABEL_DESC_CODE_RING3 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE_RING3 + 4], al
	mov	byte [LABEL_DESC_CODE_RING3 + 7], ah

	; init ring 3 statck
	mov	eax, LABEL_STACK3
	mov	word [LABEL_DESC_STACK3 + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_STACK3 + 4], al
	mov	byte [LABEL_DESC_STACK3 + 7], ah

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

	mov ax, SelectorStack
	mov ss, ax
	mov esp, TopOfStack

	mov ax, SelectorVideo
	mov gs, ax
	mov edi, (80 * 11 + 79) * 2
	mov ah, 0x0c
	mov al, 'P'
	mov [gs:edi], ax

	mov ax, SelectorTSS
	ltr ax

	push SelectorStack3
	push TopOfStack3
	push SelectorCodeRing3
	push 0
	retf

	ud2 ; should never arrived here

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

; 全局堆栈段
[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
	times 512 db 0

TopOfStack	equ	$ - LABEL_STACK

[section .s3]
align 32
[bits 32]
LABEL_STACK3:
	times 512 db 0
TopOfStack3 equ $ - LABEL_STACK3

[section .ring3]
align 32
[bits 32]
LABEL_CODE_RING3:
	mov ax, SelectorVideo
	mov gs, ax
	mov edi, (80 * 14 + 0) * 2
	mov ah, 0x0c
	mov al, '3'
	mov [gs:edi], ax

	call SelectorCallGateTest:0
	
	cli
	hlt
SegCodeRing3Len equ $ - LABEL_CODE_RING3

; TSS
[SECTION .tss]
ALIGN	32
[BITS	32]
LABEL_TSS:
		DD	0			; Back
		DD	TopOfStack		; 0 级堆栈
		DD	SelectorStack		; 
		DD	0			; 1 级堆栈
		DD	0			; 
		DD	0			; 2 级堆栈
		DD	0			; 
		DD	0			; CR3
		DD	0			; EIP
		DD	0			; EFLAGS
		DD	0			; EAX
		DD	0			; ECX
		DD	0			; EDX
		DD	0			; EBX
		DD	0			; ESP
		DD	0			; EBP
		DD	0			; ESI
		DD	0			; EDI
		DD	0			; ES
		DD	0			; CS
		DD	0			; SS
		DD	0			; DS
		DD	0			; FS
		DD	0			; GS
		DD	0			; LDT
		DW	0			; 调试陷阱标志
		DW	$ - LABEL_TSS + 2	; I/O位图基址
		DB	0ffh			; I/O位图结束标志
TSSLen		equ	$ - LABEL_TSS