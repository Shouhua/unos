#include "lib/string.h"
#include "kernel/multiboot.h"
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/framebuffer.h"
#include "kernel/mm/pmm.h"
#include "kernel/mm/vmm.h"
#include "kernel/constants.h"
#include "hal/cpu.h"
#include "kernel/keyboard.h"
// #include "kernel/timer.h"

// https://stackoverflow.com/questions/8398755/access-symbols-defined-in-the-linker-script-by-application
// https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
// 使用(uint32_t)&__kernel_start, 原因见链接中解释
extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

char * get_mem_type_str(multiboot_uint32_t type) {
	if(type == MULTIBOOT_MEMORY_AVAILABLE) {
		return "Available";
	}
	if(type == MULTIBOOT_MEMORY_RESERVED) {
		return "Reserved";
	}
	if(type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
		return "ACPI reclaimable";
	}
	if(type == MULTIBOOT_MEMORY_NVS) {
		return "NVS";
	}
	if(type == MULTIBOOT_MEMORY_BADRAM) {
		return "Bad RAM";
	}
	return "**NOT KNOWN**";
}

void kmain(multiboot_info_t * mb_info) {
	fb_clear();

	char buf[64];
	cpu_get_brand(buf);
	printf("[KERNEL] CPU: %s\n", buf);

	init_gdt();
	init_idt();

	init_pmm(MEM_1MB + (mb_info->mem_upper << 10), &__kernel_end);

	printf("[KERNEL] Mem: 0x%x KB, Extended Mem: 0x%x KB\n\x09Total Mem: 0x%x B\n",
					mb_info->mem_lower,
					mb_info->mem_upper,
					(mb_info->mem_upper<<10) + 0x100000); // 可使用内存总数

	multiboot_memory_map_t* map = (multiboot_memory_map_t*)mb_info->mmap_addr;
	uint8_t map_size = mb_info->mmap_length / sizeof(multiboot_memory_map_t);
	printf("[PMM] Physical Memory Map:\n");
	for (uint8_t i = 0; i < map_size; i++) {
		// printf("\tBase: 0x%08x, len: 0x%08x B, type: %d(%s)\n",
		// 				map[i].addr_low,
		// 				map[i].len_low,
		// 				map[i].type,
		// 				get_mem_type_str(map[i].type));
		if(map[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
			// 标记available内存
			pmm_init_region(map[i].addr_low, map[i].len_low);
		}
	}

	printf("[PMM] Pmm initialized with %d KB physical memory\n", pmm_get_memory_size()/1024);
	printf("[PMM] Pmm initialized blocks: %d, used or reserved blocks: %d, free blocks: %d\n", 
		pmm_get_block_count(),
		pmm_get_used_block_count(),
		pmm_get_free_block_count());

	// printf("[PMM] Start: 0x%x B; end: 0x%x B; size: 0x%x B\n", (uint32_t)&__kernel_start, (uint32_t)&__kernel_end, (uint32_t)(&__kernel_end - &__kernel_start));
	// 将内核占用内存调整为占用
	pmm_deinit_region((uint32_t)&__kernel_start, (uint32_t)(&__kernel_end - &__kernel_start));
	printf("[PMM] Allocate %d block for kernel\n", (uint32_t)(&__kernel_end - &__kernel_start)/4096);

	printf("[PMM] Pmm initialized blocks: %d, used or reserved blocks: %d, free blocks: %d\n", 
		pmm_get_block_count(),
		pmm_get_used_block_count(),
		pmm_get_free_block_count());

	init_paging();

	vmm_map_page((void*)VGA_BUFFER_PADDR, (void*)VGA_BUFFER_VADDR);
	fb_set_buffer((uint8_t*)VGA_BUFFER_VADDR);

	printf("[VMM] Vmm DONE\n");
	// init_timer(50); // 19HZ, setting frequency divsior
	init_keyboard();
}