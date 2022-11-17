#include "kernel/gdt.h"
#include "lib/string.h"

#define GDT_ENTRY_SIZE 6

extern void gdt_flush(gdt_ptr_t *);

gdt_entry_t gdt_entries[GDT_ENTRY_SIZE];
gdt_ptr_t gdt_ptr;

void gdt_set_entry(
		int32_t idx,
		uint32_t base,
		uint32_t limit,
		gdt_access_t access,
		uint8_t d,
		uint8_t g)
{

	gdt_entries[idx] = (struct gdt_entry){
			.limit_low = limit & 0xffff,
			.base_low = base & 0xffffff,
			.access = access,
			.limit_high = (limit >> 16) & 0xf,
			.a = 0,
			.unused = 0,
			.d = d,
			.g = g,
			.base_high = (base >> 24) & 0xffff};
}

void init_gdt()
{
	gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_ENTRY_SIZE - 1;
	gdt_ptr.base = gdt_entries;

	gdt_set_entry(0, 0, 0, (struct gdt_access){0, 0, 0, 0}, 0, 0);
	gdt_set_entry(
			1, // index
			0, 0xffffffff,
			(struct gdt_access){
					.type = GDT_SEGMENT_TYPE_CODE | GDT_SEGMENT_TYPE_EXECUTE_READ,
					.dt = GDT_SEGMENT_DT_CODE,
					.dpl = GDT_DPL_RING0,
					.p = GDT_SEGMENT_PRESENT}, // access
			GDT_DEFAULT_OPERAND_SIZE_32,	 // d
			GDT_GRANULARITY_4KILOBYTE			 // g
	);

	gdt_set_entry(
			2, // index
			0, 0xffffffff,
			(struct gdt_access){
					.type = GDT_SEGMENT_TYPE_DATA | GDT_SEGMENT_TYPE_READ_WRITE,
					.dt = GDT_SEGMENT_DT_CODE,
					.dpl = GDT_DPL_RING0,
					.p = GDT_SEGMENT_PRESENT}, // access
			GDT_DEFAULT_OPERAND_SIZE_32,	 // d
			GDT_GRANULARITY_4KILOBYTE			 // g
	);

	gdt_set_entry(
			3, // index
			0, 0xffffffff,
			(struct gdt_access){
					.type = GDT_SEGMENT_TYPE_CODE | GDT_SEGMENT_TYPE_EXECUTE_READ,
					.dt = GDT_SEGMENT_DT_CODE,
					.dpl = GDT_DPL_RING3,
					.p = GDT_SEGMENT_PRESENT}, // access
			GDT_DEFAULT_OPERAND_SIZE_32,	 // d
			GDT_GRANULARITY_4KILOBYTE			 // g
	);

	gdt_set_entry(
			4, // index
			0, 0xffffffff,
			(struct gdt_access){
					.type = GDT_SEGMENT_TYPE_DATA | GDT_SEGMENT_TYPE_READ_WRITE,
					.dt = GDT_SEGMENT_DT_CODE,
					.dpl = GDT_DPL_RING3,
					.p = GDT_SEGMENT_PRESENT}, // access
			GDT_DEFAULT_OPERAND_SIZE_32,	 // d
			GDT_GRANULARITY_4KILOBYTE			 // g
	);

	gdt_flush(&gdt_ptr);
}