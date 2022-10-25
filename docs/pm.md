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
						; while code in ring 2 and 3 can. Note that the privilege level remains the same, ie. a far-jump from ring 3 to a segment with a DPL of 2 remains in ring 3 after the jump.
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