#include "io.h"
#include "framebuffer.h"

extern void disable_blink();
extern void enable_blink();

int main() {
	// fb_move_cursor(0x0320);
	// disable_blink();
	// fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
	// fb_write_cell(1, 'B', FB_GREEN, FB_DARK_GREY);
	fb_write("No. 5 is alive");
    // unsigned int pos = 0;
    // unsigned int i = 0;
		// unsigned short len = 15;
		// char *buf = "No. 5 is alive";
		// fb_write_cell(pos++, 'N', FB_WHITE, FB_BLACK);
    // for(i = 0; i < len; ++i) {
    //     fb_write_cell(pos++, buf[i], FB_WHITE, FB_BLACK);
    // }
	return 0;
}