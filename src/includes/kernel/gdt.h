#ifndef __GDT_H__
#define __GDT_H__

#include "lib/stdint.h"

#define GDT_ACCESS_TYPE_CODE 10

// Granularity Field (1 bit)
#define GDT_GRANULARITY_BYTE 0
#define GDT_GRANULARITY_4KILOBYTE 1

// D Field (1 bit) - defines operand size for code segments
#define GDT_DEFAULT_OPERAND_SIZE_16 0
#define GDT_DEFAULT_OPERAND_SIZE_32 1

// DPL Field (2 bits)
#define GDT_DPL_RING0 0
#define GDT_DPL_RING1 1
#define GDT_DPL_RING2 2
#define GDT_DPL_RING3 3

// Segment Type Field (4 bits)
#define GDT_SEGMENT_TYPE_DATA 0
#define GDT_SEGMENT_TYPE_CODE 0x8

// Data Type
// code: 1, confirming, readable(1 readable, 0 only execute), accessed(0)
// code: 0, expand down, writable, accessed
// E - expand direction
#define GDT_SEGMENT_TYPE_EXPAND_UP 0
#define GDT_SEGMENT_TYPE_EXPAND_DOWN 0x4

// W - read/write
#define GDT_SEGMENT_TYPE_READ_ONLY 0
#define GDT_SEGMENT_TYPE_READ_WRITE 0x2

// Code Type
// C - non/confirming
#define GDT_SEGMENT_TYPE_NONCONFORMING 0
#define GDT_SEGMENT_TYPE_CONFORMING 0x4

// R - execute/read
#define GDT_SEGMENT_TYPE_EXECUTE_ONLY 0
#define GDT_SEGMENT_TYPE_EXECUTE_READ 0x2

// A - accessed
#define GDT_SEGMENT_TYPE_RESET_ACCESSED 0

// Segment Present (p) - segment present in memory?
#define GDT_SEGMENT_NOT_PRESENT 0
#define GDT_SEGMENT_PRESENT 1

// Descriptor Type (S)
#define GDT_SEGMENT_DT_SYSTEM 0
#define GDT_SEGMENT_DT_CODE 1

#define GDT_SYSTEM_TSS 9


struct gdt_access
{
	uint8_t type : 4; // 36 - 39: segment type - code segment
	uint8_t dt : 1;	 // 40: descriptor type
	uint8_t dpl : 2;	 // 41 - 42: descriptor privilege level - Ring 0 - 3
	uint8_t p : 1;	 // 43: is segment present? (1=yes)
} _packed;
typedef struct gdt_access gdt_access_t;

struct gdt_entry
{
	uint16_t limit_low : 16;	 // 0 - 15
	uint32_t base_low : 24;	 // 16 - 35
	gdt_access_t access; // 36 - 43
	uint8_t limit_high : 4;	 // 44 - 46
	uint8_t a : 1;			 // 47: available for system use, must be 0
	uint8_t unused : 1;		 // 48: must be 0
	uint8_t d : 1;			 // 49: operand size (0 = 16 bit, 1 = 32 bit)
	uint8_t g : 1;			 // 50: granularity (0 = 1 Byte, 1 = 1KiB)
	uint8_t base_high : 8;	 // 51 - 58
} _packed;
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr
{
	uint16_t limit;
	gdt_entry_t *base;
} _packed;
typedef struct gdt_ptr gdt_ptr_t;

void init_gdt();
void gdt_set_entry(
		int32_t idx,
		uint32_t base,
		uint32_t limit,
		gdt_access_t access,
		uint8_t d,
		uint8_t g);

#endif