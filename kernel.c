#include "io.h"
#include "framebuffer.h"
#include "serial.h"
#include "string.h"
#include "multiboot.h"
#include "descriptor_tables.h"

extern void disable_blink();
extern void enable_blink();

void kernel(multiboot_info_t *info) {
	// fb_move_cursor(0x0320);
	// disable_blink();
	// fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
	// fb_write_cell(1, 'B', FB_GREEN, FB_DARK_GREY);
	init_descriptor_tables();
	fb_clear();
	char greeting[] = "No. 5 is alive!\n";
	fb_write(greeting, strlen(greeting));
	serial_write(greeting, strlen(greeting));

	char flags[100];
	itoa(info->flags, flags, 16);
	fb_write(flags, strlen(flags));

	printf("\nHelle, world!\n");

	asm volatile ("int $0x3");
  asm volatile ("int $0x4");
    // unsigned int pos = 0;
    // unsigned int i = 0;
		// unsigned short len = 15;
		// char *buf = "No. 5 is alive";
		// fb_write_cell(pos++, 'N', FB_WHITE, FB_BLACK);
    // for(i = 0; i < len; ++i) {
    //     fb_write_cell(pos++, buf[i], FB_WHITE, FB_BLACK);
    // }
	return;
}