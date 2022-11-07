#include "kernel/multiboot.h"
#include "kernel/framebuffer.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/mm/pmm.h"
// #include "kernel/mm/vmm.h"
#include "kernel/constants.h"
#include "lib/string.h"

extern void __kernel_start;
extern void __kernel_end;

void kmain(multiboot_info_t * mb_info) {
	fb_clear();

	printf("[KERNEL] Mem: 0x%x KB, Extended Mem: 0x%x KB\n\x09Total Mem: 0x%x B\n",
					mb_info->mem_lower,
					mb_info->mem_upper,
					(mb_info->mem_upper<<10) + 0x100000); // 可使用内存总数

	// multiboot_memory_map_t *mmap;
	// printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
	// 				(unsigned) mb_info->mmap_addr, (unsigned) mb_info->mmap_length);
	// for (mmap = (multiboot_memory_map_t *) mb_info->mmap_addr;
	// 			(unsigned long) mmap < mb_info->mmap_addr + mb_info->mmap_length;
	// 			mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
	// 															+ mmap->size + sizeof (mmap->size)))
	// 	printf (" size = 0x%x, base_addr = 0x%x%08x,"
	// 					" length = 0x%x%08x, type = 0x%x\n",
	// 					(unsigned) mmap->size,
	// 					(unsigned) (mmap->addr >> 32),
	// 					(unsigned) (mmap->addr & 0xffffffff),
	// 					(unsigned) (mmap->len >> 32),
	// 					(unsigned) (mmap->len & 0xffffffff),
	// 					(unsigned) mmap->type);

	multiboot_memory_map_t* map = (multiboot_memory_map_t*)mb_info->mmap_addr;
	uint8_t map_size = mb_info->mmap_length / sizeof(multiboot_memory_map_t);
	printf("[KERNEL] Physical Memory Map:\n");
	for (uint8_t i = 0; i < map_size; i++) {
		printf("\tBase: 0x%08x, len: 0x%08x B, type: 0x%x\n",
						map[i].addr_low,
						map[i].len_low,
						map[i].type);
		if(map[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
			// 标记available内存
			pmm_deinit_region(map[i].addr_low, map[i].len_low);
		}
	}

	init_gdt();
	init_idt();

	init_pmm(MEM_1MB + (mb_info->mem_upper << 10));
	printf("[KERNEL] pmm initialized with %d KB physical memory\n", pmm_get_memory_size()/1024);

	// init_paging();

	// asm volatile("int $0x3");
	// asm volatile("int $0x4");
}