#include "kernel/multiboot.h"
#include "kernel/framebuffer.h"
#include "lib/string.h"

void kmain(multiboot_info_t * mb_info) {
	fb_clear();
	printf("&mb_info: 0x%x\n", &mb_info);
	printf("mb_info: 0x%x\n", mb_info);
	printf("mb_info->flags: 0x%x\n", mb_info->flags);
	printf("mb_info->mmap_length: 0x%x\n", mb_info->mmap_length);
	printf("[MM] Mem: 0x%x KB, Extended Mem: 0x%x KB\n",
					mb_info->mem_lower,
					mb_info->mem_upper);
}