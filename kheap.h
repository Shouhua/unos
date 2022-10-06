// kheap.h -- Interface for kernel heap functions, also provides
//            a placement malloc() for use before the heap is
//            initialised.
//            Written for JamesM's kernel development tutorials.

#ifndef KHEAP_H
#define KHEAP_H

#include "types.h"

/**
   Allocate a chunk of memory, sz in size. If align == 1,
   the chunk must be page-aligned. If phys != 0, the physical
   location of the allocated chunk will be stored into phys.

   This is the internal version of kmalloc. More user-friendly
   parameter representations are available in kmalloc, kmalloc_a,
   kmalloc_ap, kmalloc_p.
**/
u32 kmalloc_init(u32 sz, int align, u32 *phys);

/**
   Allocate a chunk of memory, sz in size. The chunk must be
   page aligned.
**/
u32 kmalloc_a(u32 sz);

/**
   Allocate a chunk of memory, sz in size. The physical address
   is returned in phys. Phys MUST be a valid pointer to u32!
**/
u32 kmalloc_p(u32 sz, u32 *phys);

/**
   Allocate a chunk of memory, sz in size. The physical address
   is returned in phys. It must be page-aligned.
**/
u32 kmalloc_ap(u32 sz, u32 *phys);

/**
   General allocation function.
**/
u32 kmalloc(u32 sz);

#endif // KHEAP_H
