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
#include "kernel/timer.h"
#include "kernel/tss.h"
#include "kernel/mm/malloc.h"
#include "kernel/network/pci.h"
#include "kernel/network/rtl8139.h"
#include "kernel/syscall.h"
#include "kernel/process.h"
#include "lib/log.h"

// https://stackoverflow.com/questions/8398755/access-symbols-defined-in-the-linker-script-by-application
// https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
// 使用(uint32_t)&__kernel_start, 原因见链接中解释
extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

extern void enter_userland(uint32_t*);
extern void spinlock_lock(uint32_t*);
extern void spinlock_unlock(uint32_t*);

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

void print_jiffies() {
	fb_set_color(FB_RED, FB_BLACK);
	printf("[Kernel] Time callback jiffies: %d\n", jiffies);
	fb_reset_color();
}

void test_syscall() {
	char* helo = "syscal printf: helo syscall!\n";

	asm volatile (" \
		mov $0, %%eax; \
		mov %0, %%ebx; \
		int $0x80; \
		" : :"g"(helo):"eax","ebx");
}

void user_process2() {
    // uint32_t lock = 0;
    while(1) {
        for(int i = 0; i < 1000; i++) {
            for(int j= 0; j < 2000; j++) {

            }
        }
        // spinlock_lock(&lock);
        printf("hi there2\n");
        // spinlock_unlock(&lock);
    }
}

void user_process() {
    // uint32_t lock = 0;
    for(int i = 0; i < 2; i++) {
        create_process_from_routine(user_process2, "user process"+i);
    }
    while(1) {
        for(int i = 0; i < 1000; i++) {
            for(int j= 0; j < 2000; j++) {

            }
        }
        // spinlock_lock(&lock);
        printf("hi there, user_process\n");
        // spinlock_unlock(&lock);
    }
}

void
divide_hanlder()
{
	printf("Divide Error\n");
	PANIC("");
}

void kmain(multiboot_info_t * mb_info) {
	fb_clear();

	char buf[64];
	cpu_get_brand(buf);
	printf("[KERNEL] CPU: %s\n", buf);

	init_gdt();
	init_idt();
	tss_init(5, 0x10, 0);

	register_interrupt_handler(0, divide_hanlder);

	init_pmm(MEM_1MB + (mb_info->mem_upper << 10));

	// printf("[KERNEL] Mem: 0x%x KB, Extended Mem: 0x%x KB\n\x09Total Mem: 0x%x B\n",
	// 				mb_info->mem_lower,
	// 				mb_info->mem_upper,
	// 				(mb_info->mem_upper<<10) + 0x100000); // 可使用内存总数

	multiboot_memory_map_t* map = (multiboot_memory_map_t*)mb_info->mmap_addr;
	uint8_t map_size = mb_info->mmap_length / sizeof(multiboot_memory_map_t);
	printf("[PMM] Physical Memory Map:\n");
	for (uint8_t i = 0; i < map_size; i++) {
		printf("\tBase: 0x%08x, len: 0x%08x B, type: %d(%s)\n",
						map[i].addr_low,
						map[i].len_low,
						map[i].type,
						get_mem_type_str(map[i].type));
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

	printf("[PMM] Start: 0x%x B; end: 0x%x B; size: 0x%x B\n", (uint32_t)&__kernel_start, (uint32_t)&__kernel_end, (uint32_t)(&__kernel_end - &__kernel_start));
	// 将内核占用内存调整为占用
	pmm_deinit_region((uint32_t)&__kernel_start, (uint32_t)(&__kernel_end - &__kernel_start));
	// 调整，想着kernel_end-0x400000，位heap区域, 后面取消pmm_alloc_bloc，或者基于这个重构malloc，统一分配内核内存
	pmm_deinit_region((uint32_t)&__kernel_end, 0x400000 - (uint32_t)(&__kernel_end));
	printf("[PMM] Allocate %d block for kernel\n", (0x400000 - (uint32_t)(&__kernel_start))/4096);

	printf("[PMM] Pmm initialized blocks: %d, used or reserved blocks: %d, free blocks: %d\n", 
		pmm_get_block_count(),
		pmm_get_used_block_count(),
		pmm_get_free_block_count());

	init_paging();
	vmm_map_page(kpage_dir, (void*)VGA_BUFFER_PADDR, (void*)VGA_BUFFER_VADDR, PTE_PRESENT | PTE_WRITABLE |PTE_USER);
	vmm_map_page(kpage_dir, (void*)VGA_BUFFER_PADDR+0x1000, (void*)VGA_BUFFER_VADDR+0x1000, PTE_PRESENT | PTE_WRITABLE |PTE_USER);
	fb_set_buffer((uint8_t*)VGA_BUFFER_VADDR);
	printf("[VMM] Vmm DONE\n");

	// 分配kernel heap
	init_mm((uint32_t)&__kernel_end, 0xC0400000 - 0x1000);
	// mm_print_info();

	// uint32_t *ptr = (uint32_t *)0xA0000000;
	// uint32_t do_page_fault = *ptr;
	// printf("%x\n", do_page_fault);
	init_timer(100);
	// init_keyboard();

	register_timer_callback(print_jiffies, 3);

	// pci_init();
	// rtl8139_init();
	
	// printf("[KERNEL] ALL DONE!!!\n");
	// asm volatile("int $0x2b");

	process_init();
	syscall_init();

	// uint32_t esp;
	// asm volatile("mov %%esp, %0" : "=r"(esp));
	tss_set_stack(0x10, 0x400000);

	// Start the first process
	create_process_from_routine(user_process, "user process");

	// printf("Done!\n");

	// test_syscall();
	// userland
	// uint32_t esp;
	// asm volatile("mov %%esp, %0" : "=r"(esp));
	// tss_set_stack(0x10, esp);
	// enter_userland();
	// printf("Uer land done\n");
}