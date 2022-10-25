; GDT(global descriptor table)
; /*
; struct gdt_access
; {
	; ; 3	 2	1	0
	; ;	E DC RW A
	; ; E, execute bit, 0 data segement, 1 code segement
	; ; DC Direction bit/Conforming bit
	; ;		data: 0 data grow up, 
	; ;		data: 1 grow down, ie. the offset must be greater than limit
	; ; 	code: If clear (0) code in this segment can only be executed from the ring set in DPL.
	; ; 	code: If set (1) code in this segment can be executed from an equal or lower privilege level. 
				; For example, code in ring 3 can far-jump to conforming code in a ring 2 segment. 
				; The DPL field represent the highest privilege level that is allowed to execute the segment. 
					; For example, code in ring 0 cannot far-jump to a conforming code segment where DPL is 2, 
						; while code in ring 2 and 3 can. Note that the privilege level remains the same, 
						;	ie. a far-jump from ring 3 to a segment with a DPL of 2 remains in ring 3 after the jump.
; RW: Readable bit/Writable bit.
	; data: Writeable bit. If clear (0), write access for this segment is not allowed. If set (1) write access is allowed. Read access is always allowed for data segments.
	; code: Readable bit. If clear (0), read access for this segment is not allowed. If set (1) read access is allowed. Write access is never allowed for code segments.
; A: Accessed bit. Best left clear (0), the CPU will set it when the segment is accessed.
	; u8 type : 4; // 36 - 39: segment type - code segment
	; u8 dt : 1;	 // 40: descriptor type, 1 code/data, 0 system gate/tss etc
	; u8 dpl : 2;	 // 41 - 42: descriptor privilege level - Ring 0 - 3
	; u8 p : 1;	 // 43: is segment present? (1=yes)
; } _packed;
; typedef struct gdt_access gdt_access_t;
; 
; struct gdt_entry
; {
	; u16 limit_low : 16;	 // 0 - 15
	; u32 base_low : 24;	 // 16 - 35
	; gdt_access_t access; // 36 - 43
	; u8 limit_high : 4;	 // 44 - 46
	; u8 a : 1;			 // 47: available for system use, must be 0
	; u8 unused : 1;		 // 48: must be 0
	; u8 d : 1;			 // 49: operand size (0 = 16 bit, 1 = 32 bit)
	; u8 g : 1;			 // 50: granularity (0 = 1 Byte, 1 = 1KiB)
	; u8 base_high : 8;	 // 51 - 58
; } _packed;
; typedef struct gdt_entry gdt_entry_t;
; 
; struct gdt_ptr
; {
	; u16 limit; 
	; gdt_entry_t *base; ; 32bits
; } _packed;
; */

DA_32		EQU	4000h	; 32 位段

DA_DPL0		EQU	  00h	; DPL = 0
DA_DPL1		EQU	  20h	; DPL = 1
DA_DPL2		EQU	  40h	; DPL = 2
DA_DPL3		EQU	  60h	; DPL = 3
;----------------------------------------------------------------------------
; 存储段描述符类型值说明
;----------------------------------------------------------------------------
; Execute Direction/Conforming Read/Write Accessed
DA_DR		EQU	90h	; 存在的只读数据段类型值
DA_DRW		EQU	92h	; 存在的可读写数据段属性值 
DA_DRWA		EQU	93h	; 存在的已访问可读写数据段类型值
DA_C		EQU	98h	; 存在的只执行代码段属性值
DA_CR		EQU	9Ah	; 存在的可执行可读代码段属性值
DA_CCO		EQU	9Ch	; 存在的只执行一致代码段属性值
DA_CCOR		EQU	9Eh	; 存在的可执行可读一致代码段属性值
;----------------------------------------------------------------------------
; 系统段描述符类型值说明
;----------------------------------------------------------------------------
; P DPL DT(descriptor type 1 code/data segment, 0 gate trap)
DA_LDT		EQU	  82h	; 局部描述符表段类型值 (LDT)
DA_TaskGate	EQU	  85h	; 任务门类型值(Task Gate)
DA_386TSS	EQU	  89h	; 可用 386 任务状态段类型值
DA_386CGate	EQU	  8Ch	; 386 调用门类型值
DA_386IGate	EQU	  8Eh	; 386 中断门类型值
DA_386TGate	EQU	  8Fh	; 386 陷阱门类型值
;----------------------------------------------------------------------------


; 选择子图示:
;         ┏━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┓
;         ┃ 15 ┃ 14 ┃ 13 ┃ 12 ┃ 11 ┃ 10 ┃ 9  ┃ 8  ┃ 7  ┃ 6  ┃ 5  ┃ 4  ┃ 3  ┃ 2  ┃ 1  ┃ 0  ┃
;         ┣━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━╋━━╋━━┻━━┫
;         ┃                                 描述符索引                       ┃ TI ┃   RPL    ┃
;         ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┻━━┻━━━━━┛
;
; RPL(Requested Privilege Level): 请求特权级，用于特权检查。
;
; TI(Table Indicator): 引用描述符表指示位
;	TI=0 指示从全局描述符表GDT中读取描述符；
;	TI=1 指示从局部描述符表LDT中读取描述符。
;

;----------------------------------------------------------------------------
; 选择子类型值说明
; 其中:
;       SA_  : Selector Attribute

SA_RPL0		EQU	0	; ┓
SA_RPL1		EQU	1	; ┣ RPL
SA_RPL2		EQU	2	; ┃
SA_RPL3		EQU	3	; ┛

SA_TIG		EQU	0	; ┓TI
SA_TIL		EQU	4	; ┛
;----------------------------------------------------------------------------

; 宏 ------------------------------------------------------------------------------------------------------
;
; 描述符
; usage: Descriptor Base, Limit, Attr
;        Base:  dd
;        Limit: dd (low 20 bits available)
;        Attr:  dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3
	dw	%2 & 0FFFFh				; 段界限 1				(2 字节)
	dw	%1 & 0FFFFh				; 段基址 1				(2 字节)
	db	(%1 >> 16) & 0FFh			; 段基址 2				(1 字节)
	dw	((%2 >> 8) & 0F00h) | (%3 & 0F0FFh)	; 属性 1 + 段界限 2 + 属性 2		(2 字节)
	db	(%1 >> 24) & 0FFh			; 段基址 3				(1 字节)
%endmacro ; 共 8 字节
; high -> low
; offset(2 bytes), attrs(P dpl(2bits) s type(4bits) 000,param count), selector(2bytes), offset(2bytes)
; 门
; usage: Gate Selector, Offset, DCount, Attr
;        Selector:  dw
;        Offset:    dd
;        DCount:    db
;        Attr:      db
%macro Gate 4
	dw	(%2 & 0FFFFh)				; 偏移 1				(2 字节)
	dw	%1					; 选择子				(2 字节)
	dw	(%3 & 1Fh) | ((%4 << 8) & 0FF00h)	; 属性					(2 字节)
	dw	((%2 >> 16) & 0FFFFh)			; 偏移 2				(2 字节)
%endmacro ; 共 8 字节
