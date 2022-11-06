#include "kernel/multiboot.h"
#include "kernel/framebuffer.h"
#include "lib/string.h"

void kmain(multiboot_info_t * mb_info) {
	fb_clear();
	printf("hello, world!\n");
	// printf("mb_info address: 0x%x\n", &mb_info);
	// printf("mb_info address: 0x%x\n", mb_info->flags);
	// printf("[MM] Mem: 0x%x KB, Extended Mem: 0x%x KB\n",
	// 				mb_info->mem_lower,
	// 				mb_info->mem_upper);
}