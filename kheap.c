// kheap.c -- Kernel heap functions, also provides
//            a placement malloc() for use before the heap is
//            initialised.
//            Written for JamesM's kernel development tutorials.

#include "kheap.h"

// end is defined in the linker script.
extern u32 end; // 0x107ca4
u32 placement_address = (u32)&end;

u32 kmalloc_init(u32 sz, int align, u32 *phys)
{
	// This will eventually call malloc() on the kernel heap.
	// For now, though, we just assign memory at placement_address
	// and increment it by sz. Even when we've coded our kernel
	// heap, this will be useful for use before the heap is initialised.
	if (align == 1 && (placement_address & 0xFFFFF000))
	{
		// Align the placement address;
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}
	if (phys)
	{
		*phys = placement_address;
	}
	u32 tmp = placement_address;
	placement_address += sz;
	return tmp;
}

u32 kmalloc_a(u32 sz)
{
	return kmalloc_init(sz, 1, 0);
}

u32 kmalloc_p(u32 sz, u32 *phys)
{
	return kmalloc_init(sz, 0, phys);
}

u32 kmalloc_ap(u32 sz, u32 *phys)
{
	return kmalloc_init(sz, 1, phys);
}

u32 kmalloc(u32 sz)
{
	return kmalloc_init(sz, 0, 0);
}
